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


#include <getopt.h>

#include "lock.h"
#include "common.h"
#include "ini_file.h"
#include "node_base.h"
#include "thread_pool.h"
#include "conf_manager.h"
#include "inbound_chan_thread.h"
#include "autorun_chan_thread.h"
#include "arithmetic_operate.h"
#include "calldata_manager.h"
#include "cpu_manager.h"
#include "autorun_manager.h"

#include "ivr_timer.h"
#include "ivrapi.h"
#include "ivr_data_collection.h"

#include "ivr_callinfo.h"


#ifndef SCMPF_MODULE_VERSION
#define SCMPF_MODULE_VERSION "undefined"
#endif

using ivr::IvrCallDataCollection;
static const char* cut_path(const char* in) {
    const char* p, *ret = in;
    char delims[] = "/\\";
    char* i;

    for (i = delims; *i; i++) {
        p = in;

        while ((p = strchr(p, *i)) != 0) {
            ret = ++p;
        }
    }

    return ret;
}

void esl_log_ivr(const char* file, const char* func, int line, int level, const char* fmt, ...) {
    if (0 == g_output_esllog) {
        // disable esl log file
        return;
    }

    const char* fp;
    char* data;
    va_list ap;
    int ret;

    fp = cut_path(file);

    va_start(ap, fmt);

    ret = vasprintf(&data, fmt, ap);

    if (ret != -1) {
        //wangtingdel com_writelog("esl", "%s:%d %s() %s",fp,line,func,data);
        BGCC_WARN("esl", "%s:%d %s() %s", fp, line, func, data);
        /*
        if(level<=3){//ERROR
            IVR_FATAL("%s:%d %s() %s", fp, line, func, data);
        }
        else if(4==level){//WARING
            IVR_WARN("%s:%d %s() %s", fp, line, func, data);
        }
        else if(5==level){//NOTICE
            IVR_NOTICE("%s:%d %s() %s", fp, line, func, data);
        }
        else if(6==level){//trace
            IVR_TRACE("%s:%d %s() %s", fp, line, func, data);
        }
        else{//DEBUG
            IVR_DEBUG("%s:%d %s() %s", fp, line, func, data);
        }
        */
        free(data);
    }

    va_end(ap);
}

//IVR���������� �ص�����
void inbound_callback(esl_socket_t server_sock,
                      esl_socket_t client_sock, struct sockaddr_in* addr) {
    esl_handle_t handle;
    string callid;

    bzero(&handle, sizeof(esl_handle_t));

    if (ESL_SUCCESS != esl_attach_handle(&handle, client_sock, addr)) {
        IVR_WARN("attach handle failed");
        esl_disconnect(&handle);
        return;
    }

    if (!handle.connected) {
        IVR_WARN("get callid failed");
        esl_disconnect(&handle);
        return;
    }

    callid = esl_event_get_header(handle.info_event, "caller-unique-id");
    IVR_DEBUG("CALLID = %s", callid.c_str());
    // load_balance_mod ������caller-unique-id == "xyz" ����cpuռ���ʺ�ͨ������
    // ivr_monitor_mod ������caller-unique-id == "opq" ����cpuռ���ʺ�ͨ������
    int32_t idtype = 0;

    if (strcasecmp(callid.c_str(), "xyz") == 0) {
        idtype = 1;
    } else if (strcasecmp(callid.c_str(), "opq") == 0) {
        idtype = 2;
    }

    //
    if (idtype != 0) {
        uint32_t inbound_chan_num_cp = 0;
        uint32_t cpu = CpuManager::get_instance()->get_cpu_occupy();
        std::string err;
        char resp[20] = {0};
        bzero(resp, 20);

        {
            rw_lock_t lock(g_inbound_chan_rwlock);

            if (lock.locked()) {
                inbound_chan_num_cp = g_inbound_chan_num;
            } else {
                err = "lock inbound num failed";
            }
        }

        if (err.length()) {
            IVR_WARN("%s", err.c_str());
        }

        // cann't change format of this string, because monitor and ivrloadbanlance use this string
        snprintf(resp, 20, "cpu=%02u,chan=%05u", cpu, inbound_chan_num_cp);
        esl_send(&handle, resp);

        IVR_DEBUG("ivr-%s-resp = %s", (idtype == 1 ? "loadbalance" : "monitor"), resp);
    }

    esl_disconnect(&handle);
    return;
}

