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

// ivr data collection define

#ifndef _IVR_DATA_COLLECTION_H
#define _IVR_DATA_COLLECTION_H

#include "ivr_callinfo.h"
#include "fs_struct.h"

namespace ivr {
// ÿ�����������Ϣ
struct SkillCallData{
public:
    SkillCallData() : giveuptimes(0){}
public:
    uint32_t giveuptimes;  // �ü�������Ŷӷ�����
};

// ��ǰ�ĺ������ݺ�������
struct IvrCallData{
public:
    IvrCallData()
    : cur_inbound_num(0)
    , cur_accept_num(0)
    , trans_num(0)
    , total_inbound_num(0)
    , total_accept_num(0)
    , giveuptimes(0){}
public:
    uint32_t cur_inbound_num;  // ��ǰ��������
    uint32_t cur_accept_num;  // ��ǰ��Ծ�ĺ�����
    uint32_t trans_num;  //ת��ϯ��
    uint32_t total_inbound_num;  // �ܺ�������
    uint32_t total_accept_num;  // �ܻ�Ծ�ĺ�����
    uint32_t giveuptimes; // �Ŷӷ�����
    std::map<std::string, SkillCallData*> skill_call_data; //���ܺ�������
};

class IvrInboundCall {
public:
    enum {
        BEGIN,
        ACCEPT,
        INFLOW,
        CUSTOMERANSWER,
        AGENTALERTING,
        AGENTANSWER,
        CONNECTED,
        ROUTEBYSKILL,
        ROUTEBYAGENTID,
        ROUTEBYAGENTLIST,
        TRANSAGENT,
        EXITFLOW,
        CALLWITHOUTFOLW,
        THISPARTYHANGUP,
        OTHERPARTYHANGUP,
        UNKNOW
    }; // state

    IvrInboundCall() : _m_state(UNKNOW), _has_hangup(false), _exit_flow(false){}
    ~IvrInboundCall(){}
    int32_t update_route_skill_endtime(const std::string& skill_name);
    int32_t set_appdata(const std::string& appdata);
    int32_t set_state(int32_t state);//���õ�ǰ״̬
    int32_t get_state();//���õ�ǰ״̬
    void init_new_call(const ivr_session_id_t& sessionId, const string &callid, const string &caller, const string &callee); 
    void set_skill(const std::string& skill);
    std::string get_skill();
    std::string get_called();
    bool get_exit_flow();
    void set_agent_num(const string &agentnum);
    time_t get_begintime();
private:
    int32_t _m_state;
    bool _has_hangup;
    bool _exit_flow;
    IvrCallInfo _m_ivr_callinfo;
} ;

class IvrCallDataCollection
{
public:
    enum {
        GETBYPLAT,
        GETBYIVRNUM,
    };

    IvrCallDataCollection(){}
private:
    ~IvrCallDataCollection(){}
public:
    static IvrCallDataCollection& instance();

    // @breif ��ʼ������
    int32_t initialize(const char* path);
    
    // @breif �� ��ʼ������
    int32_t uninitialize();
    
    // ������д���ļ����ж����ں���
    void calldata_to_file_block_func();
    
    // @brief ������(��������) +1
    // @param caller �������
    // @param called ivrת����
    // @parma sessionid ,���ÿͨ�����
    // @return 0:success other:failed
    int32_t new_accept_call(const std::string& caller, const std::string& called
                    , const ivr_session_id_t& sessionId, const std::string& uuid);

    // @brief ������(���е���) +1
    // @param caller �������
    // @param called ivrת����
    // @parma uuit uuid,���ÿͨ�����
    // @return 0:success other:failed
    int32_t new_inbound_call(const std::string& caller, const std::string& called
                    , const std::string& uuid);
    
    // @brief ����״̬
    // @parma callid callid,���ÿͨ����
    // @parma state ״̬ 
    // @parma skill ���ܣ�Ĭ��Ϊ��
    // @return 0:success other:failed   
    int32_t set_appdata(const ivr_session_id_t& sessionId, const std::string& appdata);
    int32_t set_state(const ivr_session_id_t& sessionId, const int32_t state, const std::string& skill);
    
    // @brief �����¼�
    // @parma event,�¼�
    // @return 0:success other:failed
    int32_t process_event(struct fs_event& event);

    // @brief ����·���¼��ͳ�ʱ�¼�
    // @parma event,�¼�
    // @return 0:success other:failed
    int32_t process_event(ivr_base_event_t* event);
    
    // @breif ��ȡϵͳ��ǰ��������
    // @param calldata calldata��ַ
    // @param result ���صļ���������
    // @return 0:success other:failed  
    int32_t get_call_data(struct IvrCallData* p, std::string& result);
    
    // @breif ��ȡϵͳ��ǰ��������
    // @param result ���صļ���������
    // @return 0:success other:failed  
    int32_t get_inbound_by_plat(std::string& result);

    // @breif ��ȡϵͳ��ǰ��������
    // @param result ���صļ���������
    // @return 0:success other:failed  
    int32_t get_inbound_call_data(const int32_t type, const std::string& input, std::string& result);
    
    // @breif ��ȡϵͳ��ǰ��������
    // @param ivrnum ivr������
    // @param result ����ivr�����뷵������
    // @return 0:success other:failed  
    int32_t get_inbound_by_ivrnum(const std::string& ivrnum, std::string& result);

    // @brief ���ļ��л�ȡ��������
    int32_t get_data_from_file();

    // @brief ����������д���ļ���
    int32_t put_data_to_file();

    // @�ж��Ƿ��Ŷӷ���������Ǹ����������
    int32_t is_giveup_call(IvrInboundCall *call
                    , const uint64_t sessionid, const std::string& called);

    // @brief ��λ
    void clear_invaid_call();
    
    // @brief ��λ
    void reset();

private:
    bgcc::Mutex _m_locker;
    std::string _cached_file; // �����ļ�·��
    std::string _cached_date; // ��������
    struct IvrCallData _plat_call_data;//��ǰ�ĺ�������
    std::map<std::string, struct IvrCallData*> _ivrnum_call_data; // ��ivr������洢�ĵ����������
    std::map<ivr_session_id_t, IvrInboundCall*> _realtime_call;
    std::map<ivr_session_id_t, IvrInboundCall*> _exitflow_call;
    std::map<std::string, ivr_session_id_t> _first_uuid_map;
    std::map<std::string, ivr_session_id_t> _second_uuid_map;
    std::set<std::string> _refuse_call;
};

typedef std::map<std::string, struct SkillCallData*>::iterator IterSkill;
typedef std::map<std::string, struct IvrCallData*>::iterator IterIvrNum;
typedef std::map<ivr_session_id_t, IvrInboundCall*>::iterator IterCall;
typedef std::map<std::string, ivr_session_id_t>::iterator IterSession;
typedef std::set<std::string>::iterator IterStr;
}

#endif

