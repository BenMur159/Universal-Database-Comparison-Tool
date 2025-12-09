#ifndef IROWREADER_H
#define IROWREADER_H

#include <string>
#include <optional>

class IRowReader
{
public:
    virtual ~IRowReader() = default;

    IRowReader(const IRowReader &) = delete;
    IRowReader(IRowReader &&) = delete;
    IRowReader &operator=(const IRowReader &) = delete;
    IRowReader &operator=(IRowReader &&) = delete;

    virtual bool next() = 0;
    virtual std::optional<std::string> getString(int col) const = 0;
    virtual int getColumnCount() const = 0;

protected:
    IRowReader() = default;
};

#endif // IROWREADER_H
