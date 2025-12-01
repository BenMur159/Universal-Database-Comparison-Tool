#ifndef DATABASESCHEMAINFOTYPES_H
#define DATABASESCHEMAINFOTYPES_H

#include <string>
#include <vector>
#include <cstddef>
#include <unordered_map>

enum class LogicalType {
    Integer,
    Real,
    Text,
    Blob,
    DateTime,
    Boolean,
    Other
};

struct ColumnInfo
{
    std::string name;
    std::string dbType;
    LogicalType logicalType{LogicalType::Other};
    bool isNullable = true;
    bool isPartOfPrimaryKey = false;
};

struct TableInfo
{
    std::string name;
    std::vector<ColumnInfo> columns;
    std::vector<std::size_t> primaryKeyColumnIndices; // indices into columns
};

struct DatabaseSchema
{
    std::unordered_map<std::string, TableInfo> tablesByName;
};

#endif // DATABASESCHEMAINFOTYPES_H
