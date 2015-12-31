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

#ifndef AGENTPROXY
#define AGENTPROXY

#ifdef _WIN32
#include <hash_map>

#else
#include <ext/hash_map>
using namespace __gnu_cxx;  //for hash_map
#endif


#include <string>
#include <vector>
#include <time.h>
#include <bgcc.h>


#include "acdcommon.h"
#include "acd.h"
#include "ap.h"

#include "ApCommon.h"
#include "ApClient.h"
#include "ApServer.h"
#include "ApService.h"
#include "Queue.h"
#include "ApCallbackThread.h"
#include "ApHeartBeatThread.h"
#include "ApListenThread.h"

using namespace ap;
using namespace acd;

class ApServer;
class ApClient;
class ApService;

class AgentProxy {
    typedef Guard<Mutex> ScopeGuard;

    /*���г�Ա�ӿ�����*/
public:
    /**
     * @brief Init:AgentProxy���ʼ�����������ڳ�ʼ����Ϣ���С�hash_map�������¼��̡߳���ȡlog��conf���õȹ���
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ05��33��
     */
    bool Init();

    /**
     * @brief SetMapAgentIdData:����һ���µ���������agentdata
     *
     * @param agentInfo
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 12ʱ59��09��
     */
    bool SetMapAgentIdData(AgentData* agentInfo);

    /**
     * @brief DelMapAgentIdData:���ָ�����ŵ�������Ϣ
     * @param agentId
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ07��04��
     */
    bool DelMapAgentIdData(const std::string& agentId);

    /**
     * @brief ClearMapAgentData:���hash_map���д洢��Ϣ
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ07��04��
     */
    bool ClearMapAgentData();

    /**
     * @brief ResetProxy:����ָ�����ŵ�������Ϣ
     *
     * @param agentId    ָ���Ĺ���
     * @param agentInfo  ���õ�������Ϣ
     *
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ07��30��
     */
    bool ResetProxy(const std::string& agentId, AgentData* agentInfo);

    /**
     * @brief GetAgentData ��ȡָ�����ŵ�������Ϣ
     *
     * @param agentId    ָ���Ĺ���
     * @param agentInfo  ��ȡ��������Ϣ
     *
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��29�� 17ʱ23��25��
     */
    bool GetAgentData(const std::string& agentId, AgentData& agentInfo);

    /**
     * @brief GetAgentData ���ݹ��Ż�ȡhandleֵ
     *
     * @param agentId    ָ���Ĺ���
     * @param agentInfo  �����������ȡ��handleֵ
     *
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��29�� 17ʱ23��25��
     */
    bool GetAgentHandleById(const std::string agentId, int64_t& handle);

    /**
     * @brief GetAgentData ���ݹ��Ų鿴��ϯ��Ϣ�Ƿ����
     * @param agentId    ָ���Ĺ���
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��29�� 17ʱ23��25��
     */
    bool IsAgentHasExist(const std::string agentId);

    /**
     * @brief GetAgentData ������ϯ��Ϣ
     * @param agentinfo    ָ���ĸ��µ���ϯ��Ϣ
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2012��06��29�� 17ʱ23��25��
     */
    bool UpdateAgentData(AgentData& agentinfo);

    /**
     * @brief SetAgentOldProxyNull ��oldproxy�ÿ�
     * @param agentId          ָ���ĸ��µ���ϯ����
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool SetAgentOldProxyNull(const std::string agentId, int64_t handle);

    /**
     * @brief SetAgentHandleById    ����ָ������ϯ��handleֵ
     * @param agentId               ָ������ϯ����
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool SetAgentHandleById(const std::string agentId, int64_t handle);

    /**
     * @brief SetAgentFlag          ����ָ����ϯ�Ƿ�ɾ���ı�־λ
     * @param agentId               ָ������ϯ����
     * @param flag                  ��ʾ�Ƿ�signout
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SetAgentFlag(const std::string agentId, int32_t flag);

    /**
     * @brief SendCallEvent         ���ͺ����¼�������
     * @param acdCallEvent          ��ϯ�����¼�
     * @param threadNo              �����߳�ID
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SendCallEvent(const AgentEventT& acdCallEvent, int32_t threadNo);

    /**
     * @brief SendOtherEvent        ���������¼�������
     * @param acdCallEvent          ��ϯ�����¼�
     * @param threadNo              �����߳�ID
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SendOtherEvent(const OtherEventT& acdOtherEvent, int32_t threadNo);

    /**
     * @brief SendOtherEvent        ����·���¼�������
     * @param acdCallEvent          ��ϯ�����¼�
     * @param threadNo              �����߳�ID
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SendRouteEvent(const RouteEventT& acdRouteEvent, int32_t threadNo);

    /**
     * @brief SendOtherEvent        ����ý���¼�������
     * @param acdCallEvent          ��ϯ�����¼�
     * @param threadNo              �����߳�ID
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SendMediaEvent(const MediaEventT& acdMediaEvent, int32_t threadNo);

    /**
     * @brief GetHeartBeatTimeOut      ��ȡ�����¼����
     * @return ����ʱ��
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    int32_t GetHeartBeatTimeOut();

    /**
     * @brief AddAcdCallEvent :д�����¼�������
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ08��14��
     */
    bool AddAcdCallEvent(AgentEventT& event);

