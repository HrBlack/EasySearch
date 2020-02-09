/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file data.cpp
 * @author liushihao(liushihao02@baidu.com)
 * @date 2019/12/16 10:56:19
 * @version $Revision$ 
 * @brief 
 *  
 **/

/* vim: set ts=4 sw=4 sts=4 tw=100 */

#include "data.h"
 
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// 加载训练数据
Data load_data(
            const char* file_path, 
            std::unordered_map<std::string, float>& high_quality_web){
    using namespace std;
    Data data;
    ifstream inputs;
    string tmp_line;
    unordered_map<string, int> url_dict;
    inputs.open(file_path);
    string url_split;
    while (!inputs.eof()){
        int splash_count = 0;
        DataItem data_item;
        getline(inputs, tmp_line, '\n');
        if (tmp_line == "\0"){
            return data;
        }
        stringstream line(tmp_line);
        getline(line, data_item.query, '\t');
        getline(line, data_item.doc, '\t');
        getline(line, data_item.url, '\t');
        // 对url做去重
        if (url_dict.find(data_item.url) == url_dict.end()){
            url_dict[data_item.url] = 1;
        } else{
            continue;
        }
        // 给高权威性的网站加分
        for (auto web : high_quality_web){
            if (data_item.url.find(web.first) != std::string::npos){
                data_item.url_score += web.second;
                break;
            }
        }
        // url的splash和数字特征，暂且搁置
        //std::stringstream url_stream(data_item.url);
        //while (!url_stream.eof()){
        //    getline(url_stream, url_split, '/');
        //    splash_count += 1;
        //}
        //data_item.url_score -=  0.5 * splash_count;
        //for (auto s : url_split){
        //    if (std::isdigit(s)){
        //        data_item.url_score = -1;
        //        break;
        //    }
        //}
        line >> data_item.label;
        data.push_back(data_item);
    }
    inputs.close();
    return data;
}

// 加载停词词表
std::unordered_map<std::string, float> load_stopwords(const char* file_path){
    using namespace std;
    unordered_map<string, float> stopwords;
    string line;
    ifstream inputs;
    inputs.open(file_path);
    while (!inputs.eof()){
        getline(inputs, line, '\n');
        stopwords[line] = 0.5;
    }
    inputs.close();
    return stopwords;
}

