#ifndef SQLSERVERRUNTIMEERROR_H
#define SQLSERVERRUNTIMEERROR_H

#include <string>
#include <stdexcept>

#include "sqlserver/OdbcDiagnostics.h"

class SqlServerRuntimeError : public std::runtime_error
{
public:
    /****************************************
     * @brief:  default with diagnostics
     */
    SqlServerRuntimeError(const std::string& where, SQLSMALLINT handleType, SQLHANDLE handle)
    : std::runtime_error(where + ":" + odbc::addDiagToErrorMessage(handleType, handle))
    {

    }
    /*********************************************************
     *  @brief: no diagnostics available because of SQL_NULL_HANDLE
     */
    SqlServerRuntimeError(const std::string& where)
        : std::runtime_error(where)
    {

    }

};

#endif // SQLSERVERRUNTIMEERROR_H
