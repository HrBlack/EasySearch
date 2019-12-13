// #include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// typedef vector<DocItem> DocList;

// class DocItem
// {
//     public:
//     int doc_id, frequency;
//     vector<int> position;
//     DocItem() = default;
//     DocItem(int doc_id, int frequency, vector<int> position): 
//         doc_id(doc_id), frequency(frequency), position(position){};
// };

class DocList
{
    public:
    vector<int> doc_ids, frequencies;
    vector<vector<int>> positions;
    DocList() = default;
    DocList(vector<int> doc_ids, vector<int> frequencies, vector<vector<int>> positions): 
        doc_ids(doc_ids), frequencies(frequencies), positions(positions){};
};

vector<string> segmentation(string doc_content){
    /* Do segmentation for the original content of a document,
    return the tokenized content in vector */
    return {"这个", "笔记本电脑", "多少钱"};
}

unordered_map<string, DocList> indexing(vector<string> doc_content){
    unordered_map<string, DocList> inverted_index;
    int doc_id = 1;
    for (int i = 0; i < doc_content.size(); i++){
        string word = doc_content[i];
        int count = 0;
        if (inverted_index.find(word) == inverted_index.end()){
            inverted_index[word] = DocList({doc_id}, {++count}, {{0}});
        } else{
            if (inverted_index[word].doc_ids.back() == doc_id){
                inverted_index[word].frequencies.back() ++;
            }
        }
    }
    return inverted_index;
}

unordered_map<string, DocList> inverted_index;

DocList look_up(string query){
    vector<string> tokenized_query = segmentation(query);
    vector<int> doc_intersect;
    vector<vector<int>> frequency_intersect;
    for (int i = 0; i < inverted_index[tokenized_query[0]].doc_ids.size(); ++i){
        int base_id = inverted_index[tokenized_query[0]].doc_ids[i];
        int total = tokenized_query.size(), count = 1;
        vector<int> freq_of_term_in_doc;
        for (int j = 1; j < tokenized_query.size(); ++j){
            int id = binary_search(inverted_index[tokenized_query[j]].doc_ids, base_id);
            if (id == -1){
                freq_of_term_in_doc = {};
                break;
            } else{
                freq_of_term_in_doc.push_back(inverted_index[tokenized_query[j]].frequencies[id]);
                count ++;
            }
        }
        if (total == count){
            doc_intersect.push_back(base_id);
            frequency_intersect.push_back(freq_of_term_in_doc);
        }
    }

    // 利用set存储
    // for (string word : tokenized_query){
    //     for (int doc_id : inverted_index[word].doc_ids){
    //         doc_intersection.insert(doc_id);
    //     }
    // }
    // DocList intersection;
    // for (string term : tokenized_query){
    //     if (inverted_index.find(term) != inverted_index.end()){
    //         return;
    //     }
    // }
}

int binary_search(vector<int> vec, int target){
    int i = 0, j = vec.size()-1;
    if (target < vec[i] || target > vec[j]){
        return -1;
    }
    while (i < j){
        int mid = (i+j)/2;
        if (vec[mid] < target){
            i = mid + 1;
        } else if(vec[mid] > target){
            j = mid - 1;
        } else{
            return mid;
        }
    }
    return -1;
}

vector<float> ranking(){
    int tf;
    float idf;
    float weight = tf * idf;
    return {weight};
}

int main(){
    vector<string> str = segmentation("s");
    unordered_map<string, DocList> index = indexing(str);
    for (auto iter = index.begin(); iter != index.end(); iter++){
        cout << (*iter).first << '\t' << (*iter).second.frequencies.back() << endl;
    }
}