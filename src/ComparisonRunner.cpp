#include "dbdiff/ComparisonRunner.h"

#include "BackendRegistry.h"
#include "sqlite/SqliteBackend.h"
#include "sqlserver/SqlServerBackend.h"
#include "IDatabaseConnection.h"
#include "ISchemaInspector.h"
#include "dbdiff/DatabaseSchemaInfoTypes.h"
#include "DatabaseSchemaBuilder.h"
#include "DatabaseComperator.h"

void registerAllBuiltInBackends()
{
    registerSqliteBackend();
    registerSqlServerBackend();
}

DatabaseDiffReport runDatabaseComparison(const ConnectionConfig& cfgA,
                                         const ConnectionConfig& cfgB)
{
    // Lazy register in case the user forgets
    if(BackendRegistry::isEmpty())
    {
        registerAllBuiltInBackends();
    }
    std::unique_ptr<IDatabaseConnection> connA{ makeDatabaseConnection(cfgA) };
    std::unique_ptr<IDatabaseConnection> connB{ makeDatabaseConnection(cfgB) };

    std::unique_ptr<ISchemaInspector> inspectorA{ connA->createSchemaInspector() };
    std::unique_ptr<ISchemaInspector> inspectorB{ connB->createSchemaInspector() };

    DatabaseSchema dbSchemaA{ buildDatabaseSchema(*inspectorA) };
    DatabaseSchema dbSchemaB{ buildDatabaseSchema(*inspectorB) };

    return compareDatabase(*connA, *connB, dbSchemaA, dbSchemaB);
}
