#include "sqlserver/OdbcDiagnostics.h"

#include <iostream>

namespace odbc
{
    std::vector<OdbcDiagRecord> collectDiagnostics(SQLSMALLINT handleType,
                                               SQLHANDLE handle)
    {
        if(handle == SQL_NULL_HANDLE)
        {
            return {};
        }
        std::vector<OdbcDiagRecord> out;

        SQLSMALLINT recNumber = 1;
        while (true) {
            SQLCHAR sqlState[6] = {};
            SQLCHAR message[512] = {};
            SQLINTEGER nativeError = 0;
            SQLSMALLINT textLen = 0;

            SQLRETURN rc = SQLGetDiagRecA(
                handleType,
                handle,
                recNumber,
                sqlState,
                &nativeError,
                message,
                sizeof(message),
                &textLen
                );

            if (rc == SQL_NO_DATA) {
                break; // no more records
            }
            if (!SQL_SUCCEEDED(rc)) {
                break; // give up on diagnostics
            }
            OdbcDiagRecord rec;
            rec.sqlState    = reinterpret_cast<const char*>(sqlState);
            rec.nativeError = nativeError;
            rec.message     = reinterpret_cast<const char*>(message);

            out.emplace_back(rec);
            ++recNumber;
        }

        return out;
    }

    std::string addDiagToErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle)
    {
        if(handle == SQL_NULL_HANDLE)
        {
            return {};
        }
        std::string errMsgAdd{};
        auto diags = collectDiagnostics(handleType, handle);
        if(!diags.empty())
        {
            errMsgAdd = " [" + diags[0].sqlState + "] "
                        + std::to_string(diags[0].nativeError) + ": "
                        + diags[0].message;
        }
        else
        {
            errMsgAdd = " unknown error";
        }
        return errMsgAdd;
    }

    void logSuccessInfo(const char* where, SQLSMALLINT handleType, SQLHANDLE handle)
    {
        if (handle == SQL_NULL_HANDLE)
            return;

        auto diags = collectDiagnostics(handleType, handle);
        for (const auto& d : diags) {
            if (d.nativeError == 5701 || d.nativeError == 5703) {
                // ignore this one, it's just context/language info
                // see microsoft doc for more information on these
                continue;
            }
            // log or print the warning
            std::cerr << where << " ODBC info [" << d.sqlState << "] "
                      << d.nativeError << ": " << d.message << "\n";
        }
    }

}
