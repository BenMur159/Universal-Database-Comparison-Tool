
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#include "ConnectionConfig.h"
#include "BackendRegistry.h"
#include "sqlite/SqliteBackend.h"
#include "sqlserver/SqlServerBackend.h"
#include "IDatabaseConnection.h"
#include "ISchemaInspector.h"
#include "IRowReader.h"
#include "TableReadSpec.h"
#include "util/Sqlutil.h"
#include "DatabaseSchemaInfoTypes.h"
#include "DatabaseSchemaBuilder.h"
#include "TableComperator.h"
#include "TableStructureComperator.h"
#include "DatabaseComperator.h"

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

std::ostream& operator<<(std::ostream& os, const DatabaseDiffReport& report)
{
    if (report.tables.empty())
    {
        os << "No tables found in diff report.\n";
        return os;
    }

    if (report.isEqual)
    {
        os << "Databases are equal.\n";
    }
    else
    {
        os << "Databases differ.\n";
    }

    for (const auto& table : report.tables)
    {
        os << "\n========================================\n";

        os << "Table:\n";
        if (!table.tableNameA.empty())
        {
            os << "  A: " << table.tableNameA << "\n";
        }
        if (!table.tableNameB.empty())
        {
            os << "  B: " << table.tableNameB << "\n";
        }

        os << "  Difference kind: ";
        switch (table.diffKind)
        {
        case TableDiffKind::OnlyInA:
            os << "OnlyInA\n";
            break;
        case TableDiffKind::OnlyInB:
            os << "OnlyInB\n";
            break;
        case TableDiffKind::ColumMismatch:   // rename to ColumnMismatch in your enum
            os << "ColumnMismatch\n";
            break;
        case TableDiffKind::DataMismatch:
            os << "DataMismatch\n";
            break;
        case TableDiffKind::Equal:
            os << "Equal\n";
            break;
        }

        // ---------- Schema / column mismatches ----------
        if (table.diffKind == TableDiffKind::ColumMismatch)
        {
            os << "  Schema differences:\n";

            if (!table.strucDiff.has_value())
            {
                os << "    (no detailed structure diff stored)\n";
            }
            else
            {
                const auto& diff = *table.strucDiff;

                if (diff.issues.empty())
                {
                    os << "    (no issues listed, but kind is ColumnMismatch)\n";
                }
                else
                {
                    for (const auto& issue : diff.issues)
                    {
                        switch (issue.kind)
                        {
                        case StructureIssueKind::ColumnCountMismatch:
                        {
                            const auto colsA = table.tabInfA.columns.size();
                            const auto colsB = table.tabInfB.columns.size();
                            os << "    Column count mismatch: A has " << colsA
                               << " columns, B has " << colsB << " columns.\n";
                            break;
                        }
                        case StructureIssueKind::ColumnNameMismatch:
                        {
                            const int idx = issue.columnIndex;
                            os << "    Column " << idx << " name mismatch: ";
                            if (idx >= 0 &&
                                static_cast<std::size_t>(idx) < table.tabInfA.columns.size() &&
                                static_cast<std::size_t>(idx) < table.tabInfB.columns.size())
                            {
                                const auto& colA = table.tabInfA.columns[static_cast<std::size_t>(idx)];
                                const auto& colB = table.tabInfB.columns[static_cast<std::size_t>(idx)];
                                os << "A=\"" << colA.name << "\", "
                                   << "B=\"" << colB.name << "\"\n";
                            }
                            else
                            {
                                os << "(index out of range in TableInfo)\n";
                            }
                            break;
                        }
                        case StructureIssueKind::ColumnTypeMismatch:
                        {
                            const int idx = issue.columnIndex;
                            os << "    Column " << idx << " type mismatch: ";
                            if (idx >= 0 &&
                                static_cast<std::size_t>(idx) < table.tabInfA.columns.size() &&
                                static_cast<std::size_t>(idx) < table.tabInfB.columns.size())
                            {
                                const auto& colA = table.tabInfA.columns[static_cast<std::size_t>(idx)];
                                const auto& colB = table.tabInfB.columns[static_cast<std::size_t>(idx)];
                                os << "A(dbType=\"" << colA.dbType
                                   << "\", logical=" << static_cast<int>(colA.logicalType) << "), "
                                   << "B(dbType=\"" << colB.dbType
                                   << "\", logical=" << static_cast<int>(colB.logicalType) << ")\n";
                            }
                            else
                            {
                                os << "(index out of range in TableInfo)\n";
                            }
                            break;
                        }
                        case StructureIssueKind::NullabilityMismatch:
                        {
                            const int idx = issue.columnIndex;
                            os << "    Column " << idx << " nullability mismatch: ";
                            if (idx >= 0 &&
                                static_cast<std::size_t>(idx) < table.tabInfA.columns.size() &&
                                static_cast<std::size_t>(idx) < table.tabInfB.columns.size())
                            {
                                const auto& colA = table.tabInfA.columns[static_cast<std::size_t>(idx)];
                                const auto& colB = table.tabInfB.columns[static_cast<std::size_t>(idx)];
                                os << "A is " << (colA.isNullable ? "" : "NOT ")
                                   << "NULLABLE, "
                                   << "B is " << (colB.isNullable ? "" : "NOT ")
                                   << "NULLABLE.\n";
                            }
                            else
                            {
                                os << "(index out of range in TableInfo)\n";
                            }
                            break;
                        }
                        case StructureIssueKind::PrimaryKeyMismatch:
                        {
                            const int idx = issue.columnIndex;
                            os << "    Column " << idx << " primary key membership mismatch: ";
                            if (idx >= 0 &&
                                static_cast<std::size_t>(idx) < table.tabInfA.columns.size() &&
                                static_cast<std::size_t>(idx) < table.tabInfB.columns.size())
                            {
                                const auto& colA = table.tabInfA.columns[static_cast<std::size_t>(idx)];
                                const auto& colB = table.tabInfB.columns[static_cast<std::size_t>(idx)];
                                os << "A is " << (colA.isPartOfPrimaryKey ? "" : "NOT ")
                                   << "part of PK, "
                                   << "B is " << (colB.isPartOfPrimaryKey ? "" : "NOT ")
                                   << "part of PK.\n";
                            }
                            else
                            {
                                os << "(index out of range in TableInfo)\n";
                            }
                            break;
                        }
                        default:
                            os << "Equal?!";
                        }
                    }
                }
            }
        }

        // ---------- Data mismatches ----------
        if (table.diffKind == TableDiffKind::DataMismatch)
        {
            if (table.rowDiffs.empty())
            {
                os << "  No row differences stored, but kind is DataMismatch.\n";
            }
            else
            {
                os << "  Row differences:\n";
                for (const auto& row : table.rowDiffs)
                {
                    os << "    row=\"" << row.canonicalRow << "\"\n"
                       << "      countA=" << row.countA
                       << ", countB=" << row.countB << "\n";
                }
            }
        }
    }

    os << "\n========================================\n";
    return os;
}

