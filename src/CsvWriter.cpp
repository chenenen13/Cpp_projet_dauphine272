#include "CsvWriter.hpp"

#include <stdexcept>

CsvWriter::CsvWriter(const std::string& file_path) : file_(file_path) {
    if (!file_.is_open()) {
        throw std::runtime_error("Could not open CSV output file: " + file_path);
    }
}

void CsvWriter::write_row(const std::vector<std::string>& row) {
    for (std::size_t i = 0; i < row.size(); ++i) {
        file_ << row[i];
        if (i + 1 < row.size()) {
            file_ << ",";
        }
    }
    file_ << "\n";
}

bool CsvWriter::is_open() const {
    return file_.is_open();
}