// Event Socket �̺߳���
void* esl_listen_thread_func(void* arg) {
    if (ESL_SUCCESS != esl_listen(g_server_ip.c_str(), g_server_port, inbound_callback)) {
        IVR_FATAL("esl_listen return fail");
        //raise(SIGKILL);
        _exit(0);
        //exit (0); //ֱ�ӽ�������
    }

    return NULL;
}

//���������ɵ��ļ� �̺߳���
void* calldata_to_file_thread_func(void* arg) {
    CallDataManager* calldata_manager = CallDataManager::get_instance();
    calldata_manager->calldata_to_file_block_func();
    return NULL;
}

//����CPUռ���� �̺߳���
void* calc_cpu_occupy_thread_func(void* arg) {
    CpuManager* cpu_manager = CpuManager::get_instance();
    cpu_manager->calculate_cpu_occupy_block_func();
    return NULL;
}

void* calldata_collection_thread_func(void* arg) {
    IvrCallDataCollection::instance().calldata_to_file_block_func();
    return NULL;
}

//�������ʱ��Ϣ �̺߳���
void* disp_runinfo_thread_func(void* arg) {
    CpuManager* cpu_manager = CpuManager::get_instance();
    uint32_t cpu = 0;
    uint32_t inbound_chan = 0;
    uint32_t autorun_chan = 0;
    uint32_t autodial_chan = 0;

    while (!g_stopped) {
        cpu = cpu_manager->get_cpu_occupy();

        {
            rw_lock_t lock1(g_inbound_chan_rwlock);

            if (lock1.locked()) {
                inbound_chan = g_inbound_chan_num;
            }
        }

        {
            rw_lock_t lock2(g_autorun_chan_rwlock);

            if (lock2.locked()) {
                autorun_chan = g_autorun_chan_num;
            }
        }

        {
            rw_lock_t lock3(g_autodial_chan_rwlock);

            if (lock3.locked()) {
                autodial_chan = g_autodial_chan_num;
            }
        }

        IVR_TRACE("cpu:%u\tinbound-channel:%u\tautorun-channel:%u\tautodial-channel:%u",
                  cpu, inbound_chan, autorun_chan, autodial_chan);
        ivr_tools_t::safe_sleeps(6);
    }

    return NULL;
}

//���������̿��� �̺߳���
void* autorun_script_manager_thread_func(void* arg) {
    AutoRunManager* autorun_manager = AutoRunManager::get_instance();
    uint32_t autorun_script_num = g_autorun_conf.autorun_script_vec.size();

    for (uint32_t i = 0; i < autorun_script_num; i++) {
        autorun_manager->begin(i);
    }

    return NULL;
}

//�źŴ���
void sig_handler(int32_t signum) {
    if (signum == SIGINT) {
        IVR_TRACE("����ֹͣ�߳��ź�");
        g_stopped = 1;

        //Ϊ��ֹͣ���е��̣߳���ÿ���߳�һ�������work�����̺߳�����ִ��
        ThreadPool* threadpool = ThreadPool::get_instance();
        uint32_t max_chan_num = threadpool->get_max_thread_num();

        for (uint32_t i = 0; i < max_chan_num; i++) {
            inbound_script_t* basescript = new inbound_script_t;

            InboundChanThread* inbound_thread = new InboundChanThread(basescript);
            ThreadPool* threadpool = ThreadPool::get_instance();
            threadpool->assign_work(inbound_thread);
        }

        ivr_tools_t::safe_sleeps(1);
        sem_lock_t sem_lock(&g_stop_sem);
        sem_lock.post();
    }
}

bool init_log() {
    std::string bgcclogpath = LOG_PATH;
    bgcclogpath += "bgcc.cfg";
    bgcc::log_open(bgcclogpath.c_str());
    IVR_TRACE("bgcc::log_open success!");

    return true;
}

void *log_thread(void *) {
    while (true) {
        bgcc::TimeUtil::safe_sleep_s(g_log_timeout);
        ivr::IvrCallInfo::_m_logger.SetTimeout();
    }
    return NULL;
}

extern std::auto_ptr<ConfManager> g_conf_manager;

