/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 /**
 * @file segment.cpp
 * @author lichenggang(lichenggang@baidu.com)
 * @date 2014/06/05 13:37:14
 * @version 1
 * @brief 
 *  
 **/

#include "segment.h"

namespace cute_engine
{
Segment::Segment()
{
    _pscw_dict = NULL;
    _pscw_out = NULL;
}

Segment::~Segment()
{
    if (_pscw_dict)
    {
       scw_destroy_worddict(_pscw_dict);
       _pscw_dict = NULL;
    }
    if (_pscw_out)
    {
        scw_destroy_out(_pscw_out);
        _pscw_out = NULL;
    }   
}

bool Segment::init(const char *dict_path, const int max_token_count)
{
    if (NULL == dict_path)
    {
        return false; 
    }

    if (NULL == (_pscw_dict = scw_load_worddict(dict_path)))
    {
        return false;
    }
    if (NULL == (_pscw_out = scw_create_out(max_token_count * 5, SCW_OUT_ALL | SCW_OUT_PROP)))
    {
        return false;
    }

    return true;
}

bool Segment::segment(const char *str)
{
    if (NULL == str)
    {
        return false;
    }
    scw_segment_words(_pscw_dict, _pscw_out, str, strlen(str), 1);
    return true;
}

int Segment::get_token(token_t tokens[], const int size, const TokenType token_type)
{
    int token_count = scw_get_token_1(_pscw_out, token_type, tokens, size);
    return token_count;
}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
