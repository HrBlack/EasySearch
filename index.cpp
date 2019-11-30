// #include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

typedef vector<DocItem> DocList;

class DocItem
{
    public:
    int doc_id, frequency;
    vector<int> position;
    DocItem() = default;
    DocItem(int doc_id, int frequency, vector<int> position): 
        doc_id(doc_id), frequency(frequency), position(position){};
};

vector<string> segmentation(string doc_content){
    /* Do segmentation for the original content of a document,
    return the tokenized content in vector */
    return {"电脑", "笔记本电脑", "键盘", "鼠标", "电脑", "键盘"};
}

unordered_map<string, vector<DocItem>> indexing(vector<string> doc_content){
    unordered_map<string, vector<DocItem>> inverted_index;
    int doc_id = 1;
    for (int i = 0; i < doc_content.size(); i++){
        string word = doc_content[i];
        int count = 0;
        if (inverted_index.find(word) == inverted_index.end()){
            inverted_index[word] = {DocItem(doc_id, ++count, {0})};
        }
        else{
            DocItem & last_item = inverted_index[word].back();
            if (last_item.doc_id == doc_id){
                last_item.frequency++;
            }
        }
    }
    return inverted_index;
}

unordered_map<string, vector<DocItem>> inverted_index;
DocList search(string query){
    vector<string> tokenized_query = segmentation(query);
    vector<DocItem> intersection;
    for (string term : tokenized_query){
        if (inverted_index.find(term) != inverted_index.end()){
            return;
        }
    }
}

vector<float> ranking(){
    int tf;
    float idf;
    float weight = tf * idf;
    return {weight};
}

int main(){
    vector<string> str = segmentation("s");
    unordered_map<string, vector<DocItem>> index = indexing(str);
    for (auto iter = index.begin(); iter != index.end(); iter++){
        cout << (*iter).first << '\t' << (*iter).second.back().frequency << endl;
    }
}