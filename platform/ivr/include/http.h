/*
 * Copyright 2002-2014 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      CC/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef  __HTTP_H_
#define  __HTTP_H_

#include <stdint.h>
#include <string>

/**
* @brief curl��http�����װ��
*   detail description
*
*/
class http_t {
private:
    const static uint32_t MAX_BUF = 40960;
    /**
    * @brief curl�õ��Ľṹ��
    *   detail description
    *
    */
    struct write_data_t {
        char* wr_buf;
        uint32_t  wr_index;
        int32_t  wr_err;
        size_t maxlen;
    };

    /**
     * @brief http�ص��ĺ���
     *
     * @param [in/out] buffer   : void*
     * @param [in/out] size   : size_t
     * @param [in/out] nmemb   : size_t
     * @param [in/out] userp   : void*
     * @return  size_t
     * @retval
     * @see
     * @note
     * @author chenyuzhen
     * @date 2011/09/02 18:05:20
    **/
    static size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp);
public:
    /**
     * @brief ����http����POST
     *
     * @param [in/out] url   : const char*
     * @param [in/out] timeout   : int32_t
     * @return bool
     * @retval
     * @see
     * @note
     * @author chenyuzhen
     * @date 2011/09/02 18:05:32
    **/
    static bool  post(
        const char* url,
        std::string& out, int32_t timeout);
    /**
     * @brief ����http����GET
     *
     * @param [in/out] url   : const char*
     * @param [in/out] timeout   : int32_t
     * @return bool
     * @retval
     * @see
     * @note
     * @author Stars
     * @date 2011/11/21 16:01:11
    **/
    static bool  get(
        const char* url,
        std::string& out, int32_t timeout);
};

#endif  //__HTTP_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
