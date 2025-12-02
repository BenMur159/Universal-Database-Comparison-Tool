#ifndef SQLITEBACKEND_H
#define SQLITEBACKEND_H

#include <memory>

#include "BackendRegistry.h"

// class IDatabaseConnection;
// struct ConnectionConfig;

// class SqliteBackendFactory : public IDatabaseBackendFactory
// {
// public:
//     SqliteBackendFactory() = default;
//     ~SqliteBackendFactory() override = default;

//     std::unique_ptr<IDatabaseConnection> createConnection(const ConnectionConfig& cfg) override;


// };

void registerSqliteBackend();

#endif // SQLITEBACKEND_H
