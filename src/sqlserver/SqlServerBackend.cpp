#include "sqlserver/SqlServerBackend.h"

#include <memory>

#include "BackendRegistry.h"
#include "sqlserver/SqlServerConnection.h"
#include "IDatabaseConnection.h"
#include "ConnectionConfig.h"

namespace{
    class SqlServerBackendFactory : public IDatabaseBackendFactory
    {
    public:
        SqlServerBackendFactory() = default;
        ~SqlServerBackendFactory() override = default;

        std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) override;


    };

    std::unique_ptr<IDatabaseConnection> SqlServerBackendFactory::createConnection(const ConnectionConfig &cfg)
    {
        return std::make_unique<SqlServerConnection>(cfg.connStr);
    }

}

void registerSqlServerBackend()
{
    BackendRegistry::registerBackend("sqlserver", std::make_unique<SqlServerBackendFactory>());
}
