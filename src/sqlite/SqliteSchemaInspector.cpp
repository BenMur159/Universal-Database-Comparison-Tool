#include "sqlite/SqliteSchemaInspector.h"

#include <stdexcept>
#include <cstddef>
#include <algorithm>

#include "sqlite3.h"
#include "util/Sqlutil.h"

#include <iostream>


namespace {
    struct StmtGuard
    {
        sqlite3_stmt* stmt{nullptr};

        StmtGuard() = default;
        explicit StmtGuard(sqlite3_stmt* s) : stmt{s} {}

        ~StmtGuard() noexcept {
            if(stmt)
            {
                sqlite3_finalize(stmt);
                stmt = nullptr;
            }
        }

        StmtGuard(const StmtGuard&) = delete;
        StmtGuard& operator=(const StmtGuard&) = delete;
        StmtGuard(StmtGuard&&) = delete;
        StmtGuard& operator=(StmtGuard&&) = delete;
    };

    LogicalType mapDbTypeToLogicalType(std::string dbType)
    {
        std::transform(dbType.begin(), dbType.end(), dbType.begin(),
                        [](unsigned char c){ return std::toupper(c); } );

        if(dbType.find("INT") != std::string::npos)
            return LogicalType::Integer;

        if(dbType.find("TEXT") != std::string::npos
            || dbType.find("CHAR") != std::string::npos)
        {
            return LogicalType::Text;
        }

        if(dbType.find("REAL") != std::string::npos
            || dbType.find("FLOA") != std::string::npos)
        {
            return LogicalType::Real;
        }

        if(dbType.find("BLOB") != std::string::npos)
            return LogicalType::Blob;

        // default
        return LogicalType::Other;
    }
}


SqliteSchemaInspector::SqliteSchemaInspector(sqlite3* db)
    : m_db{db}
{

}

// SqliteSchemaInspector::~SqliteSchemaInspector()
// {

// }

std::vector<std::string> SqliteSchemaInspector::listAllTables() const
{

    std::string sql{
        "SELECT name FROM sqlite_schema "
        "WHERE type='table' "
        "AND name NOT LIKE 'sqlite_%' "
        "ORDER BY name;"
    };

    sqlite3_stmt* stmt{nullptr};

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
        throw std::runtime_error("SqliteSchemaInspector::listAllTables prepare failed (" + std::to_string(rc) + "): " + errMsg);
    }
    StmtGuard guard{stmt}; // automatic finalization when out of scope

    std::vector<std::string> out;
    while(true)
    {
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_DONE)
            break;
        if(rc != SQLITE_ROW)
        {
            const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
            throw std::runtime_error("step failed (" + std::to_string(rc) + "): " + errMsg);
        }
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if(!text)
            throw std::runtime_error("Tablename was nullptr");
        else
            out.emplace_back(reinterpret_cast<const char*>(text));
    }
    return out;
}

std::vector<std::string> SqliteSchemaInspector::listAllColumnNames(const std::string &tableName) const
{
    std::string quoTabName{sqlite3_util::quoteIdentifier(tableName)};
    std::string sql{
        "PRAGMA table_info(" + quoTabName +");"
    };
    sqlite3_stmt* stmt{nullptr};

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
        throw std::runtime_error("SqliteSchemaInspector::listAllColumnNames prepare failed (" + std::to_string(rc) + "): " + errMsg);
    }
    StmtGuard guard{stmt}; // automatic finalization when out of scope

    // PRAGMA table_info columns: cid(0), name(1), type(2), notnull(3), dflt_value(4), pk(5)
    int nameCol = 1;
    std::vector<std::string> out;
    while(true)
    {
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_DONE)
            break;
        if(rc != SQLITE_ROW)
        {
            const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
            throw std::runtime_error("SqliteSchemaInspector::listAllColumnNames step failed (" + std::to_string(rc) + "): " + errMsg);
        }
        const unsigned char* text{sqlite3_column_text(stmt, nameCol)}; //
        if(!text)
            out.emplace_back("<NULL>");
        else
            out.emplace_back(reinterpret_cast<const char*>(text));
    }
    return out;
}

TableInfo SqliteSchemaInspector::getTableInfo(const std::string &tableName) const
{
    std::string quoTabName{sqlite3_util::quoteIdentifier(tableName)};
    std::string sql{
        "PRAGMA table_info(" + quoTabName +");"
    };
    sqlite3_stmt* stmt{nullptr};

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
        throw std::runtime_error("SqliteSchemaInspector::getTableInfo prepare failed (" + std::to_string(rc) + "): " + errMsg);
    }
    StmtGuard guard{stmt};

    // PRAGMA table_info columns: cid(0), name(1), type(2), notnull(3), dflt_value(4), pk(5)
    TableInfo tabInf{};
    tabInf.name = tableName;
    std::size_t colIdx{0};
    while(true)
    {
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_DONE)
            break;
        if(rc != SQLITE_ROW)
        {
            const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
            throw std::runtime_error("SqliteSchemaInspector::getTableInfo step failed (" + std::to_string(rc) + "): " + errMsg);
        }

        ColumnInfo colInf{};
        const unsigned char* colName{sqlite3_column_text(stmt, 1)};
        if(!colName)
        {
            throw std::runtime_error("SqliteSchemaInspector::getTableInfo unexpected null "
                                     "column name in PRAGMA table_info for table " + tableName);
        }
        colInf.name = reinterpret_cast<const char*>(colName);

        const unsigned char* colType = sqlite3_column_text(stmt, 2);
        colInf.dbType = colType ? reinterpret_cast<const char*>(colType) : std::string{};
        colInf.logicalType = mapDbTypeToLogicalType(colInf.dbType);

        const int notNull{sqlite3_column_int(stmt, 3)};
        const int pk{sqlite3_column_int(stmt, 5)};

        colInf.isPartOfPrimaryKey = (pk != 0);
        colInf.isNullable = (notNull == 0 && !colInf.isPartOfPrimaryKey);

        if(colInf.isPartOfPrimaryKey)
        {
            tabInf.primaryKeyColumnIndices.push_back(colIdx);
        }
        tabInf.columns.push_back(std::move(colInf));
        ++colIdx;
    }
    if (tabInf.columns.empty())
    {
        throw std::runtime_error("SqliteSchemaInspector::getTableInfo table " + tableName + " "
                                 "has no columns, might not exist" );
    }
    return tabInf;
}

