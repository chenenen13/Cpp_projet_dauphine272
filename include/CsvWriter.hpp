#pragma once

#include <fstream>
#include <string>
#include <vector>

class CsvWriter {
public:
    explicit CsvWriter(const std::string& file_path);

    void write_row(const std::vector<std::string>& row);
    bool is_open() const;

private:
    std::ofstream file_;
};