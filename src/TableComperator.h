#ifndef TABLECOMPERATOR_H
#define TABLECOMPERATOR_H

#include <vector>

#include "dbdiff/DatabaseSchemaInfoTypes.h"
#include "dbdiff/DatabaseDiffReport.h"
#include "IDatabaseConnection.h"

struct TableComparisonResult
{
    std::vector<RowDifferences> rowDifferences;
};

TableComparisonResult compareTableRowData(
    IDatabaseConnection& dbA, IDatabaseConnection& dbB,
    const TableInfo& tabInfA, const TableInfo& tabInfB
);

#endif // TABLECOMPERATOR_H
