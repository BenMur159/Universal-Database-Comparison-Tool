#include "ComparisonRunner.h"

#include "IDatabaseConnection.h"
#include "BackendRegistry.h"

// void registerAllBackends()
// {
//     BackendRegistry::registerBackend("sqlite", std::make_unique<SqliteBackendFactory>());
//     BackendRegistry::registerBackend("sqlserver", std::make_unique<SqlServerBackendFactory>());
// }

DatabaseDiffReport runDatabaseComparison(const ConnectionConfig& cfgA,
                                         const ConnectionConfig& cfgB)
{
    return {};
}
