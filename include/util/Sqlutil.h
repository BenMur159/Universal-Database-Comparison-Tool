#ifndef SQLUTIL_H
#define SQLUTIL_H

#include <string>
#include <vector>

namespace sqlite3_util {
    std::string quoteIdentifier(const std::string& ident);
    std::string quoteIdentifierList(const std::vector<std::string>& identList);
}

namespace sqlServer_util {
    std::string bracketIdentifier(const std::string& ident);
    std::string bracketIdentifierList(const std::vector<std::string>& identList);
}

#endif // SQLUTIL_H
