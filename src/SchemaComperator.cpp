#include "SchemaComperator.h"

SchemaDiffResult compareTableSchema(const TableInfo &tabInfA, const TableInfo &tabInfB)
{
    SchemaDiffResult result{};
    // column count issue
    if(tabInfA.columns.size() != tabInfB.columns.size())
    {
        result.issues.emplace_back(SchemaIssue{SchemaIssueKind::ColumnCountMismatch});
        return result;
    }

    //column name issue
    int colCount = tabInfA.columns.size();
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].name != tabInfB.columns[col].name)
        {
            result.issues.emplace_back(SchemaIssue{SchemaIssueKind::ColumnNameMisMatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    // column type issue
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].logicalType != tabInfB.columns[col].logicalType)
        {
            result.issues.emplace_back(SchemaIssue{SchemaIssueKind::ColumnTypeMisMatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isNullable != tabInfB.columns[col].isNullable)
        {
            result.issues.emplace_back(SchemaIssue{SchemaIssueKind::NullabilityMismatch, col});
        }
    }

    if(!result.isEqual())
        return result;

    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isPartOfPrimaryKey != tabInfB.columns[col].isPartOfPrimaryKey)
        {
            result.issues.emplace_back(SchemaIssue{SchemaIssueKind::PrimaryKeyMismatch, col});
        }
    }

    return result;
}

