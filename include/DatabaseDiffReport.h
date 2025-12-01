#ifndef DATABASEDIFFREPORT_H
#define DATABASEDIFFREPORT_H

#include <string>
#include <vector>
#include "DatabaseSchemaInfoTypes.h"

struct RowDifferences
{
    RowDifferences(const std::string& row, std::size_t a, std::size_t b)
        : canonicalRow{row}, countA{a}, countB{b}
    {}
    std::string canonicalRow{};
    // How often does this row appear in the databases
    // FYI: we are not guaranteed that rows are unique
    std::size_t countA{0};
    std::size_t countB{0};
};


enum class TableDiffKind
{
    OnlyInA,
    OnlyInB,
    ColumMismatch,
    DataMismatch,
    Equal
};

struct TableDiffReport
{
    std::string tableNameA{}; // empty if only in B
    std::string tableNameB{}; // empty if only in A

    TableDiffKind diffKind{TableDiffKind::Equal};

    TableInfo tabInfA{};
    TableInfo tabInfB{};

    // only not empty when DataMissMatch;
    std::vector<RowDifferences> rowDiffs{};
};

struct DatabaseDiffReport
{
    bool isEqual{true}; // true if all tables are equal

    std::vector<TableDiffReport> tables;
};

#endif // DATABASEDIFFREPORT_H
