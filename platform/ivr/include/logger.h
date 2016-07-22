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


#pragma once

#include <string>           //��׼C++�⣬string��
#include <fstream>          //��׼C++�⣬IO����
#include <vector>           //��׼C++�⣬vector
#include <list>             //��׼C++�⣬list
#include <cstdio>           //��׼C++�⣬remove
#include <cstring>          //��׼C++�⣬strcmp
#include <cassert>

#include <sys/stat.h>       // mkdir, access
#include <dirent.h>         // DIR, dirent, opendir, readdir, closedir
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <stdarg.h>
#include <getopt.h>         // getopt_long

#include "bgcc.h"

#define SHOW_LONG "ll"
#define PATH_FLAG '/'

const char CONFIG_DIR[] = "conf/";
const char LOG_DIR[] = "log/";
const char EVNET_LOG_DIR[] = "log/eventlog/";
const char CALL_LOG_DIR[] = "log/calllog/";
const char BGCC_LOG_CONFIG_FILE[] = "conf/bgcc.cfg";

const char CONFIG_FILE[] = "acd.conf";
const char LOG_FILE[] = "acdlog";
const char EVNET_LOG_FILE[] = "acdeventlog";
const char CALL_LOG_FILE[] = "acdcalllog";
//const char DATA_CACHE_FILE[] = "log/acd_datacached.log";

typedef bgcc::Guard<bgcc::Mutex> SingleLocker;

// ��־���й㷺ʹ��
//const uint32_t STRING_LENGHT = 64;
const uint32_t LONG_STRING_LENGHT = 128;
const uint32_t TEXT_LENGHT = 1024;
const uint32_t LONG_TEXT_LENGHT = 2048;

//typedef char String[STRING_LENGHT];
typedef char LongString[LONG_STRING_LENGHT];
typedef char Text[TEXT_LENGHT];
typedef char LongText[LONG_TEXT_LENGHT];

// ��־����
enum Level {
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_DEBUG
};

enum EventTypeT {
    AGENT_EVENT,
    MEDIA_EVENT,
    ROUTE_EVENT,
    OTHER_EVENT
};

/*
struct SendEventT {
    int64_t m_event_id;
    acd_callback_proxy_ptr m_callback_proxy;
    EventTypeT m_event_type;
    acd::AgentEventT m_agent_event;
    acd::MediaEventT m_media_event;
    acd::RouteEventT m_route_event;
    acd::OtherEventT m_other_event;
};
*/

using namespace std;

class Logger {
private:
    uint32_t mLogCount;                 //��־�ļ���С
    uint32_t mFileCount;                    //��־�ļ�����
    string mDirName;                            //��־�ļ�·��
    string mFileName;                           //��־�ļ���
    string mVersion;                            //����汾
    bool mShowVersion;                          //�Ƿ���ʾ�汾
    Level mLevel;                               //��־����

    uint32_t mCurLogCount;                  //��ǰ��־����
    string mFuleFileName;                       //��־�ļ�ȫ��
    ofstream mOutStream;                        //IO��
    bgcc::Mutex mLocker;                        //ͬ����
private:
    bool CreatDirectory(const string& strPath);
    void GetTimeFileName(LongString strTime);
    void GetCurrentTimeString(LongString strTime);
    void InitLogger();
    void DeleteLog();
    void UnInitLogger();
    void ProcessLog();
    bool LogFileNotExisted();
    void GetLevelString(Level level, string& levelString);
public:
    Logger();
    ~Logger();
    void Initialize(uint32_t logCount, uint32_t fileCount, const char* dirName, const char* fileName,
                    const char* version, bool showVersion = true, Level level = LOG_LEVEL_NOTICE);
    void UnInitialize();
    void WriteLog(const char* pFormat, ...);
    void WriteLog(Level level, const char* pFormat, ...);
    void WriteLog(Level level, const char* file, uint32_t line, const char* fun, const char* pFormat,
                  ...);
    void SetTimeout();
    void SetLogLevel(Level level = LOG_LEVEL_NOTICE);
};
