#include "sqlserver/SqlServerSchemaInspector.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <cstring>
#include <cstddef>

#include "sqlserver/OdbcDiagnostics.h"
#include "sqlserver/SqlServerRuntimeError.h"
#include "DatabaseSchemaInfoTypes.h"

namespace{
    struct StmtGuard{
        SQLHSTMT stmt{SQL_NULL_HSTMT};

        StmtGuard() = default;
        explicit StmtGuard(SQLHSTMT s) : stmt{s} {}

        ~StmtGuard() noexcept {
            if (stmt) {
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                stmt = SQL_NULL_HSTMT;
            }
        }
        StmtGuard(const StmtGuard&) = delete;
        StmtGuard& operator=(const StmtGuard&) = delete;
        StmtGuard(StmtGuard&&) = delete;
        StmtGuard& operator=(StmtGuard&&) = delete;
    };
}


SqlServerSchemaInspector::SqlServerSchemaInspector(SQLHDBC dbc)
    : m_dbc{dbc}
{

}

std::vector<std::string> SqlServerSchemaInspector::listAllTables() const
{
    SQLRETURN rc;
    SQLHSTMT stmt{SQL_NULL_HSTMT};

    rc = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if(!SQL_SUCCEEDED(rc))
    {
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllTables() SQLAllocHandle", SQL_HANDLE_DBC, m_dbc};
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllTables() SQLAllocHandle", SQL_HANDLE_STMT, stmt);
    }
    StmtGuard guard{stmt};

    std::string sql {
        "SELECT TABLE_NAME "
        "FROM INFORMATION_SCHEMA.TABLES "
        "WHERE TABLE_TYPE = \'BASE TABLE\' "
        "ORDER BY TABLE_NAME;"
    };

    rc = SQLExecDirectA(stmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if(!SQL_SUCCEEDED(rc))
    {
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllTables() SQLExecDirectA", SQL_HANDLE_STMT, stmt};
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllTables() SQLExecDirectA", SQL_HANDLE_STMT, stmt);
    }

    SQLCHAR buffer[256];
    SQLLEN tableNameLen{0};
    rc = SQLBindCol(
        stmt,
        1,
        SQL_C_CHAR,
        buffer,
        sizeof(buffer),
        &tableNameLen
    );
    if(!SQL_SUCCEEDED(rc))
    {
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllTables() SQLBindCol(1)", SQL_HANDLE_STMT, stmt};
    }
    if(rc == SQL_SUCCESS_WITH_INFO)
    {
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllTables() SQLBindCol(1)", SQL_HANDLE_STMT, stmt);
    }

    std::vector<std::string> out;
    while(true)
    {
        rc = SQLFetch(stmt);
        if(rc == SQL_NO_DATA)
            break;
        if(!SQL_SUCCEEDED(rc))
        {
            throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllTables() SQLFetch", SQL_HANDLE_STMT, stmt};
        }
        if(rc == SQL_SUCCESS_WITH_INFO)
        {
            odbc::logSuccessInfo("SqlServerSchemaInspector::listAllTables() SQLFetch", SQL_HANDLE_STMT, stmt);
        }

        if(tableNameLen == SQL_NULL_DATA)
            out.emplace_back("<NULL>");
        else
            out.emplace_back(reinterpret_cast<const char*>(buffer),
                             static_cast<std::size_t>(tableNameLen));
    }
    return out;

}

std::vector<std::string> SqlServerSchemaInspector::listAllColumnNames(const std::string &tableName) const
{
    SQLRETURN rc;
    SQLHSTMT stmt{SQL_NULL_HSTMT};

    rc = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLAllocHandle", SQL_HANDLE_DBC, m_dbc};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLAllocHandle", SQL_HANDLE_STMT, stmt);

    StmtGuard guard{stmt};

    std::string sql{
        "SELECT COLUMN_NAME "
        "FROM INFORMATION_SCHEMA.COLUMNS "
        "WHERE TABLE_NAME = ? "
        "ORDER BY ORDINAL_POSITION;"
    };

    rc = SQLPrepareA(stmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLPrepareA", SQL_HANDLE_STMT, stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLPrepareA", SQL_HANDLE_STMT, stmt);

    SQLLEN tableLen{SQL_NTS};
    rc = SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_CHAR,
        SQL_VARCHAR,
        128,
        0,
        (SQLPOINTER)tableName.c_str(),
        static_cast<SQLLEN>(tableName.size() + 1),
        &tableLen
    );
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLBindParameter", SQL_HANDLE_STMT, stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLBindParameter", SQL_HANDLE_STMT, stmt);

    rc = SQLExecute(stmt);
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLExecute", SQL_HANDLE_STMT, stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLExecute", SQL_HANDLE_STMT, stmt);

    std::vector<std::string> out{};
    SQLCHAR buffer[256];
    SQLLEN colNameLen{0};
    rc = SQLBindCol(
        stmt,
        1,
        SQL_C_CHAR,
        buffer,
        sizeof(buffer),
        &colNameLen
    );
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLBindCol(1)", SQL_HANDLE_STMT, stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLBindCol(1)", SQL_HANDLE_STMT, stmt);

    while(true)
    {
        rc = SQLFetch(stmt);
        if(rc == SQL_NO_DATA)
            break;
        if(!SQL_SUCCEEDED(rc))
            throw SqlServerRuntimeError{"SqlServerSchemaInspector::listAllColumnNames(...) SQLFetch", SQL_HANDLE_STMT, stmt};
        if(rc == SQL_SUCCESS_WITH_INFO)
            odbc::logSuccessInfo("SqlServerSchemaInspector::listAllColumnNames(...) SQLFetch", SQL_HANDLE_STMT, stmt);

        if(colNameLen == SQL_NULL_DATA)
            out.emplace_back("<NULL>");
        else
            out.emplace_back(reinterpret_cast<const char*>(buffer),
                             static_cast<std::size_t>(colNameLen));
    }
    return out;
}

