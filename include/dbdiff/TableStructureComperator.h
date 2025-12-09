#ifndef TABLESTRUCTURECOMPERATOR_H
#define TABLESTRUCTURECOMPERATOR_H

#include <vector>

#include "dbdiff/DatabaseSchemaInfoTypes.h"

enum class StructureIssueKind
{
    Equal,
    ColumnCountMismatch,
    ColumnNameMismatch,
    ColumnTypeMismatch,
    NullabilityMismatch,
    PrimaryKeyMismatch
};

struct StructureIssue{
    StructureIssueKind kind{StructureIssueKind::Equal};
    int columnIndex{-1}; // -1 means not relevant
};

struct TableStructureDiffResult
{
    std::vector<StructureIssue> issues;
    bool isEqual() const { return issues.empty(); }
};

TableStructureDiffResult compareTableStructure(const TableInfo& tabInfA, const TableInfo& tabInfB);

#endif // TABLESTRUCTURECOMPERATOR_H
