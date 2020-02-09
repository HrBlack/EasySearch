/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file main.cpp
 * @author liushihao(liushihao02@baidu.com)
 * @date 2019/12/16 11:04:30
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "data.h"
#include "segment.h"
#include "skip_list.h"

#include <cmath>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <vector>

using std::vector;
using std::string;
using std::endl;
using std::cin;
using std::cout;
using cute_engine::Segment;
using cute_engine::TOKEN_BASIC;

typedef std::unordered_map<std::string, SkipList*> InvertedIndex;

// 切词接口
std::vector<std::string> segment(
                            Segment* psegment, 
                            std::unordered_map<string, float>& stopwords, 
                            const char* str, 
                            bool cut_stopwords = true){
    if (!psegment->segment(str)){
        std::cerr << "FATAL: fail to segment str[" << str << "]" << endl;
        return {""};
    }
    token_t tokens[1024];
    int token_count = psegment->get_token(tokens, 1024, TOKEN_BASIC);
    vector<string> segmented_words;
    for (int i = 0; i < token_count; ++i){
        // 去停词
        if (cut_stopwords && stopwords.find(tokens[i].buffer) != stopwords.end()){
            continue;
        }
        char *str = tokens[i].buffer;
        if (std::isalpha(str[0])){
            for (int j = 0; j < strlen(str); ++j){
                str[j] = std::tolower(str[j]);
            }
        }
        segmented_words.push_back(tokens[i].buffer);
    }
    return segmented_words;
}

// 构建倒排表，拉链部分为跳表
void build_inverted_index(Data& data, 
                          InvertedIndex& inverted_index, 
                          Segment* psegment, 
                          std::unordered_map<string, float>& stopwords){
    for (size_t i = 0; i < data.size(); ++i){
        vector<string> segmented_doc = segment(psegment, stopwords, data[i].doc.c_str());
        for (size_t j = 0; j < segmented_doc.size(); ++j){
            // 单词第一次放进表里
            if (inverted_index.find(segmented_doc[j]) == inverted_index.end()){
                SkipList *skip_list = new SkipList(0.5, 10);
                skip_list->insert(i, {i, 1, segmented_doc.size(), {static_cast<int>(j)}});
                inverted_index[segmented_doc[j]] = skip_list;
            }
            else{
                SkipNode* node = inverted_index[segmented_doc[j]]->search(i);
                // 单词已经在表里，但是单词对应的doc不在
                if (node == nullptr){
                    inverted_index[segmented_doc[j]]->insert(i, 
                            {i, 1, segmented_doc.size(), {static_cast<int>(j)}});
                } 
                // 单词和doc都已在表中，更新信息
                else{
                    node->value.freq += 1;
                    node->value.positions.push_back(j);
                }
            }
        }
    }
}

