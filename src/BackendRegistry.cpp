#include "BackendRegistry.h"

#include <unordered_map>
#include <stdexcept>

#include "IDatabaseConnection.h"
#include "ConnectionConfig.h"

namespace BackendRegistry {
    // FYI: anonymous namespace for only internal linkage
    namespace{
        std::unordered_map<std::string, std::unique_ptr<IDatabaseBackendFactory>> g_factories;
    }

    void registerBackend(const std::string& databaseType, std::unique_ptr<IDatabaseBackendFactory> factory)
    {
        g_factories.emplace(databaseType, std::move(factory));
    }

    IDatabaseBackendFactory& getBackendFactory(const std::string& databaseType)
    {
        auto it = g_factories.find(databaseType);
        if(it == g_factories.end())
        {
            throw std::runtime_error("No backend factory of type = \"" + databaseType + "\"" );
        }
        return *(it->second);
    }
}

std::unique_ptr<IDatabaseConnection>
makeDatabaseConnection(const ConnectionConfig& cfg)
{
    auto& factory = BackendRegistry::getBackendFactory(cfg.databaseType);
    return factory.createConnection(cfg);
}
