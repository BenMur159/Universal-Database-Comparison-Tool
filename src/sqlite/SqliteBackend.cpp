#include "sqlite/SqliteBackend.h"

#include <memory>

#include "BackendRegistry.h"
#include "IDatabaseConnection.h"
#include "ConnectionConfig.h"
#include "sqlite/SqliteConnection.h"

namespace{
    class SqliteBackendFactory : public IDatabaseBackendFactory
    {
    public:
        SqliteBackendFactory() = default;
        ~SqliteBackendFactory() override = default;

        std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) override;


    };

    std::unique_ptr<IDatabaseConnection> SqliteBackendFactory::createConnection(const ConnectionConfig &cfg)
    {
        return std::make_unique<SqliteConnection>(cfg.connStr);
    }
}

void registerSqliteBackend()
{
     BackendRegistry::registerBackend("sqlite", std::make_unique<SqliteBackendFactory>());
}
