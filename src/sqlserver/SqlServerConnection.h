#ifndef SQLSERVERCONNECTION_H
#define SQLSERVERCONNECTION_H

#include <string>
#include <memory>

#include "IDatabaseConnection.h"

class ISchemaInspector;
class IRowReader;
struct ConnectionConfig;

typedef void* SQLHENV;
typedef void* SQLHDBC;


class SqlServerConnection : public IDatabaseConnection
{
public:
    SqlServerConnection(const std::string& connStr);
    ~SqlServerConnection() noexcept override;

    SqlServerConnection(const SqlServerConnection&) = delete;
    SqlServerConnection& operator=(const SqlServerConnection&) = delete;
    SqlServerConnection(SqlServerConnection&&) = delete;
    SqlServerConnection& operator=(SqlServerConnection&&) = delete;

    std::unique_ptr<ISchemaInspector> createSchemaInspector() override;
    std::unique_ptr<IRowReader> createRowReader(const TableReadSpec& spec) override;

private:
    void close() noexcept;
    SQLHENV m_env{nullptr};
    SQLHDBC m_dbc{nullptr};
};

#endif // SQLSERVERCONNECTION_H
