#ifndef TABLEREADSPEC_H
#define TABLEREADSPEC_H

#include <string>
#include <vector>

struct TableReadSpec
{
    std::string tableName{};
    std::vector<std::string> selectedCols{}; // empty vector == *

};

#endif // TABLEREADSPEC_H
