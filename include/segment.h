/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 /**
 * @file segment.h
 * @author lichenggang(lichenggang@baidu.com)
 * @date 2014/06/05 13:40:08
 * @version 1
 * @brief 
 *  
 **/
#ifndef PS_SE_SEGMENT_H
#define PS_SE_SEGMENT_H

#include "isegment.h"

namespace cute_engine
{
static const int MAX_PATH_LEN = 1024;

enum TokenType
{
    TOKEN_BASIC = SCW_OUT_BASIC|SCW_OUT_PROP,
    TOKEN_HUNPAI = SCW_OUT_WPCOMP|SCW_OUT_PROP
};

class Segment
{
public:
    Segment();

    ~Segment();

    /**
     * @brief   初始化切词 
     * @return  
     *          true: 成功   
     *          false: 失败
     * @param
     *          dict_path: 切词词典路径
     *          max_token_count: 切出的最多词数
     */
    bool init(const char *dict_path, const int max_token_count);

    /**
     * @brief   切词
     * @return  
     *          true: 切词成功
     *          false: 切词失败
     * @param
     *          str: 待切词字符串
     */
    bool segment(const char *str);

    /**
     * @brief   获取切词结果
     * @param   
     *          tokens:  切词term输出buffer
     *          size:    切词buffer大小
     *          buff_id: 获取的切词类型
     * @return
     */
    int get_token(token_t tokens[], const int size, const TokenType token_type);

private:
    scw_worddict_t *_pscw_dict;  // 切词词典
    scw_out_t *_pscw_out;  // 切词结果buffer
};
}

#endif  //PS_SE_SEGMENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
