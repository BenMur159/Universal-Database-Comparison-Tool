#ifndef SQLSERVERSCHEMAINSPECTOR_H
#define SQLSERVERSCHEMAINSPECTOR_H

#include <string>
#include <vector>

#include "ISchemaInspector.h"
#include "DatabaseSchemaInfoTypes.h"

typedef void* SQLHDBC;

class SqlServerSchemaInspector : public ISchemaInspector
{
public:
    explicit SqlServerSchemaInspector(SQLHDBC dbc);
    ~SqlServerSchemaInspector() noexcept override = default;

    SqlServerSchemaInspector(const SqlServerSchemaInspector&) = delete;
    SqlServerSchemaInspector& operator=(const SqlServerSchemaInspector&) = delete;
    SqlServerSchemaInspector(SqlServerSchemaInspector&&) = delete;
    SqlServerSchemaInspector& operator=(SqlServerSchemaInspector&&) = delete;

    std::vector<std::string> listAllTables() const override;
    std::vector<std::string> listAllColumnNames(const std::string &tableName) const override;
    TableInfo getTableInfo(const std::string& tableName) const override;

private:
    SQLHDBC m_dbc;

};

#endif // SQLSERVERSCHEMAINSPECTOR_H
