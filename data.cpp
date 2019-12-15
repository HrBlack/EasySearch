#include "data.h"

#include <fstream>
#include <sstream>
#include <iostream>

Data load_data(const char* file_path){
    using namespace std;
    Data data;
    ifstream inputs;
    string tmp_line;
    DataItem data_item;
    inputs.open(file_path);
    while (!inputs.eof()){
        getline(inputs, tmp_line, '\n');
        if (tmp_line == "\0"){
            return data;
        }
        stringstream line(tmp_line);
        line >> data_item.query >> data_item.doc >> data_item.label;
        data.push_back(data_item);
    }
    inputs.close();
    return data;
}

// int main(){
//     Data& data = load_data("train.txt");
//     std::cout << data[0].label;
// }