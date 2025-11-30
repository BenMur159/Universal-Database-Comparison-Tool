#include "dbdiff.h"

// SQLite is a C library, tell the C++ compiler to use C linkage

extern "C" {
#include "sqlite3.h"
}


SqliteVersion getSqliteVersion()
{
    SqliteVersion v{};
    v.version = sqlite3_libversion();  // this comes from sqlite3.c
    return v;
}
