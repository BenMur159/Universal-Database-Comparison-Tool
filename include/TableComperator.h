#ifndef TABLECOMPERATOR_H
#define TABLECOMPERATOR_H

#include <vector>

#include "DatabaseSchemaInfoTypes.h"
#include "DatabaseDiffReport.h"
#include "IDatabaseConnection.h"

struct TableComparisonResult
{
    std::vector<RowDifferences> rowDifferences;
};

TableComparisonResult compareTable(
    IDatabaseConnection& dbA, IDatabaseConnection& dbB,
    const TableInfo& tabInfA, const TableInfo& tabInfB
);

#endif // TABLECOMPERATOR_H