TableInfo SqlServerSchemaInspector::getTableInfo(const std::string& tableName) const
{
    SQLRETURN rc;
    SQLHSTMT stmt{SQL_NULL_HSTMT};

    rc = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLAllocHandle", SQL_HANDLE_DBC, m_dbc);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLAllocHandle", SQL_HANDLE_STMT, stmt);

    StmtGuard guard{stmt};

    const char* sql =
        "SELECT "
        "  c.COLUMN_NAME, "
        "  c.IS_NULLABLE, "
        "  c.DATA_TYPE, "
        "  CASE WHEN pk.COLUMN_NAME IS NOT NULL THEN 1 ELSE 0 END AS IS_PRIMARY_KEY "
        "FROM INFORMATION_SCHEMA.COLUMNS AS c "
        "LEFT JOIN ( "
        "  SELECT k.TABLE_NAME, k.COLUMN_NAME "
        "  FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS AS tc "
        "  JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE AS k "
        "    ON tc.CONSTRAINT_NAME = k.CONSTRAINT_NAME "
        "   AND tc.TABLE_NAME = k.TABLE_NAME "
        "  WHERE tc.CONSTRAINT_TYPE = 'PRIMARY KEY' "
        ") AS pk "
        "  ON pk.TABLE_NAME = c.TABLE_NAME "
        " AND pk.COLUMN_NAME = c.COLUMN_NAME "
        "WHERE c.TABLE_NAME = ? "
        "ORDER BY c.ORDINAL_POSITION";

    rc = SQLPrepareA(stmt, (SQLCHAR*)sql, SQL_NTS);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLPrepareA",SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLPrepareA", SQL_HANDLE_STMT, stmt);

    // Bind parameter @TABLE_NAME
    SQLLEN nameLen = SQL_NTS;
    rc = SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_CHAR,
        SQL_VARCHAR,
        0,
        0,
        (SQLPOINTER)tableName.data(),
        0,
        &nameLen
    );
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLBindParameter", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLBindParameter", SQL_HANDLE_STMT, stmt);

    rc = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLExecute", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLExecute", SQL_HANDLE_STMT, stmt);

    // Output buffers
    SQLCHAR colNameBuf[256];
    SQLCHAR colTypeBuf[256];
    SQLCHAR isNullableBuf[4];
    SQLINTEGER isPkInt = 0;

    SQLLEN colNameInd = 0;
    SQLLEN isNullableInd = 0;
    SQLLEN colTypeInd = 0;
    SQLLEN isPkInd = 0;

    rc = SQLBindCol(stmt, 1, SQL_C_CHAR, colNameBuf, sizeof(colNameBuf), &colNameInd);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLBindCol(1)", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLBindCol(1)", SQL_HANDLE_STMT, stmt);

    rc = SQLBindCol(stmt, 2, SQL_C_CHAR, isNullableBuf, sizeof(isNullableBuf), &isNullableInd);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLBindCol(2)", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLBindCol(2)", SQL_HANDLE_STMT, stmt);

    rc = SQLBindCol(stmt, 3, SQL_C_CHAR, colTypeBuf,sizeof(colTypeBuf), &colTypeInd);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLBindCol(3)", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLBindCol(3)", SQL_HANDLE_STMT, stmt);

    rc = SQLBindCol(stmt, 4, SQL_C_SLONG, &isPkInt, 0, &isPkInd);
    if (!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLBindCol(4)", SQL_HANDLE_STMT, stmt);
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLBindCol(4)", SQL_HANDLE_STMT, stmt);


    TableInfo tabInf{};
    tabInf.name = tableName;
    std::size_t colIdx{0};
    while(true)
    {
        rc = SQLFetch(stmt);
        if(rc == SQL_NO_DATA)
            break;
        if(!SQL_SUCCEEDED(rc))
            throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo SQLFetch", SQL_HANDLE_STMT, stmt);
        if(rc == SQL_SUCCESS_WITH_INFO)
            odbc::logSuccessInfo("SqlServerSchemaInspector::getTableInfo SQLFetch", SQL_HANDLE_STMT, stmt);

        ColumnInfo colInf{};

        // These should never be NULL
        if(colNameInd == SQL_NULL_DATA || isNullableInd == SQL_NULL_DATA
            || colTypeInd == SQL_NULL_DATA || isPkInd == SQL_NULL_DATA)
        {
            throw SqlServerRuntimeError("SqlServerSchemaInspector::getTableInfo after SQLFetch a SQL_NULL_DATA was detected");
        }
        colInf.name = reinterpret_cast<const char*>(colNameBuf);
        colInf.isNullable = (std::strncmp(reinterpret_cast<const char*>(isNullableBuf), "YES", 3) == 0);
        colInf.dbType = reinterpret_cast<const char*>(colTypeBuf);
        colInf.isPartOfPrimaryKey = (isPkInt != 0);
        if(colInf.isPartOfPrimaryKey)
        {
            tabInf.primaryKeyColumnIndices.push_back(colIdx);
        }
        tabInf.columns.push_back(std::move(colInf));
        ++colIdx;
    }
    if(tabInf.columns.empty())
    {
        throw std::runtime_error(
            "SqlServerSchemaInspector::getTableInfo table " + tableName + " has no columns, might not exist"
        );
    }
    return tabInf;
}
