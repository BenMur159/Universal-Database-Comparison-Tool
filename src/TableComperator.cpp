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

TableComparisonResult compareTable(IDatabaseConnection &dbA, IDatabaseConnection &dbB,
                                   const TableInfo &tabInfA, const TableInfo &tabInfB)
{
    TableReadSpec tabSpecA{tabInfA.name, {}}; // empty string = * for cols
    std::unordered_map<std::string, std::size_t> canoRowsA{};

    auto readerA{dbA.createRowReader(tabSpecA)};
    while(readerA->next())
    {
        std::string canonicalRowA{makeCanonicalRow(*readerA, tabInfA)};
        if(auto it = canoRowsA.find(canonicalRowA);
            it != canoRowsA.end())
        {
            ++(it->second);
        }
        else
           canoRowsA.emplace(canonicalRowA, 1);
    }

    TableReadSpec tabSpecB{tabInfB.name};
    std::unordered_map<std::string, std::size_t> canoRowsB;

    auto readerB{dbB.createRowReader(tabSpecB)};
    while(readerB->next())
    {
        std::string canonicalRowB{makeCanonicalRow(*readerB, tabInfB)};
        if(auto it = canoRowsB.find(canonicalRowB);
            it != canoRowsB.end())
        {
            ++(it->second);
        }
        else
            canoRowsB.emplace(canonicalRowB, 1);
    }

    TableComparisonResult out;
    for(auto itA = canoRowsA.begin(); itA != canoRowsA.end(); ++itA)
    {
        auto itB = canoRowsB.find(itA->first);
        // not in B, only in A
        if(itB == canoRowsB.end())
        {
            out.rowDifferences.emplace_back(itA->first, itA->second, 0);
        }
        // in both
        else
        {
            out.rowDifferences.emplace_back(itA->first, itA->second, itB->second);
        }
    }

    //not in A, only in B
    for(auto itB = canoRowsB.begin(); itB != canoRowsB.end(); ++itB)
    {
        auto itA = canoRowsA.find(itB->first);
        if(itA == canoRowsA.end())
        {
            out.rowDifferences.emplace_back(itB->first, 0, itB->second);
        }
    }
    return out;
}
