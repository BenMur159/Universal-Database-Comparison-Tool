#include "sqlserver/SqlServerRowReader.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <array>
#include <cstring>

#include "sqlserver/SqlServerRuntimeError.h"

SqlServerRowReader::SqlServerRowReader(SQLHSTMT stmt)
    : m_stmt{stmt}, m_cols{}
{

}

SqlServerRowReader::~SqlServerRowReader() noexcept
{
    if(m_stmt)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, m_stmt);
        m_stmt = SQL_NULL_HSTMT;
    }
}

bool SqlServerRowReader::next()
{
    SQLRETURN rc = SQLFetch(m_stmt);
    if(rc == SQL_NO_DATA)
        return false;
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerRowReader::next SQLFetch", SQL_HANDLE_STMT, m_stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerRowReader::next SQLFetch", SQL_HANDLE_STMT, m_stmt);

    if(m_cols.empty())
    {
        int count{getColumnCount()};
        m_cols.reserve(count);
        for(int i{0}; i < count; ++i)
        {
            m_cols.emplace_back();  // ColState{false, ""}
        }
    }
    else
    {
        int count{getColumnCount()};
        for(int i{0}; i < count; ++i)
        {
            m_cols[i].fetched = false;  // ColState{false, ""}
            m_cols[i].value.clear();
        }
    }

    return true;
}

std::optional<std::string> SqlServerRowReader::getString(int col) const
{
    std::array<SQLCHAR, 256> buffer{};
    SQLLEN indicator = 0;
    SQLRETURN rc;

    if(m_cols[col].fetched)
    {
        if(m_cols[col].isNull)
            return {};
        else
            return m_cols[col].value;
    }

    while(true)
    {

        rc = SQLGetData(
            m_stmt,
            col + 1,
            SQL_C_CHAR,
            buffer.data(),
            buffer.size(),
            &indicator
        );

        if(rc == SQL_NO_DATA)
            break;

        if(!SQL_SUCCEEDED(rc))
            throw SqlServerRuntimeError{"SqlServerRowReader::getString SQLGetData", SQL_HANDLE_STMT, m_stmt};

        if(indicator == SQL_NULL_DATA)
        {
            m_cols[col].isNull = true;
            break;
        }


        std::size_t chunkLen{std::strlen(
            reinterpret_cast<const char*>(buffer.data())
        )};
        m_cols[col].value.append(reinterpret_cast<const char*>(buffer.data()), chunkLen);

        if(rc == SQL_SUCCESS)
            break;
    }
    m_cols[col].fetched = true;

    if(m_cols[col].isNull)
        return {};
    else
        return m_cols[col].value;

}

int SqlServerRowReader::getColumnCount() const
{
    if(m_colCount)
        return m_colCount;

    SQLSMALLINT numCols{0};
    SQLRETURN rc = SQLNumResultCols(m_stmt, &numCols);
    if(!SQL_SUCCEEDED(rc))
        throw SqlServerRuntimeError{"SqlServerRowReader::getColumnCount SQLNumResultCols", SQL_HANDLE_STMT, m_stmt};
    if(rc == SQL_SUCCESS_WITH_INFO)
        odbc::logSuccessInfo("SqlServerRowReader::getColumnCount SQLNumResultCols", SQL_HANDLE_STMT, m_stmt);

    m_colCount = static_cast<int>(numCols);
    return m_colCount;
}