void registerAllBackends()
{
    // BackendRegistry::registerBackend("sqlite", std::make_unique<SqliteBackendFactory>());
    // BackendRegistry::registerBackend("sqlserver", std::make_unique<SqlServerBackendFactory>());
    registerSqliteBackend();
    registerSqlServerBackend();
}

int main()
{
    registerAllBackends();
    std::string testPath = "C:/Users/43676/source/repos/JhChallenge/examples/MyExample1.db";

    std::unique_ptr<IDatabaseConnection> connA = makeDatabaseConnection(ConnectionConfig{"sqlite", testPath});
    auto inspectorA = connA->createSchemaInspector();

    auto dbSchemaA = buildDatabaseSchema(*inspectorA);
    for(auto it = dbSchemaA.tablesByName.begin(); it != dbSchemaA.tablesByName.end(); ++it)
    {
        std::cout << it->first << "\n";
        std::cout << it->second << "\n\n";
    }
    std::cout << "\n\n";

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

    TableInfo factoryInfoA = (dbSchemaA.tablesByName.find("Factory"))->second;
    TableInfo factoryInfoB = (dbSchemaB.tablesByName.find("Factory"))->second;
    std::cout << "Sanity test: A->" << factoryInfoA.name << ", B->" << factoryInfoB.name << "\n";
    TableComparisonResult tabCompResult = compareTableRowData(*connA, *connB, factoryInfoA, factoryInfoB);

    for(const auto& result : tabCompResult.rowDifferences)
    {
        std::cout << result.canonicalRow << " - A: " << result.countA << " - B " << result.countB << "\n";
    }
    std::cout << "\n\n";

    DatabaseDiffReport dbDiffRep = compareDatabase(*connA, *connB, dbSchemaA, dbSchemaB);
    std::cout << dbDiffRep;

}
