#include "data.h"

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

struct HitList{
    int freq;
    vector<int> positions;
};

typedef unordered_map<int, HitList> DocList;
typedef unordered_map<string, DocList> InvertedIndex;

void build_lexicon(){
    // 切词并建立单词和doc的编号（本题不建立doc的字典也行，因为doc并没有重复）
}

vector<string> segment(string doc_content){
    // 切词工具，待引入
    return {"今天", "天气", "今天", "晴朗"};
}

void build_forward_index(){
    // TBC
}

void build_inverted_index(const Data& data, InvertedIndex& inverted_index){
    for (size_t i = 0; i < data.size(); ++i){
        vector<string> segmented_doc = segment(data[i].doc);
        for (size_t j = 0; j < segmented_doc.size(); ++j){
            // 该单词不在倒排表的情况
            if (inverted_index.find(segmented_doc[j]) == inverted_index.end()){
                DocList initial_map;
                initial_map[i] = {1, {static_cast<int>(j)}};
                inverted_index[segmented_doc[j]] = initial_map;
            }
            // 该单词在倒排表中，但doc还未读取过
            else if (inverted_index[segmented_doc[j]].find(i) == inverted_index[segmented_doc[j]].end()){
                inverted_index[segmented_doc[j]][i] = {1, {static_cast<int>(j)}};
            }
            // 单词和doc都在表里
            else{
                inverted_index[segmented_doc[j]][i].freq += 1;
                inverted_index[segmented_doc[j]][i].positions.push_back(j);
            }
        }
    }
}

// 搜索模块：对query term进行倒排拉链合并，未来考虑对拉链进行截断处理
vector<int> intersect_zipper(const vector<string>& segmented_query, const InvertedIndex& inverted_index){
    unordered_map<int, int> doc_counter;
    vector<int> intersection;
    for (auto word : segmented_query){
        for (auto doc : inverted_index.at(word)){
            if (doc_counter.find(doc.first) == doc_counter.end()){
                doc_counter[doc.first] = 1;
            }else{
                doc_counter[doc.first] += 1;
            }
        }
    }
    for (auto item : doc_counter){
        if (doc_counter[item.second] == segmented_query.size()){
            intersection[item.first] = item.second;
        }
    }
    return intersection;
}

namespace abc{
// 排序模块
vector<pair<int, double>> rank(vector<string>& segmented_query, vector<int>& recall_docs, InvertedIndex& inverted_index){
    // tf-idf方法
    vector<pair<int, double>> doc_scores;
    for (auto doc : recall_docs){
        double score = 0;
        for (auto word : segmented_query){
            int tf = inverted_index[word][doc].freq;
            double idf = 1.0 / inverted_index[word].size();
            score += tf * idf;
        }
        doc_scores.push_back(make_pair(doc, score));
    }
    sort(doc_scores.begin(), 
         doc_scores.end(), 
         [] (const pair<int, double> &a, const pair<int, double> &b) {return a.second > b.second;});
    return doc_scores;
}
}

int main(){
    Data data = load_data("train.txt");
    // unordered_map<string, int> lexicon;

    // 建立倒排表
    InvertedIndex inverted_index;
    build_inverted_index(data, inverted_index);

    // 对query term进行倒排拉链合并，然后完成排序
    for (auto item : data){
        vector<string> segmented_query = segment(item.query);
        vector<int> recall_list = intersect_zipper(segmented_query, inverted_index);
        vector<pair<int, double>> doc_scores = abc::rank(segmented_query, recall_list, inverted_index);
    }
}

// int binary_search(vector<int> vec, int target){
//     int i = 0, j = vec.size()-1;
//     if (target < vec[i] || target > vec[j]){
//         return -1;
//     }
//     while (i < j){
//         int mid = (i+j)/2;
//         if (vec[mid] < target){
//             i = mid + 1;
//         } else if(vec[mid] > target){
//             j = mid - 1;
//         } else{
//             return mid;
//         }
//     }
//     return -1;
// }