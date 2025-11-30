#include "sqlserver/SqlServerConnection.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>

#include "sqlserver/OdbcDiagnostics.h"
#include "sqlserver/SqlServerRuntimeError.h"
#include "sqlserver/SqlServerSchemaInspector.h"
#include "sqlserver/SqlServerRowReader.h"
#include "TableReadSpec.h"
#include "util/Sqlutil.h"

SqlServerConnection::SqlServerConnection(const std::string &connStr)
{
    SQLRETURN rc;

    // allocate environment
    rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_env);
    if(!SQL_SUCCEEDED(rc))
    {
        // Doesn't have diagnostics because handle is null (SQL_NULL_HANDLE)
        SqlServerRuntimeError ex("SqlServerConnection ctor: SQLAllocHandle(SQL_HANDLE_ENV) failed");
        close();
        throw ex;

    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerConnection ctor: SQLAllocHandle(SQL_HANDLE_ENV)", SQL_HANDLE_ENV, m_env);
    }

    // set environment ODBC version to 3
    rc = SQLSetEnvAttr(m_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if(!SQL_SUCCEEDED(rc))
    {
        SqlServerRuntimeError ex{"SqlServerConnection ctor: SQLSetEnvAttr(SQL_ATTR_ODBC_VERSION)", SQL_HANDLE_ENV, m_env};
        close();
        throw ex;
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerConnection ctor: SQLSetEnvAttr(SQL_ATTR_ODBC_VERSION)", SQL_HANDLE_ENV, m_env);
    }

    // allocate database connection handler
    rc = SQLAllocHandle(SQL_HANDLE_DBC, m_env, &m_dbc);
    if (!SQL_SUCCEEDED(rc))
    {
        SqlServerRuntimeError ex{"SqlServerConnection ctor: SQLAllocHandle(SQL_HANDLE_DBC)", SQL_HANDLE_ENV, m_env};
        close();
        throw ex;
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerConnection ctor: SQLAllocHandle(SQL_HANDLE_DBC)", SQL_HANDLE_DBC, m_dbc);
    }

    // connect to database
    rc = SQLDriverConnectA(
        m_dbc,
        nullptr,
        (SQLCHAR*)connStr.c_str(),
        SQL_NTS,
        nullptr,
        0,
        nullptr,
        SQL_DRIVER_NOPROMPT
    );
    if (!SQL_SUCCEEDED(rc))
    {
        SqlServerRuntimeError ex{"SqlServerConnection ctor: SQLDriverConnectA", SQL_HANDLE_DBC, m_dbc};
        close();
        throw ex;
    }

    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerConnection ctor: SQLDriverConnectA", SQL_HANDLE_DBC, m_dbc);
    }

}

SqlServerConnection::~SqlServerConnection() noexcept
{
    close();
}

std::unique_ptr<ISchemaInspector> SqlServerConnection::createSchemaInspector()
{
    return std::make_unique<SqlServerSchemaInspector>(m_dbc);
}

std::unique_ptr<IRowReader> SqlServerConnection::createRowReader(const TableReadSpec &spec)
{
    SQLRETURN rc;
    SQLHSTMT stmt{SQL_NULL_HSTMT};

    std::string brackTableName{sqlServer_util::bracketIdentifier(spec.tableName)};
    std::string brackCols;
    if(spec.selectedCols.empty())
        brackCols = "*";
    else
        brackCols = sqlServer_util::bracketIdentifierList(spec.selectedCols);
    std::string sql {
        "SELECT " + brackCols + " "
        "FROM " + brackTableName + ";"
    };

    rc = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerConnection::createRowReader(...) SQLAllocHandle", SQL_HANDLE_DBC, m_dbc};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerConnection::createRowReader(...) SQLAllocHandle", SQL_HANDLE_STMT, stmt);

    rc = SQLExecDirectA(stmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if(!SQL_SUCCEEDED(rc))
    {
        SqlServerRuntimeError ex{"SqlServerConnection::createRowReader(...) SQLExecDirectA", SQL_HANDLE_STMT, stmt};
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw ex;
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerConnection::createRowReader(...) SQLExecDirectA", SQL_HANDLE_STMT, stmt);

    return std::make_unique<SqlServerRowReader>(stmt);
}

void SqlServerConnection::close() noexcept
{
    if(m_dbc)
    {
        SQLDisconnect(m_dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_dbc);
        m_dbc = SQL_NULL_HDBC;
    }
    if(m_env)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_env);
        m_env = SQL_NULL_HENV;
    }
}
