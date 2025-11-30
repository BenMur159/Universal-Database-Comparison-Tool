#include "TableComperator.h"

#include <stdexcept>
#include <optional>

#include "IRowReader.h"


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

TableComparisonResult compareTable(IDatabaseConnection &dbA, IDatabaseConnection &dbB, const TableInfo &tableA, const TableInfo &tableB)
{
    throw std::logic_error("TableComparisonResult compareTable not implemented yet");
}
