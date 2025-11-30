#include "util/Sqlutil.h"

#include <cstddef>

namespace sqlite3_util {
    std::string quoteIdentifier(const std::string& ident)
    {
        std::string out{};
        out.reserve(ident.size() + 2);
        out += "\"";
        for (char c : ident)
        {
            out += c;
            if(c == '\"')
            {
                out += "\""; // SQL-parser will read two "" as one "
            }
        }
        out += "\"";
        return out;
    }

    std::string quoteIdentifierList(const std::vector<std::string> &identList)
    {
        std::string out{};
        std::size_t reserveVal{0};
        for(const auto& ident : identList)
        {
            reserveVal += ident.size() + 3; // +2 for " and +1 for ,
        }
        out.reserve(reserveVal);
        for(std::size_t idx{0}; idx < identList.size(); ++idx)
        {
            out += quoteIdentifier(identList[idx]);
            if(idx != identList.size() - 1)
            {
                out += ',';
            }
        }
        return out;
    }
}

namespace sqlServer_util {
    std::string bracketIdentifier(const std::string &ident)
    {
        std::string out{};
        out.reserve(ident.size() + 2);
        out += "[";
        for (char c : ident)
        {
            out += c;
            if(c == '[')
            {
                out += "["; // SQLServer-parser will read two "" as one "
            }
            if(c == ']')
            {
                out += "]"; // SQLServer-parser will read two "" as one "
            }
        }
        out += "]";
        return out;
    }

    std::string bracketIdentifierList(const std::vector<std::string> &identList)
    {
        std::string out{};
        std::size_t reserveVal{0};
        for(const auto& ident : identList)
        {
            reserveVal += ident.size() + 3; // +2 for [] and +1 for ,
        }
        out.reserve(reserveVal);
        for(std::size_t idx{0}; idx < identList.size(); ++idx)
        {
            out += bracketIdentifier(identList[idx]);
            if(idx != identList.size() - 1)
            {
                out += ',';
            }
        }
        return out;
    }
}