int32_t main(int32_t argc, char* argv[]) {
    char const* const short_options = "vh";
    struct option long_options[] = {
        { "version", 0, NULL, 'v'},
        { "help", 0, NULL, 'h'},
    };

    // is version command
    char ch = 0;

    while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (ch) {
        case 'v':
            std::cout << "version: " << SCMPF_MODULE_VERSION << std::endl;
            return 0;

        case 'h':
            std::cout << "version " << SCMPF_MODULE_VERSION << "\n./{IVRSERVER}& run ivrserver" << std::endl;
            return 0;

        default:
            std::cerr << "ivrserver --version";
            return 0;
        }
    }

    sem_init(&g_stop_sem, 0, 0);

    signal(SIGPIPE, SIG_IGN);
    //signal(SIGINT, sig_handler);

    //esl_global_set_default_logger(0);

    if (!init_log()) {
        return FAILURE;
    }

    esl_global_set_logger(esl_log_ivr);

    g_conf_manager.reset(new ConfManager("conf/system.conf", "conf/inbound.conf", "conf/autorun.conf"));

    if (g_conf_manager->load_max_chan_num() != SUCCESS) {
        return FAILURE;
    }

    //��ȡ���ͨ������
    uint32_t max_chan_num = g_inbound_conf.max_inbound_chan_num
                            + g_autorun_conf.max_autorun_chan_num;

    if (max_chan_num > MAX_THREAD_NUM) {
        IVR_FATAL("max_inbound_chan_num + max_autorun_chan_num > MAX_USER_THREAD");
        return FAILURE;
    }

    //��ʼ���̳߳�
    ThreadPool* threadpool = ThreadPool::get_instance();
    threadpool->initialize_threads(max_chan_num);

    ivr_tools_t::safe_sleeps(1);//���̳߳��ȳ�ʼ�� �ȴ�һ�� �����߳������־̫��

    IVR_TRACE("create thread_pool(%u) success", max_chan_num);

    //���������ļ�
    if (g_conf_manager->load_conf() != SUCCESS) {
        return FAILURE;
    }

    //modified by zzy
    pthread_t tid = 0;
    if (pthread_create(&tid, NULL, log_thread, NULL) != 0) {
        IVR_FATAL("create logthread thread failure");
        return FAILURE;
    }

    std::string filename = "calllog-" + g_server_ip;
    ivr::IvrCallInfo::_m_logger.Initialize(
            g_log_count, 
            g_log_filecount, 
            "log/calllog", 
            filename.c_str(), 
            "3", 
            true);
    
    pthread_t thread_h = 0;

    //����event socket�����߳�
    if (pthread_create(&thread_h, NULL, esl_listen_thread_func, NULL) != 0) {
        IVR_FATAL("create esl listen thread failure");
        return FAILURE;
    }


    //���������ļ������߳�
    if (pthread_create(&thread_h, NULL, calldata_to_file_thread_func, NULL) != 0) {
        IVR_FATAL("create generate calldata thread failure");
        return FAILURE;
    }

    //��������CPUռ�����߳�
    if (pthread_create(&thread_h, NULL, calc_cpu_occupy_thread_func, NULL) != 0) {
        IVR_FATAL("create calculate cpu occupy thread failure");
        return FAILURE;
    }

    //��������ʱ��Ϣ����߳�
    if (pthread_create(&thread_h, NULL, disp_runinfo_thread_func, NULL) != 0) {
        IVR_FATAL("create display runinfo thread failure");
        return FAILURE;
    }

    //��ʼ�����ݻ��棬 ��������ʱ�������ݻ����߳�
    if (IvrCallDataCollection::instance().initialize(DATA_CACHE_FILE) != 0) {
        IVR_FATAL("init ivr call data collection failed");
        return FAILURE;
    }
    if (pthread_create(&thread_h, NULL, calldata_collection_thread_func, NULL) != 0) {
        IVR_FATAL("create ivr call data thread failed");
        return FAILURE;
    }

    //���ͨ���Ŀ��Ʋ���
    uint32_t num = g_autorun_conf.autorun_script_vec.size();

    for (uint32_t i = 0; i < num; i++) {
        AutoRunManager::get_instance()->begin(i);
    }

    // wait for reload
    ivr_reload_server svr(g_reload_port);

    if (g_reload_port != 0) {
        svr.start();
    }

#if IVR_ENABLE_DEBUGER
    extern void start_debug_server(int32_t);
    start_debug_server(g_server_port - 1);
#endif

    sem_lock_t stop_sem(&g_stop_sem);
    stop_sem.wait();

    return SUCCESS;
}

