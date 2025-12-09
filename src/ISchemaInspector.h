#ifndef ISCHEMAINSPECTOR_H
#define ISCHEMAINSPECTOR_H

#include <vector>
#include <string>

#include "dbdiff/DatabaseSchemaInfoTypes.h"

class ISchemaInspector
{
public:
    virtual ~ISchemaInspector() = default;

    ISchemaInspector(const ISchemaInspector&) = delete;
    ISchemaInspector(ISchemaInspector &&) = delete;
    ISchemaInspector &operator=(const ISchemaInspector &) = delete;
    ISchemaInspector &operator=(ISchemaInspector &&) = delete;


    virtual std::vector<std::string> listAllTables() const = 0;
    virtual std::vector<std::string> listAllColumnNames(const std::string& tableName) const = 0;
    virtual TableInfo getTableInfo(const std::string& tableName) const = 0;

protected:
    ISchemaInspector() = default;

};

#endif // ISCHEMAINSPECTOR_H