// 基于跳表的倒排表归并
vector<vector<HitList>> intersect_zipper(
                vector<string>& segmented_query, 
                const InvertedIndex& inverted_index,
                int data_size){
    vector<vector<HitList>> intersection;
    vector<HitList> doc_list;
    vector<SkipNode*> pointer;
    vector<float> idf_score;
    size_t n = segmented_query.size();
    int target_id = 0;
    float idf_sum = 0;
    //判断term是否在倒排表中，如果不在则将其舍弃；计算idf值
    int i = 0;
    while (i < n){
        if (inverted_index.find(segmented_query[i]) == inverted_index.end()){
            printf("该单词不在倒排表中：%s\n", segmented_query[i].c_str());
            segmented_query.erase(segmented_query.begin() + i);
            n -= 1;
        } else{
            float idf = log(1.0 * data_size / inverted_index.at(segmented_query[i])->size);
            idf_sum += idf;
            idf_score.push_back(idf);
            i++;
        }
    }
    // 当term数量 <= 2的情况
    if (n == 0){
        return intersection;
    } else if (n == 1){
        SkipNode *node = inverted_index.at(segmented_query[0])->head->forward[0];
        while (node->key != INT_MAX){
            doc_list.push_back(node->value);
            intersection.push_back(doc_list);
            doc_list.clear();
            node = node->forward[0];
        }
        return intersection;
    }
    // 初始化检索条件，使最短的链表作为检索标准
    size_t index = 0;
    int list_length = INT_MAX;
    vector<SkipList*> skip_list;
    for (size_t i = 0; i < n; ++i){
        // 4个单词以上的query才会被考虑按term重要性剔除term
        idf_score[i] /= idf_sum;
        if (n >= 4 && idf_score[i] < 0.5 / n){
            continue;
        }
        skip_list.push_back(inverted_index.at(segmented_query[i]));
        pointer.push_back(skip_list.back()->head);
        pointer.back() = pointer.back()->forward[0];
        if (skip_list.back()->size < list_length){
            index = skip_list.size() - 1;
            list_length = skip_list.back()->size;
        }
        // 打印跳表的结构
        //inverted_index.at(segmented_query[i])->print();
    }
    std::swap(pointer[index], pointer[0]);
    std::swap(skip_list[index], skip_list[0]);
    target_id = pointer[0]->key;
    doc_list.push_back(pointer[0]->value);
    n = skip_list.size();
    // 开始检索
    while (true){
        for (size_t i = 1; i < n; ++i){
            // 判断终止条件
            if (pointer[i]->key == INT_MAX || target_id == INT_MAX){
                return intersection;
            } else if (pointer[i]->key > target_id){
                break;
            } else if (pointer[i]->key == target_id){
                doc_list.push_back(pointer[i]->value);
                if (i == n - 1){
                    intersection.push_back(doc_list);
                }
                continue;
            }
            int level = skip_list[i]->get_node_level(pointer[i]->forward);
            // 在跳表中寻找target id的位置
            for (int j = level - 1; j >= 0; --j){
                while (pointer[i]->forward[j] != nullptr && 
                       pointer[i]->forward[j]->key < target_id && 
                       pointer[i]->forward[j]->key != INT_MAX){
                    pointer[i] = pointer[i]->forward[j];
                }
            }
            pointer[i] = pointer[i]->forward[0];
            // 下面条件分别为：跳表已到尾部；target id没找到；已找到
            if (pointer[i]->key == INT_MAX){
                return intersection;
            } else if (pointer[i]->key > target_id){
                break;
            } else if (pointer[i]->key == target_id){
                doc_list.push_back(pointer[i]->value);
                if (i == n - 1){
                    intersection.push_back(doc_list);
                }
            }
        }
        //一轮遍历结束，更新target id
        doc_list.clear();
        if (pointer[0]->forward[0]->key == INT_MAX){
            return intersection;
        } else{
            pointer[0] = pointer[0]->forward[0];
            target_id = pointer[0]->key;
            doc_list.push_back(pointer[0]->value);    
        }
    }
}

// 基于tf-idf的排序
namespace cute_engine{
vector<std::pair<int, double>> rank(
                            const vector<string>& segmented_query, 
                            const vector<vector<HitList>>& recall_docs, 
                            const InvertedIndex& inverted_index, 
                            const Data& data,
                            const string& raw_query){
    vector<std::pair<int, double>> doc_scores;
    if (recall_docs.size() < 3){
        return doc_scores;
    }
    for (auto doc_list : recall_docs){
        double score = 0;
        int position_sum = 0;
        float initial_score = 1.5;
        for (size_t i = 0; i < doc_list.size(); ++i){
            double tf = std::min(5, doc_list[i].freq) * 1.0 / doc_list[i].word_count;
            string word = segmented_query[i];
            double idf = log(1.0 * data.size() / inverted_index.at(word)->size);
            score += tf * idf;
            position_sum += doc_list[i].positions[0];
        }
        string::size_type pos = 0;
        DataItem data_item = data[doc_list[0].doc_id];
        // 对于单词个数大于2的长query，连续全命中加分
        if (segmented_query.size() > 2){        
            while ((pos = data_item.doc.find(raw_query, pos)) != string::npos){
                score += initial_score / (pos + 1);
                pos += raw_query.length();
            }
        } 
        // 短query：百科额外加分
        else if (data_item.url.find("baike.baidu.com") != string::npos){
            score += 1.0;
        }
        // 命中位置很靠前的话，额外加分
        if (position_sum <= 2){
            score += 1.5;
        } else if (position_sum <= 4){
            score += 0.5;
        }
        score += data[doc_list[0].doc_id].url_score;
        doc_scores.push_back(std::make_pair(doc_list[0].doc_id, score));
    }
    std::sort(doc_scores.begin(),
         doc_scores.end(),
         [] (const std::pair<int, double>& a, 
             const std::pair<int, double>& b) {return a.second > b.second;});
    return doc_scores;
}
}