    /**
     * @brief AddAcdMediaEvent :дý���¼�������
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ08��27��
     */
    bool AddAcdMediaEvent(MediaEventT& event);

    /**
     * @brief AddAcdRouteEvent :д·���¼�������
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ08��37��
     */
    bool AddAcdRouteEvent(RouteEventT& event);

    /**
     * @brief AddAcdOtherEvent :д�����¼�������
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ08��47��
     */
    bool AddAcdOtherEvent(OtherEventT& event);

    /**
     * @brief GetAcdCallEvent :�Ӷ��ж�ȡ�����¼�
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ08��59��
     */
    bool GetAcdCallEvent(AgentEventT& event, const int32_t threadNo);

    /**
     * @brief GetAcdMediaEvent �Ӷ��ж�ȡý���¼�
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ09��11��
     */
    bool GetAcdMediaEvent(MediaEventT& event, const int32_t threadNo);

    /**
     * @brief GetAcdRouteEvent :�Ӷ��ж�ȡ·���¼�
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ09��22��
     */
    bool GetAcdRouteEvent(RouteEventT& event, const int32_t threadNo);

    /**
     * @brief GetAcdOtherEvent :�Ӷ��ж�ȡ�����¼�
     *
     * @param event
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ09��36��
     */
    bool GetAcdOtherEvent(OtherEventT& event, const int32_t threadNo);

    /**
     * @brief CreateAcdCBThread :��������ص���Ϣ�߳�
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ09��54��
     */
    bool CreateAcdCBThread();

    /**
     * @brief CreateApListenThread     ��������AP����Ĵ����߳�
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool CreateApListenThread();

    /**
     * @brief IsQueueEmpty :�ж��¼������Ƿ�Ϊ��
     *
     * @param threadNo
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ10��03��
     */
    bool IsQueueEmpty(const int32_t& threadNo);

    /**
     * @brief HeartBeat :���ap��ACD֮�������״̬
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ10��20��
     */
    void HeartBeat();

    /**
     * @brief Backup:�������������쳣ʱ������������client���ñ�����
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ10��45��
     */
    bool Backup();

    /**
     * @brief Instance :����ȫ��Ψһ��AgentProxyʵ��
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ14��28��
     */
    static AgentProxy* Instance() {
        static AgentProxy agentproxy;
        return &agentproxy;
    }

    /**
     * @brief Destroy ������Դ
     * @see
     * @note
     * @date    2012��06��29�� 13ʱ18��58��
     */
    void Destroy();
    /**
     * @brief CreateCheckHBThread :�������������Ϣ�߳�
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 13ʱ09��54��
     */
    bool CreateCheckHBThread();

    /**
     * @brief IsNeedApThreadShutdown :ɱ���߳�
     *
     * @return
     * @see
     * @note
     * @date    2012��07��01�� 15ʱ26��48��
     */
    bool IsNeedApThreadShutdown();

    /**
     * @brief CreateClient            ��������ACD�Ŀͻ���ʵ��
     * @param Ip                      ACD��IP��ַ
     * @param Port                    ACD�Ķ˿�
     * @param connectNum              ����ACD��������
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool CreateClient(std::string Ip, int32_t Port, int32_t connectNum);

    /**
     * @brief ClearHaveSendMsgNum ���ת����Ϣ��
     *
     * @return
     * @see
     * @note
     * @date    2012��07��02�� 14ʱ08��36��
     */
    void ClearHaveSendMsgNum();

    /**
     * @brief StartApListen            ��ʼ����AP�ķ���
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool StartApListen();

    /**
     * @brief ClearTimeOutAgentInfo       ����Ѿ�signout����������Ϣ����ϯ����ֹ��ʧsignout�¼�
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void ClearTimeOutAgentInfo(std::string agentId);

    void ChangeAgentListIp(AgentInfoListT& agentInfoList);
    void ChangeAgentIp(AgentInfoT& agentInfo);


    /*�ν�server��client�Ĺ����ӿ�*/
public:

