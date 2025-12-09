#ifndef SQLITEROWREADER_H
#define SQLITEROWREADER_H

#include <string>
#include <optional>

#include "IRowReader.h"

struct sqlite3;
struct sqlite3_stmt;

class SqliteRowReader : public IRowReader
{
public:
    SqliteRowReader(sqlite3_stmt* stmt, sqlite3* db);
    ~SqliteRowReader() noexcept override;

    bool next() override;
    std::optional<std::string> getString(int col) const override;
    int getColumnCount() const override;

private:
    sqlite3_stmt* m_stmt;
    sqlite3* m_db;
};

#endif // SQLITEROWREADER_H