int main(){
    // 初始化权威性库
    const char webs[] = "/home/work/liushihao/cute_engine/corpus/high_quality_webs";
    std::unordered_map<string, float> high_quality_web = load_stopwords(webs);
    std::unordered_map<string, float> insertion {
                                        {"baike.baidu.com", 1.5}, 
                                        {"baike.soso.com", 1.5},
                                        {"zhidao.baidu.com", 0.3}};
    for (auto item : insertion){
        high_quality_web.insert(item);
    }
    // 加载训练/测试数据
    Data data = load_data(
                "/home/work/liushihao/cute_engine/corpus/ltr_label_title.train",
                high_quality_web);
    // 初始化词典
    Segment *psegment = new Segment();
    const char stop_dict[] = "/home/work/liushihao/cute_engine/stopwords.txt";
    const char word_dict[] = "/home/work/liushihao/cute_engine/worddict";
    psegment->init(word_dict, 1024);
    std::unordered_map<string, float> stopwords = load_stopwords(stop_dict);
    // 构建倒排表
    InvertedIndex inverted_index;
    build_inverted_index(data, inverted_index, psegment, stopwords);

    // 测试倒排表
    //char str[] = "攻击原理";
    //for (auto term : segment(psegment, str)){
    //    SkipList* doc_list = inverted_index[term];
    //    SkipNode* x = doc_list->head;
    //    while (x->forward[0]->key != INT_MAX){
    //        x = x->forward[0];
    //        cout << data[x->key].doc << endl;
    //    }
    //}

    DataItem item;
    clock_t t1 = clock();

    // 从终端读取测试数据
    string line;
    int query_count = 0;
    while (getline(cin, line, '\n')){
        query_count += 1;
        item.query = line;
        line.clear();
        printf("对query做分词：%s\n",item.query.c_str());
        vector<string> segmented_query = segment(psegment, 
                                                 stopwords, 
                                                 item.query.c_str());
        printf("开始拉链\n");
        vector<vector<HitList>> recall_list = intersect_zipper(segmented_query, 
                                                               inverted_index, 
                                                               data.size());
        printf("开始排序\n");
        vector<std::pair<int, double>> doc_scores = cute_engine::rank(segmented_query, 
                                                                      recall_list, 
                                                                      inverted_index, 
                                                                      data,
                                                                      item.query);
        printf("排序已结束\n");
        // 输出top 10结果
        int d = 0;
        for (auto doc_pair : doc_scores){
            if (d >= 10){
                break;
            }
            printf("%s\t%s\t%s\t%f\n", 
                    item.query.c_str(),
                    data[doc_pair.first].doc.c_str(),
                    data[doc_pair.first].url.c_str(),
                    doc_pair.second);
            d++;
        }        
        item.query.clear();
    }
    delete psegment;
    double t_diff = (clock() - t1) * 1.0 / CLOCKS_PER_SEC * 1000;
    printf("平均每条query用时：%fms", t_diff/query_count);
    return 0;
}
