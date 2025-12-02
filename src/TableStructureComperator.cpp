#include "TableStructureComperator.h"

TableStructureDiffResult compareTableStructure(const TableInfo &tabInfA, const TableInfo &tabInfB)
{
    TableStructureDiffResult result{};
    // column count issue
    if(tabInfA.columns.size() != tabInfB.columns.size())
    {
        result.issues.emplace_back(StructureIssue{StructureIssueKind::ColumnCountMismatch});
        return result;
    }

    //column name issue
    int colCount = tabInfA.columns.size();
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].name != tabInfB.columns[col].name)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::ColumnNameMismatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    // column type issue
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].logicalType != tabInfB.columns[col].logicalType)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::ColumnTypeMismatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isNullable != tabInfB.columns[col].isNullable)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::NullabilityMismatch, col});
        }
    }

    if(!result.isEqual())
        return result;

    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isPartOfPrimaryKey != tabInfB.columns[col].isPartOfPrimaryKey)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::PrimaryKeyMismatch, col});
        }
    }

    return result;
}

