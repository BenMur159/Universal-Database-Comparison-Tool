#include "sqlserver/SqlServerBackendFactory.h"

#include "sqlserver/SqlServerConnection.h"
#include "IDatabaseConnection.h"
#include "ConnectionConfig.h"


std::unique_ptr<IDatabaseConnection> SqlServerBackendFactory::createConnection(const ConnectionConfig &cfg)
{
     return std::make_unique<SqlServerConnection>(cfg.connStr);
}
