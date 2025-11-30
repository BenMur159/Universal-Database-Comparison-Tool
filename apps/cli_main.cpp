
#include <memory>
#include <iostream>
#include <iomanip>

#include "ConnectionConfig.h"
#include "BackendRegistry.h"
#include "sqlite/SqliteBackendFactory.h"
#include "sqlserver/SqlServerBackendFactory.h"
#include "IDatabaseConnection.h"
#include "ISchemaInspector.h"
#include "IRowReader.h"
#include "TableReadSpec.h"
#include "util/Sqlutil.h"
#include "DatabaseSchemaInfoTypes.h"
#include "DatabaseSchemaBuilder.h"

std::ostream& operator<<(std::ostream& out, const TableInfo& tabInf)
{
    const int space = 20;
    out << tabInf.name << "\n";
    out << std::left << std::setw(space) << "Name";
    out << std::left << std::setw(space) << "Type";
    out << std::left << std::setw(space) << "isNullable";
    out << std::left << std::setw(space) << "isPrimaryKey" << "\n";
    for(const auto& col : tabInf.columns)
    {
        out << std::left << std::setw(space) << col.name;
        out << std::left << std::setw(space) << col.dbType;
        out << std::left << std::setw(space) << col.isNullable;
        out << std::left << std::setw(space) << col.isPartOfPrimaryKey << "\n";
    }
    return out;
}

void registerAllBackends()
{
    BackendRegistry::registerBackend("sqlite", std::make_unique<SqliteBackendFactory>());
    BackendRegistry::registerBackend("sqlserver", std::make_unique<SqlServerBackendFactory>());
}

int main()
{
    registerAllBackends();
    std::string testPath = "C:/Users/43676/source/repos/JhChallenge/examples/MyExample1.db";

    std::unique_ptr<IDatabaseConnection> connA = makeDatabaseConnection(ConnectionConfig{"sqlite", testPath});
    auto inspectorA = connA->createSchemaInspector();
    auto tablesNamesA = inspectorA->listAllTables();

    for(const auto& tableName : tablesNamesA)
    {
        std::cout << tableName << "\t";
    }
    std::cout << "\n\n";

    std::vector<TableInfo> tableInfosA{};
    for(const auto& tableNameA : tablesNamesA )
    {
        tableInfosA.push_back(inspectorA->getTableInfo(tableNameA));
    }
    for(const auto& tabInf : tableInfosA)
    {
        std::cout << tabInf << "\n";
    }
    std::cout << "\n\n";

    auto reader = connA->createRowReader({"Workplace", {}});

    std::cout << "Test optional: \n";
    while(reader->next())
    {
        if(!reader->getString(3))
        {
            std::cout << "null";
        }
        std::cout << *(reader->getString(3)) << "\n";
    }

    std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBB\n";

    std::string testConStr =
                "Driver={ODBC Driver 17 for SQL Server};"
                "Server=B_MUR,50532;"                 // or .\\SQLEXPRESS, or MACHINENAME\\SQLEXPRESS
                "Database=dbdiff_test;"
                "Trusted_Connection=yes;";  // or UID=...;PWD=...;


    std::unique_ptr<IDatabaseConnection> connB = makeDatabaseConnection(ConnectionConfig{"sqlserver", testConStr});
    auto inspectorB = connB->createSchemaInspector();

    auto dbSchemaB = buildDatabaseSchema(*inspectorB);

    for(auto it = dbSchemaB.tablesByName.begin(); it != dbSchemaB.tablesByName.end(); ++it)
    {
        std::cout << it->first << "\n";
        std::cout << it->second << "\n\n";
    }




}
