#include "sqlite/SqliteBackendFactory.h"

#include "IDatabaseConnection.h"
#include "ConnectionConfig.h"
#include "sqlite/SqliteConnection.h"


// SqliteBackendFactory::~SqliteBackendFactory()
// {

// }

std::unique_ptr<IDatabaseConnection> SqliteBackendFactory::createConnection(const ConnectionConfig &cfg)
{
    return std::make_unique<SqliteConnection>(cfg.connStr);
}
