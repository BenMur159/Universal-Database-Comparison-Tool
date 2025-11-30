#include "sqlite/SqliteRowReader.h"

#include <stdexcept>

#include "sqlite3.h"

SqliteRowReader::SqliteRowReader(sqlite3_stmt* stmt, sqlite3* db)
    : m_stmt{stmt}, m_db{db}
{

}

SqliteRowReader::~SqliteRowReader() noexcept
{
    if(m_stmt)
        sqlite3_finalize(m_stmt);
}

bool SqliteRowReader::next()
{
    int rc = sqlite3_step(m_stmt);
    if(rc == SQLITE_ROW)
        return true;
    if(rc == SQLITE_DONE)
        return false;

    const char* errMsg{ m_db ? sqlite3_errmsg(m_db) : sqlite3_errstr(rc) };
    throw std::runtime_error("SqliteRowReader::next step failed (" + std::to_string(rc) + "): " + errMsg);
}

std::optional<std::string> SqliteRowReader::getString(int col) const
{
    const unsigned char* text{sqlite3_column_text(m_stmt, col)};
    if(!text)
        return {};
    else
        return reinterpret_cast<const char*>(text);
}

int SqliteRowReader::getColumnCount() const
{
    return sqlite3_column_count(m_stmt);
}
