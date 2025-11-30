#ifndef IDATABASECONNECTION_H
#define IDATABASECONNECTION_H

#include <memory>

class ISchemaInspector;
class IRowReader;
struct TableReadSpec;

class IDatabaseConnection
{

public:

    virtual ~IDatabaseConnection() = default;

    IDatabaseConnection(const IDatabaseConnection&) = delete;
    IDatabaseConnection(IDatabaseConnection&&) = delete;
    IDatabaseConnection operator=(const IDatabaseConnection&) = delete;
    IDatabaseConnection& operator=(IDatabaseConnection&&) = delete;

    virtual std::unique_ptr<ISchemaInspector> createSchemaInspector() = 0;
    virtual std::unique_ptr<IRowReader> createRowReader(const TableReadSpec& spec) = 0;

protected:
    IDatabaseConnection() = default;

};

#endif // IDATABASECONNECTION_H


