/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file count_url.cpp
 * @author work(work@baidu.com)
 * @date 2019/12/25 17:19:04
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include <iostream>
#include <unordered_map>
#include <regex>
#include <sstream>

// 利用正则将url中的核心站点地址取出来
int main(){
    using namespace std;
    regex pattern("http://(.*?)/.*");
    string line;
    unordered_map<string, float[3]> url_map;
    while(getline(cin, line, '\n')){
        int label = 0;
        string url;
        smatch result;
        stringstream tmp_line(line);
        getline(tmp_line, url, '\t');
        tmp_line >> label;
        regex_match(url.cbegin(), url.cend(), result, pattern);
        if (url_map.find(result[1]) != url_map.end()){
            if (label >= 2){
                url_map[result[1]][0] += 1;
            } else {
                url_map[result[1]][1] += 1;
            }
        } else {
            if (label >= 2){
                url_map[result[1]][0] = 1;
            } else {
                url_map[result[1]][1] = 1;
            }
        }
        line.clear();
    }
    vector<pair<string, float>> url_vec;
    for (auto item : url_map){
        url_vec.push_back(make_pair(item.first, item.second[0] * 1.0 / (1 + item.second[1])));
    }
    sort(url_vec.begin(), url_vec.end(), 
        [](pair<string, float> a, pair<string, float> b) 
            {return a.second > b.second;});
    for (size_t i = 0; i < 500; ++i){
        cout << url_vec[i].first << endl;
    }
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
