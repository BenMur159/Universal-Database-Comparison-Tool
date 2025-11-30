#ifndef SQLITEBACKENDFACTORY_H
#define SQLITEBACKENDFACTORY_H

#include <memory>

#include "BackendRegistry.h"

class IDatabaseConnection;
struct ConnectionConfig;

class SqliteBackendFactory : public IDatabaseBackendFactory
{
public:
    SqliteBackendFactory() = default;
    ~SqliteBackendFactory() override = default;

    std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) override;


};

#endif // SQLITEBACKENDFACTORY_H
