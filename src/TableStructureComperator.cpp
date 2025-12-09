#include "dbdiff/TableStructureComperator.h"

TableStructureDiffResult compareTableStructure(const TableInfo &tabInfA, const TableInfo &tabInfB)
{
    TableStructureDiffResult result{};

    // 1.   Do both DB-Tables have the same amount of columns?
    if(tabInfA.columns.size() != tabInfB.columns.size())
    {
        result.issues.emplace_back(StructureIssue{StructureIssueKind::ColumnCountMismatch});
        return result;
    }

    //2.    Do both DB-Tables have the same column names in the same order?
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

    // 3.   Do the Types of both DB-Tables distill down to the same logical type?
    //      (e.g. Sqlite3::text == SqlServer::varchar(n)) -> both "string-ish"
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].logicalType != tabInfB.columns[col].logicalType)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::ColumnTypeMismatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    // 4.   Do both DB-Tables follow the same Nullability rules?
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isNullable != tabInfB.columns[col].isNullable)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::NullabilityMismatch, col});
        }
    }
    if(!result.isEqual())
        return result;

    // 5.   Do both DB-Tables have the same Primary-Key columns?
    for(int col{0}; col < colCount; ++col)
    {
        if(tabInfA.columns[col].isPartOfPrimaryKey != tabInfB.columns[col].isPartOfPrimaryKey)
        {
            result.issues.emplace_back(StructureIssue{StructureIssueKind::PrimaryKeyMismatch, col});
        }
    }

    // 6.   Here we ether return a Primary Key issue or the result is empty,
    //      meaning the tables are equal in terms of structure.
    return result;
}

