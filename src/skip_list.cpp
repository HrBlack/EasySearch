/***************************************************************************
 * 
 * Copyright (c) 2019 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file skip_list.cpp
 * @author liushihao(liushihao02@baidu.com)
 * @date 2019/12/17 15:55:55
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "data.h"
#include "skip_list.h"
using std::cout;
using std::endl;
// 构造函数
SkipNode::SkipNode(int k, HitList v, int level) : key(k), value(v), level(level){
    for (int i = 0; i < level; ++i){
        forward.emplace_back(nullptr);
    }
}

SkipList::SkipList(float p = 0.5, int l = 5) : prob(p), max_level(l){
    head = new SkipNode(INT_MIN, {-1, 0, 0, {1}}, max_level);
    tail = new SkipNode(INT_MAX, {-1, 0, 0, {1}}, max_level);
    for (size_t i = 0; i < max_level; ++i){
        head->forward[i] = tail;
    }
}

SkipList::~SkipList(){
    delete head;
    delete tail;
}

// 获取节点的层级
int SkipList::get_node_level(const std::vector<SkipNode*> forward_ptr){
    int maximum_level = 0;
    if (forward_ptr[0]->key == INT_MAX){
        return 1;
    }
    for (size_t i = 0; i < forward_ptr.size(); ++i){
        if (forward_ptr[i] != nullptr && forward_ptr[i]->key != INT_MAX){
            ++ maximum_level;
        } else{
            break;
        }
    }
    return maximum_level;
}

// 在跳表中做查询
SkipNode* SkipList::search(int search_key){
    SkipNode *x = head;
    int current_max_level = get_node_level(x->forward);
    //int current_max_level = max_current_level;
    for (int i = current_max_level - 1; i >= 0; --i){
        while (x->forward[i] != nullptr && x->forward[i]->key < search_key){
            x = x->forward[i];
        }
    }
    x = x->forward[0];
    if (x->key == search_key){
        return x;
    } else{
        return nullptr;
    }
}

// 为跳表增加新节点，在insert函数中被调用
SkipNode* SkipList::make_new_node(int key, HitList val, int level){
    return new SkipNode(key, val, level);
}

// 插入新节点至跳表尾端
void SkipList::insert(int doc_id, HitList value){
    size += 1;
    SkipNode* x = nullptr;
    std::vector<SkipNode*> update(head->forward);
    int current_max_level = get_node_level(head->forward);
    x = head;
    for (int i = current_max_level - 1; i >= 0; --i){
        while (x->forward[i] != nullptr && x->forward[i]->key < doc_id){
            x = x->forward[i];
        }
        update[i] = x;
    }
    x = x->forward[0];
    int new_node_level = 1;
    if (x->key != doc_id){
        new_node_level = choose_random_level();
        int cnt_level = get_node_level(update);
        if (new_node_level > cnt_level){
            for (int i = cnt_level; i < new_node_level; ++i){
                update[i] = head;
            }
        }
        x = make_new_node(doc_id, value, new_node_level);
    }
    for (int i = 0; i < new_node_level; ++i){
        x->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = x;
    }
}

// 插入新节点时，随机选择新节点的层级
int SkipList::choose_random_level(){
    int v = 1;
    while ((std::rand() * 1.0 / RAND_MAX) < prob && v < max_level){
        v += 1;
    }
    return v;
}

// 打印跳表的结构
void SkipList::print(){
    SkipNode* x = head;
    std::cout <<"{";
    while (x->forward[0] != nullptr){
        std::cout << "key: " << x->forward[0]->key <<
        "\t" << "level: " << get_node_level(x->forward) << std::endl;
    x = x->forward[0];
    }
    std::cout << "}" << std::endl;
}

