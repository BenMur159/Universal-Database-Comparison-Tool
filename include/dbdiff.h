#ifndef DBDIFF_H
#define DBDIFF_H


#include <string>

struct SqliteVersion {
    std::string version;
};

// Simple test function: ask SQLite for its version string
SqliteVersion getSqliteVersion();

#endif // DBDIFF_H
