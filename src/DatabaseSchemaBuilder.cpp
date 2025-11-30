#include "DatabaseSchemaBuilder.h"

DatabaseSchema buildDatabaseSchema(ISchemaInspector& inspector)
{
    DatabaseSchema dbSchema;

    auto tableNames = inspector.listAllTables();
    for(const auto& name : tableNames)
    {
        dbSchema.tablesByName.emplace(name, inspector.getTableInfo(name));
    }

    return dbSchema;
}
