/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file src/skip_list.h
 * @author liushihao(liushihao02@baidu.com)
 * @date 2019/12/17 19:08:18
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include "data.h"

#include <climits>
#include <iostream>
#include <vector>

struct SkipNode{
    int key;
    HitList value;
    int level;
    std::vector<SkipNode*> forward;
    SkipNode(int k, HitList v, int level);
};

class SkipList{
public:
    SkipList() = default;
    SkipList(float, int);
    ~SkipList();
    
    void insert(int doc_id, HitList value);
    void erase(int doc_id);
    SkipNode* search(int key);
    SkipNode* make_new_node(int key, HitList value, int level);
    int get_node_level(const std::vector<SkipNode*>);
    int choose_random_level();
    void print();
    
    SkipNode* head;
    SkipNode* tail;
    int max_level;
    float prob;
    int size = 0;
};

#endif

/* vim: set ts=4 sw=4 sts=4 tw=100 */
