#ifndef ODBCDIAGNOSTICS_H
#define ODBCDIAGNOSTICS_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <vector>

namespace odbc {
    struct OdbcDiagRecord {
        std::string sqlState;
        SQLINTEGER  nativeError;
        std::string message;
    };

    std::vector<OdbcDiagRecord> collectDiagnostics(SQLSMALLINT handleType,
                                                   SQLHANDLE handle);

    std::string addDiagToErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle);

    void logSuccessInfo(const char* where, SQLSMALLINT handleType, SQLHANDLE handle);
}

#endif // ODBCDIAGNOSTICS_H
