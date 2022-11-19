#ifndef _TASK_MANAGER_MODULE_H_
#define _TASK_MANAGER_MODULE_H_

#include <thread>
#include <mutex>

#include <string>
#include <cstdlib>
#include <cstdio>

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#include <boost/algorithm/algorithm.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
namespace ba = boost::algorithm;
namespace bf = boost::filesystem;

using namespace std;

class CALLEngineManager
{
public:
    CALLEngineManager();
    ~CALLEngineManager();

    int NextTaskId();

public:
    char (*waveArray)[2048];
    char (*gramArray)[2048];
    char (*idArray)[1024];
    char *m_ExtraSilence;
    int m_ExtraSilLen;
    int ttTaskNum;
    int curTaskIdx;

private:
    // for thread safe
    std::mutex *m_mutex_;
};

class AgentParam
{
public:
    int m_AgentID;
    char m_OutFile[1024];
    CALLEngineManager *taskMan;
};

struct WavHeader
{
    char riff[4]; // "riff"
    unsigned int size;
    char wav[4]; // "WAVE"
    char fmt[4]; // "fmt "
    unsigned int fmt_size;
    uint16_t format;
    uint16_t channels;
    unsigned int sample_rate;
    unsigned int bytes_per_second;
    uint16_t block_size;
    uint16_t bit;
    char data[4]; // "data"
    unsigned int data_size;
};
#endif