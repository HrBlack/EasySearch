#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>

struct DataItem{
    std::string query;
    std::string doc;
    int label;
};

typedef std::vector<DataItem> Data;

Data load_data(const char* file_path);

#endif