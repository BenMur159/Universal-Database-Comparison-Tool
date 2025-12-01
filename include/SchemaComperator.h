#ifndef SCHEMACOMPERATOR_H
#define SCHEMACOMPERATOR_H

#include <vector>

#include "DatabaseSchemaInfoTypes.h"

enum class SchemaIssueKind
{
    Equal,
    ColumnCountMismatch,
    ColumnNameMisMatch,
    ColumnTypeMisMatch,
    NullabilityMismatch,
    PrimaryKeyMismatch
};

struct SchemaIssue{
    SchemaIssueKind kind{SchemaIssueKind::Equal};
    int columnIndex{-1}; // -1 means not relevant
};

struct SchemaDiffResult
{
    std::vector<SchemaIssue> issues;
    bool isEqual() const { return issues.empty(); }
};

SchemaDiffResult compareTableSchema(const TableInfo& tabInfA, const TableInfo& tabInfB);

#endif // SCHEMACOMPERATOR_H
