/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file data.h
 * @author liushihao(liushihao02@baidu.com)
 * @date 2019/12/16 11:00:21
 * @version $Revision$ 
 * @brief 
 *  
 **/

#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <unordered_map>

struct DataItem{
    std::string query;
    std::string doc;
    std::string url;
    float url_score = 0;
    int label;
};

struct HitList{
    int doc_id;
    int freq;
    int word_count;
    std::vector<int> positions;
};

typedef std::vector<DataItem> Data;

Data load_data(const char*, std::unordered_map<std::string, float>&);
std::unordered_map<std::string, float> load_stopwords(const char*);

#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 */
