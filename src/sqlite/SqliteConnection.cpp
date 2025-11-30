#include "sqlite/SqliteConnection.h"

#include <stdexcept>

#include "sqlite3.h"
#include "TableReadSpec.h"
#include "sqlite/SqliteSchemaInspector.h"
#include "sqlite/SqliteRowReader.h"
#include "util/Sqlutil.h"


SqliteConnection::SqliteConnection(const std::string& connStr)
{
    int rc = sqlite3_open_v2(connStr.c_str(), &m_db, SQLITE_OPEN_READONLY, nullptr);

    if(rc != SQLITE_OK)
    {
        const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
        if(m_db)
        {
            sqlite3_close_v2(m_db);
            m_db = nullptr;
        }
        throw std::runtime_error(
            "Connection to " + connStr + " failed(" + std::to_string(rc) + "):" + errMsg
        );
    }
}

SqliteConnection::~SqliteConnection() noexcept
{
    if(m_db)
    {
        sqlite3_close_v2(m_db);
        m_db = nullptr;
    }
}

std::unique_ptr<ISchemaInspector> SqliteConnection::createSchemaInspector()
{
    return std::make_unique<SqliteSchemaInspector>(m_db);
}

std::unique_ptr<IRowReader> SqliteConnection::createRowReader(const TableReadSpec &spec)
{
    std::string quoSelectedCols;
    if(spec.selectedCols.empty())
        quoSelectedCols = "*";
    else
        quoSelectedCols = sqlite3_util::quoteIdentifierList(spec.selectedCols);

    std::string quoTableName{sqlite3_util::quoteIdentifier(spec.tableName)};

    std::string sql{
        "SELECT " + quoSelectedCols + " "
        "FROM " + quoTableName + ";"
    };

    sqlite3_stmt* stmt{nullptr};
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if(rc != SQLITE_OK)
    {
        const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
        if(stmt)
            sqlite3_finalize(stmt);
        throw std::runtime_error("SqliteConnection::createRowReader prepare failed (" + std::to_string(rc) + "): " + errMsg);
   }

    return std::make_unique<SqliteRowReader>(stmt, m_db);
}
