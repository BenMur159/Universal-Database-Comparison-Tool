#ifndef BACKENDREGISTRY_H
#define BACKENDREGISTRY_H

#include <memory>
#include <string>

class IDatabaseConnection;
struct ConnectionConfig;

class IDatabaseBackendFactory
{
public:
    virtual ~IDatabaseBackendFactory() = default;

    IDatabaseBackendFactory(const IDatabaseBackendFactory&) = delete;
    IDatabaseBackendFactory& operator=(const IDatabaseBackendFactory&) = delete;
    IDatabaseBackendFactory(IDatabaseBackendFactory&&) = delete;
    IDatabaseBackendFactory& operator=(IDatabaseBackendFactory&&) = delete;

    virtual std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) = 0;

protected:
    IDatabaseBackendFactory() = default;

};

namespace BackendRegistry {

    void registerBackend(const std::string& databaseType, std::unique_ptr<IDatabaseBackendFactory> factory);
    IDatabaseBackendFactory& getBackendFactory(const std::string& databaseType);
}

std::unique_ptr<IDatabaseConnection>makeDatabaseConnection(const ConnectionConfig& cfg);

#endif // BACKENDREGISTRY_H
