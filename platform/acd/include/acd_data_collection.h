/*
 * Copyright 2002-2015 the original author or authors.
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

// acd manager define

#ifndef _ACD_DATA_COLLECTION_H
#define _ACD_DATA_COLLECTION_H

#include<fstream> 
#include "safe_queue.h"
#include "acd_info.h"
#include "bgcc.h"
#include "thread.h"
#include "locker.h"

typedef struct {
    uint32_t outbound_call_num; //������
    uint32_t outbound_ans_num; //������ͨ��
    uint32_t inbound_call_num; //����绰��
    uint32_t inbound_ans_num; //�����ͨ��
    uint32_t inbound_call_time; //����绰��ʱ��
    uint32_t inbound_alerting_time; //����绰������ʱ��
    uint32_t outbound_call_time; //�����绰��ʱ��
} CCCallData;

typedef struct {
    uint32_t inbound_call_time; //����ͨ��ʱ��
    uint32_t inbound_alerting_time; //��������ʱ��
    uint32_t inbound_call_num; //����绰��
    uint32_t inbound_answer_num; //�����ͨ��
} SkillCallData;

namespace acd {
class AcdCallDataCollection : public Thread {
public:
    AcdCallDataCollection(){}
    ~AcdCallDataCollection(){}

    enum {
        GETBYPLAT,
        GETBYSKILL,
        GETALL
    };
protected:
    virtual void Method();
public:
    typedef std::map<string, SkillCallData*>::iterator iter;
public:

    // @brief ��ʼ��
    void initialize(const char* path);

    // @brief ����ʼ��
    void unInitialize();

    // @brief ���ļ��л�ȡ��������
    int32_t get_data_from_file();

    // @brief ����������д���ļ���
    int32_t put_data_to_file();

    // @brief ����ʱ��ͬ�� ���߸�λ����
    void commit_data();

    // @brief ��������
    // @param callinfo ��������
    void update_data(calldata_ptr& callinfo);

    // @breif ��ȡ�����Ƶ����к�����Ϣ
    // @param calldata �������Ʒ��صĺ�����Ϣ
    // @return 0:success other:failed   
    int32_t get_call_data_by_plat(string& data);

    // @breif ���ݼ������ȡ������Ϣ
    // @param calldata �������鷵�صĺ�����Ϣ
    // @param skill ������
    // @return 0:success other:failed    
    int32_t get_call_data_by_skill(string& data, const string& skill);

    // @breif ���ݼ������ȡ������Ϣ
    // @return 0:success other:failed    
    int32_t get_call_data_by_all(string& data);

    // @breif ��ȡ������Ϣ
    // @param type ��������
    // @param input ����
    // @param result ���ؽ��
    // @return 0:success other:failed    
    acd::AcdResultT get_call_data(int64_t handle, const std::string& agentId,
                                int32_t type, const string& input, string& result);

    // @brief ��λ
    void reset();

    // @breif ���������ݼ������
    // @param calldata ��������
    // @return 0:success other:failed    
    int32_t push_call_data(calldata_ptr& calldata);

private:
    bgcc::RWLock _lock;
    string _cached_file; // �����ļ�·��
    string _cached_date; // ��������
    uint64_t _cached_second; //�����ʱ�䣬��Ϊ��λ
    SafeQueue<calldata_ptr> _calldata_queue;
    std::map<string, SkillCallData*> _skill_call_info; //�������鷵�صĺ�����Ϣ
    CCCallData _cc_call_data; //��ǰ�ĺ�������
};
};

#endif

