#ifndef SQLSERVERBACKENDFACTORY_H
#define SQLSERVERBACKENDFACTORY_H

#include <memory>

#include "BackendRegistry.h"

class IDatabaseConnection;
struct ConnectionConfig;

class SqlServerBackendFactory : public IDatabaseBackendFactory
{
public:
    SqlServerBackendFactory() = default;
    ~SqlServerBackendFactory() override = default;

    std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) override;


};

#endif // SQLSERVERBACKENDFACTORY_H
