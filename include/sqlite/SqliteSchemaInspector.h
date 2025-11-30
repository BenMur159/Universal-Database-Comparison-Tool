#ifndef SQLITESCHEMAINSPECTOR_H
#define SQLITESCHEMAINSPECTOR_H

#include <string>
#include <vector>

#include "ISchemaInspector.h"
#include "DatabaseSchemaInfoTypes.h"

class SqliteConnection;
struct sqlite3;

class SqliteSchemaInspector : public ISchemaInspector
{
public:
    explicit SqliteSchemaInspector(sqlite3* db);
    ~SqliteSchemaInspector() override = default;

    std::vector<std::string> listAllTables() const override;
    std::vector<std::string> listAllColumnNames(const std::string &tableName) const override;
    TableInfo getTableInfo(const std::string& tableName) const override;

private:
    sqlite3* m_db;

};






#endif // SQLITESCHEMAINSPECTOR_H
