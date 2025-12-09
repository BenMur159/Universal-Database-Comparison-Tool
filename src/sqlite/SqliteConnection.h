#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include <memory>
#include <string>

#include "IDatabaseConnection.h"

class ISchemaInspector;
class IRowReader;
struct TableReadSpec;
struct sqlite3;

class SqliteConnection : public IDatabaseConnection
{
public:
    SqliteConnection(const std::string& connStr);
    ~SqliteConnection() noexcept override;


    std::unique_ptr<ISchemaInspector> createSchemaInspector() override;
    std::unique_ptr<IRowReader> createRowReader(const TableReadSpec& spec) override;

private:
    sqlite3* m_db{ nullptr };
};

#endif // SQLITECONNECTION_H
