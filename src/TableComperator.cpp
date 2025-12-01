#include "TableComperator.h"

#include <stdexcept>
#include <optional>
#include <unordered_map>
#include <cstddef>
#include <memory>

#include "IRowReader.h"
#include "TableReadSpec.h"

namespace {
    std::string makeCanonicalRow(const IRowReader& reader, const TableInfo& tabInf)
    {
        std::string out{};
        int colCount{static_cast<int>(tabInf.columns.size())};
        out.reserve(16*colCount); // rough guesstimate assuming most read values are 10 chars long
        if(reader.getColumnCount() != colCount)
        {
            throw std::logic_error("makeCanonicalRow: colCount != reader.getColumnCount()");
        }
        for(int col{0}; col < colCount; ++col)
        {
            if(col > 0)
                out.push_back('|');

            std::optional<std::string> value{reader.getString(col)};
            if(!value)
                out += "N";
            else
                out += std::to_string(value->size()) + "#" + *value;
        }

        return out;
    }
};

TableComparisonResult compareTableRowData(IDatabaseConnection &dbA, IDatabaseConnection &dbB,
                                   const TableInfo &tabInfA, const TableInfo &tabInfB)
{
    TableReadSpec tabSpecA{tabInfA.name}; // only name CTOR = all columns
    std::unordered_map<std::string, std::size_t> canoRowsA{};

    auto readerA{dbA.createRowReader(tabSpecA)};
    while(readerA->next())
    {
        std::string canonicalRowA{makeCanonicalRow(*readerA, tabInfA)};
        // iterator points to existing cell if already in map
        auto [it, isInserted] = canoRowsA.emplace(std::move(canonicalRowA), 0);
        ++(it->second);
    }

    TableReadSpec tabSpecB{tabInfB.name};
    std::unordered_map<std::string, std::size_t> canoRowsB;

    auto readerB{dbB.createRowReader(tabSpecB)};
    while(readerB->next())
    {
        std::string canonicalRowB{makeCanonicalRow(*readerB, tabInfB)};
        auto [it, isInserted] = canoRowsB.emplace(std::move(canonicalRowB), 0);
        ++(it->second);
    }

    TableComparisonResult out;
    for(const auto& [row, countA] : canoRowsA)
    {
        auto itB = canoRowsB.find(row);
        // not in B, only in A
        if(itB == canoRowsB.end())
        {
            out.rowDifferences.emplace_back(row, countA, 0);
        }
        // in both
        else if(countA != itB->second)
        {
            out.rowDifferences.emplace_back(row, countA, itB->second);
        }
    }
    //not in A, only in B
    for(const auto& [row, countB] : canoRowsB)
    {
        if(!canoRowsA.count(row))
        {
            out.rowDifferences.emplace_back(row, 0, countB);
        }
    }
    return out;
}