    AcdResultT SignIn(const std::string& agentId,
                      const std::string& agentDn, const std::string& agentPwd,
                      const StatusChangeT& statusChangetype, bool autoAnswer,
                      bool fcSignin, const std::string& skills,
                      int64_t& handle,
                      const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SignOut(int64_t handle, const std::string& agentId,
                       const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SetAgentStatus(int64_t handle, const std::string& agentId,
                              const AgentStatusT& agentStatus, const std::string& restReason,
                              const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetAgentStatus(int64_t handle, const std::string& agentId, AgentStatusT& agentStatus,
                              const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ResetStatuschangetype(int64_t handle, const std::string& agentId,
                                     const StatusChangeT& statusChangetype, const std::map<std::string, std::string> ctx,
                                     uint64_t time_rcv);
    AcdResultT ResetAutoAnswer(int64_t handle, const std::string& agentId, bool autoAnswer,
                               const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ResetSkill(int64_t handle, const std::string& agentId, const std::string& skill,
                          const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Reset(int64_t handle, const std::string& agentId,
                     const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT OutboundCall(int64_t handle, const std::string& agentId, const std::string& callerId,
                            const std::string& destId, const std::string& forCallerDispNum, const std::string& forCalleeDispNum,
                            int32_t timeout, const CallModeT& callMode, const CallTypeT& callType,
                            const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT AnswerCall(int64_t handle, const std::string& agentId,
                          const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ReleaseCall(int64_t handle, const std::string& agentId,
                           const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Hold(int64_t handle, const std::string& agentId,
                    const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT RetrieveHeld(int64_t handle, const std::string& agentId,
                            const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Consult(int64_t handle, const std::string& agentId, const std::string& callerId,
                       const std::string& destId, const std::string& forCallerDispNum, const std::string& forCalleeDispNum,
                       const CallTypeT& callType, const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ConsultReconnect(int64_t handle, const std::string& agentId,
                                const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ConsultTransfer(int64_t handle, const std::string& agentId,
                               const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SingleStepTransfer(int64_t handle, const std::string& agentId,
                                  const std::string& callerId, const std::string& destId, const std::string& forCallerDispNum,
                                  const std::string& forCalleeDispNum, bool isPassthrough, const CallTypeT& transferType,
                                  const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ConsultConference(int64_t handle, const std::string& agentId,
                                 const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ConferenceJoin(int64_t handle, const std::string& agentId, const std::string& callerId,
                              const std::string& destAgentId, const std::string& forCallerDispNum,
                              const std::string& forCalleeDispNum, const ConferenceT& conferenceType,
                              const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SetAssociateData(int64_t handle, const std::string& agentId, const std::string& key,
                                const std::string& value, const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetAssociateData(int64_t handle, const std::string& agentId, const std::string& key,
                                std::string& value, const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ForceSignIn(int64_t handle,
                           const std::string& agentId, const std::string& destAgentId,
                           const std::string& agentDn, const std::string& agentPwd,
                           const StatusChangeT& statusChangetype, bool autoAnswer, bool fcSignin,
                           const std::string& skills, const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ForceSignOut(int64_t handle, const std::string& agentId, const std::string& destAgentId,
                            const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ForceSetAgentStatus(int64_t handle, const std::string& agentId,
                                   const std::string& destAgentId, const AgentStatusT& agentStatus,
                                   const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Listen(int64_t handle, const std::string& agentId, const std::string& callerId,
                      const std::string& destAgentId, const std::string& forCallerDispNum,
                      const std::string& forCalleeDispNum, const std::map<std::string, std::string> ctx,
                      uint64_t time_rcv);
    AcdResultT StopListen(int64_t handle, const std::string& agentId, const std::string& destAgentId,
                          const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Insert(int64_t handle, const std::string& agentId, const std::string& callerId,
                      const std::string& destAgentId, const std::string& forCallerDispNum,
                      const std::string& forCalleeDispNum, const std::map<std::string, std::string> ctx,
                      uint64_t time_rcv);
    AcdResultT StopInsert(int64_t handle, const std::string& agentId, const std::string& destAgentId,
                          const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SwitchInsertorListen(int64_t handle, const std::string& agentId,
                                    const std::string& callerId, const std::string& destAgentId, const std::string& forCallerDispNum,
                                    const std::string& forCalleeDispNum, const std::map<std::string, std::string> ctx,
                                    uint64_t time_rcv);
    AcdResultT Break(int64_t handle, const std::string& agentId, const std::string& destAgentId,
                     const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SendDTMF(int64_t handle, const std::string& agentId, const std::string& digitals,
                        const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT StartPlay(int64_t handle, const std::string& agentId, const std::string& filename,
                         const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT StopPlay(int64_t handle, const std::string& agentId,
                        const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT MuteOn(int64_t handle, const std::string& agentId,
                      const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT MuteOff(int64_t handle, const std::string& agentId,
                       const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetAgentDetailByAgentID(int64_t handle, const std::string& agentId,
                                       const std::string& destAgentId, AgentInfoT& agentInfo, const std::map<std::string, std::string> ctx,
                                       uint64_t time_rcv);
    AcdResultT GetAgentDetailByAgentDN(int64_t handle, const std::string& agentId,
                                       const std::string& destAgentDn, AgentInfoT& agentInfo, const std::map<std::string, std::string> ctx,
                                       uint64_t time_rcv);
    AcdResultT GetAgentDetailsByAgentIDs(int64_t handle, const std::string& agentId,
                                         const StringListT& destAgentIdList, AgentInfoListT& agentInfoList,
                                         const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetAgentDetailsByAgentDNs(int64_t handle, const std::string& agentId,
                                         const StringListT& destAgentDnList, AgentInfoListT& agentInfoList,
                                         const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetRecordFileName(int64_t handle, const std::string& agentId, std::string& filename,
                                 const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetRestReason(int64_t handle, const std::string& agentId, StringListT& restReason,
                             const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetSkill(int64_t handle, const std::string& agentId, StringListT& skill,
                        const std::map<std::string, std::string>& ctx, uint64_t time_rcv);
    AcdResultT GetAgentNumberBySkill(int64_t handle, const std::string& agentId,
                                     const std::string& skill, int32_t& agentNum, const std::map<std::string, std::string> ctx,
                                     uint64_t time_rcv);
    AcdResultT GetAgentDetailsBySkill(int64_t handle, const std::string& agentId,
                                      const std::string& skill, int32_t& agentNum, AgentInfoListT& agentInfoList,
                                      const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetSkillWaitingNumber(int64_t handle, const std::string& agentId,
                                     const std::string& skill, int32_t& queueNum, int32_t& freeNum,
                                     const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetSkillWaitingCallInfo(int64_t handle, const std::string& agentId,
                                       const std::string& skill, int32_t& queueNum, QueueInfoListT& queueInfoList,
                                       const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetPrivateQueue(int64_t handle, const std::string& agentId,
                               const std::string& destAgentId, int32_t& queueNum, QueueInfoListT& queueInfoList,
                               const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT GetSysInfo(int64_t handle, const std::string& agentId, SysInfoT& sysInfo,
                          const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT SynchronizeCTITime(int64_t handle, const std::string& agentId, int64_t& timestamp,
                                  const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT Heartbeat(int64_t handle, const std::string& agentId,
                         const std::map<std::string, std::string> ctx, uint64_t time_rcv);
    AcdResultT ResetConfig(const std::string& password, const std::map<std::string, std::string>& ctx,
                           uint64_t time_rcv);

    AcdResultT JumptheQueue(int64_t handle, const std::string& agentId, int64_t requestId,
                            const std::map<std::string, std::string>& ctx, uint64_t time_rcv);
    bool ReLoadConfig();
    bool GetAgents(ApAgentInfoListT& agentInfoList);
    AcdResultT GetCallInfo(int64_t handle, const std::string& agentId, 
                            int32_t type, const std::string& input, std::string& result,
                            const std::map<std::string, std::string>& ctx);


    /*˽�г�Ա�ӿ�����*/
private:
    /**
    * @brief readConf :��ȡ�����ļ�����
    *
    * @return
    * @see
    * @note
    * @date    2013��03��25��
    */
    bool readConf(const char* conf_filepath);

    /**
     * @brief getApCommonConf       ��ȡap��ͨ������
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool getApCommonConf(const char* conf_filepath);

    /**
     * @brief getAcdConf       ��ȡap�Ĺ���acd��������Ϣ����
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool getAcdConf(const char* conf_filepath);

    /**
     * @brief getBgccConf       ��ȡap�Ĺ���bgcc���������
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool getBgccConf(const char* conf_filepath);
    /**
     * @brief Init_log :��ʼ����־ģ��
     *
     * @return
     * @see
     * @note
     * @date    2012��06��28�� 19ʱ55��35��
     */
    bool Init_log();

    /**
     * @brief SetApThreadShutdown      ���ùر��̱߳�־λ
     * @return
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void SetApThreadShutdown();

    /**
     * @brief GetCtiService ��ȡ��ǰָ��
     *
     * @return
     * @see
     * @note
     * @date    2012��07��09�� 15ʱ09��10��
     */
    APCONNACDSTATE GetCtiService();

    /**
     * @brief SetCtiService ���õ�ǰAPָ��
     *
     * @param State
     *
     * @return
     * @see
     * @note
     * @date    2012��07��09�� 15ʱ09��24��
     */
    void SetCtiService(APCONNACDSTATE State);

    /**
     * @brief SetApClient       ������������ACD�Ŀͻ���ʵ��
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool SetApClient(ApClient* client);

    /**
     * @brief AddHeartBeatCount       �����쳣����+1
     * @return
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void AddHeartBeatCount();

    /**
     * @brief ClearHeartBeatCount       ��������쳣����
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void ClearHeartBeatCount();

    /**
     * @brief GetHeartBeatCount       ��ȡ�����쳣����
     * @return  �쳣��������
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    int32_t GetHeartBeatCount();

    /**
     * @brief GetValidAcd       ��ȡ����ACD
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool GetValidAcd();

    /**
     * @brief GetValidAcd       ��������ACD
     * @return  �ɹ���true;ʧ�ܣ�false
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    bool TryConnAcd(std::string strIp, int32_t Port, APCONNACDSTATE state);

    /**
     * @brief ReSignIn       ����ACD�������ϯ��ǩ
     * @return
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    void ReSignIn(APCONNACDSTATE state);

    /**
     * @brief getProxyName       ��ȡ�ͻ������ӵ�proxyname
     * @return  ����ID
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    std::string getProxyName(const std::map<std::string, std::string>& ctx);

    /**
     * @brief getProxyIP       ��ȡ�ͻ���IP��ַ
     * @return
     * @see
     * @note
     * @date    2013��03��26�� 21ʱ23��25��
     */
    std::string getProxyIP(const std::map<std::string, std::string>& ctx);

    /*˽�г�Ա��������*/
private:

    int32_t              _heartBeatCount;           /** ��ǰ������*/
    int32_t              _apClientHandle;           /** ��ǰproxy�����Ӿ��*/
    int32_t              _apListenPort;
    int32_t              _ACDPort;                  /** ACD�Ķ˿ں�*/
    int32_t              _apListenPortOwn;
    int32_t              _apHeartBeatTimeout;
    int32_t              _apHeartBeatMaxTrytimes;
    int32_t              _apWarnSendmsgTimeout;
    int32_t              _apServerThreadPoolNum;    /**bgcc������߳���**/
    int32_t              _apClientConnNum;          /**bgcc������������**/
    int32_t              _CallbackThreadNum;        /**���ջص��¼��߳���**/
    int32_t              _AcdMainPort;
    int32_t              _AcdBackupPort;
    int32_t              _AgentTimeOut;             /**��ʱ��ʱ�䣨����ɾ��δ�յ�signout�¼�����ϯ��**/

    ApServer*            _pApServer;                /** ApServer����ָ��*/
    ApService*           _pApService;               /** ApService����ָ��*/
    ApClient*            _pApClient;                /** ApClient����ָ��*/
    ApEventQ*            _ApEventQ;                 /** �¼�����ָ�룬��ʼ��ʱ��ֵ*/
    Thread*
    _apHBCheckThread;          /** �����������ָ�����飨�̳�BGCC��,�������ACD����*/
    Thread*              _apListenThread;           /** AP�����ӿ��߳�ָ��*/

    std::string          _apHost;
    std::string          _strMainACDIP;             /** ��ACD��IP��ַ*/
    std::string          _strBackACDIP;             /** ��ACD��IP��ַ*/
    APCONNACDSTATE       _ctiService;               /** ��ʾ��ǰ������һ̨ACD*/
    MapAgentIdDataType   _mapAgentIdData;           /** ά��agentbar�������ݵ�hash_map*/
    std::vector<Thread*> _apCallbackThreadVec;
    ServiceManager       _serviceManager;
    volatile bool        _shutdown;

    Mutex                _mapMutex;                 /*map��*/
    Mutex                _threadShutdownMutex;
    Mutex                _ctiServiceMutex ;
    Mutex                _HBCountMutex;
    Mutex                _ApClientMutex;            /** proxy��*/
private:
    AgentProxy() {}
    ~AgentProxy() {}

};

#endif // AGENTPROXY

