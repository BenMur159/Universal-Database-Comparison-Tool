#ifndef TABLECOMPERATOR_H
#define TABLECOMPERATOR_H

#include "DatabaseSchemaInfoTypes.h"
#include "DatabaseDiffReport.h"
#include "IDatabaseConnection.h"

struct TableComparisonResult
{
    std::vector<RowDifferences> rowDifferences;
};

TableComparisonResult compareTable(
    IDatabaseConnection& dbA, IDatabaseConnection& dbB,
    const TableInfo& tableA, const TableInfo& tableB
);

#endif // TABLECOMPERATOR_H
