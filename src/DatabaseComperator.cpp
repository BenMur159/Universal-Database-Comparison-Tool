#include "DatabaseComperator.h"
#include "TableStructureComperator.h"
#include "TableComperator.h"

DatabaseDiffReport compareDatabase(IDatabaseConnection &connA, IDatabaseConnection &connB,
                                   const DatabaseSchema &dbSchemaA, const DatabaseSchema &dbSchemaB)
{
    DatabaseDiffReport dbDiffRep{};
    for(const auto& [nameA, tabInfA] : dbSchemaA.tablesByName)
    {
        TableDiffReport tabDiffRep{};
        const auto itB = dbSchemaB.tablesByName.find(nameA);
        //only in A, not in B
        if(itB ==  dbSchemaB.tablesByName.end())
        {
            tabDiffRep.tableNameA = nameA;
            tabDiffRep.tabInfA = tabInfA;
            tabDiffRep.diffKind = TableDiffKind::OnlyInA;

            dbDiffRep.tables.push_back(std::move(tabDiffRep));
            dbDiffRep.isEqual = false;

            continue;
        }
        // In A and B -> Schema Comparison
        tabDiffRep.tableNameA = nameA;
        tabDiffRep.tableNameB = nameA; // or itB->first;
        const auto& tabInfB{itB->second};
        TableStructureDiffResult tabStrucDiff = compareTableStructure(tabInfA, tabInfB);
        if(!tabStrucDiff.isEqual())
        {
            tabDiffRep.diffKind = TableDiffKind::ColumMismatch;
            tabDiffRep.strucDiff = std::move(tabStrucDiff);
            tabDiffRep.tabInfA = tabInfA;
            tabDiffRep.tabInfB = tabInfB;

            dbDiffRep.tables.push_back(std::move(tabDiffRep));
            dbDiffRep.isEqual = false;

            continue;
        }
        // In A and B -> Row Data Comparison
        TableComparisonResult rowDiff = compareTableRowData(connA, connB, tabInfA, tabInfB);
        if(!rowDiff.rowDifferences.empty())
        {
            tabDiffRep.diffKind = TableDiffKind::DataMismatch;
            tabDiffRep.rowDiffs = std::move(rowDiff.rowDifferences);
            tabDiffRep.tabInfA = tabInfA;
            tabDiffRep.tabInfB = tabInfB;

            dbDiffRep.tables.push_back(std::move(tabDiffRep));
            dbDiffRep.isEqual = false;

            continue;
        }

        // name, types and rows are equal if we reach here
        tabDiffRep.diffKind = TableDiffKind::Equal;
        dbDiffRep.tables.push_back(std::move(tabDiffRep));
    }

    // only in B
    for(const auto& [nameB, tabInfB] : dbSchemaB.tablesByName)
    {
        if(!dbSchemaA.tablesByName.count(nameB))
        {
            TableDiffReport tabDiffRep{};
            tabDiffRep.tableNameB = nameB;
            tabDiffRep.tabInfB = tabInfB;
            tabDiffRep.diffKind = TableDiffKind::OnlyInB;

            dbDiffRep.tables.push_back(std::move(tabDiffRep));
            dbDiffRep.isEqual = false;
        }
    }

    return dbDiffRep;
}
