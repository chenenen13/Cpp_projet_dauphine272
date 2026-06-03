#pragma once

#include <fstream>
#include <string>
#include <vector>

class CsvWriter {
public:
    explicit CsvWriter(const std::string& file_path);

    void write_row(const std::vector<std::string>& row);

private:
    std::ofstream file_;
};
