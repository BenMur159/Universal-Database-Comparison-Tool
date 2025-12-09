#ifndef SQLSERVERROWREADER_H
#define SQLSERVERROWREADER_H

#include <string>
#include <vector>
#include <optional>

#include "IRowReader.h"

typedef void* SQLHSTMT;

class SqlServerRowReader : public IRowReader
{
public:
    explicit SqlServerRowReader(SQLHSTMT stmt);
    ~SqlServerRowReader() noexcept override;

    SqlServerRowReader(const SqlServerRowReader&) = delete;
    SqlServerRowReader& operator=(const SqlServerRowReader&) = delete;
    SqlServerRowReader(SqlServerRowReader&&) = delete;
    SqlServerRowReader& operator=(SqlServerRowReader&&) = delete;

    bool next() override;
    std::optional<std::string> getString(int col) const override;
    int getColumnCount() const override;

private:
    SQLHSTMT m_stmt;

    struct ColState{
        bool fetched{false};
        bool isNull{false};
        std::string value{};
    };

    mutable std::vector<ColState> m_cols;
    mutable int m_colCount{0};

};

#endif // SQLSERVERROWREADER_H
