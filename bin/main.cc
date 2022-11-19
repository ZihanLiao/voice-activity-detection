#include <thread>
#include <mutex>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cstdint>
#include <string>
#include <sstream>

#include <fstream>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <queue>
#include <string>
#include <ctime>
#include <iterator>

#include "utils/wav.h"
#include "utils/logging.h"
#include "bin/task_manager.h"
#include "model/torch_vad_model.h"
#include "model/vad_informer.h"
#include "model/vad_api.h"
#include "model/vad_state.h"
#include "torch/torch.h"
#include "torch/script.h"

char SystemResourceDir[1024];
std::mutex common_data_mutex;

using namespace std;
#define NormalStrLen 512
#define MidianFilteringQueueLen 5
#define MidianFilteringArrayLen 3
static char single_wave_file[NormalStrLen] = {'\0'};
static char write_wave_file_char[NormalStrLen] = {'\0'};
static char wave_file_list[NormalStrLen] = {'\0'};
static float sampleRate = 16000.0f;
static int ThreadNum = 1;

int ParseOptions(int argc, char *argv[]);
bool ProcessSingleStream(const char *file_name, const char *write_file_name);

int ParseOptions(int argc, char *argv[])
{

    if (argc == 1)
    {

        return -1;
    }
    for (int i = 0; i < argc; i++)
    {
        if (strcmp("--wave-file", argv[i]) == 0)
        {
            single_wave_file[0] = '\0';
            strcpy(single_wave_file, argv[++i]);
        }
        if (strcmp("--write-file", argv[i]) == 0)
        {
            write_wave_file_char[0] = '\0';
            strcpy(write_wave_file_char, argv[++i]);
        }
        if (strcmp("--wave-list", argv[i]) == 0)
        {
            wave_file_list[0] = '\0';
            strcpy(wave_file_list, argv[++i]);
        }
        if (strcmp("--sample-rate", argv[i]) == 0)
        {
            sampleRate = std::atof(argv[++i]);
        }
        if (strcmp("--thread-num", argv[i]) == 0)
        {
            ThreadNum = std::atoi(argv[++i]);
        }
    }

    return 0;
}

bool ProcessSingleStream(const char *file_name, const char *write_file_name_char)
{
    // open wave stream source
    std::string file_name_string = file_name;
    google::InitGoogleLogging("vadlog");
    google::SetLogDestination(google::INFO, "/Users/marlowe/workspace/silero-vad/build/bin/vad.log");
    // FLAGS_logtostderr = 1;
    // FLAGS_stderrthreshold = google::INFO;
    // google::SetStderrLogging(google::INFO);
    vad::WavReader wav_reader(file_name);
    int sample_rate = wav_reader.sample_rate();
    int wave_len = wav_reader.num_samples();
    int bits_per_sample = wav_reader.bits_per_sample();
    const float* wave_data = wav_reader.data();
    float threshold = 0.5, neg_threshold = threshold - 0.15;
    int min_silence_duration_ms = 100, speech_pad_ms = 30;
    std::string jit_file_path = "/Users/marlowe/workspace/epd-system/model/silero_vad.jit";
    
    int window_size = 512;
    vad::VADConfig config;
    // std::shared_ptr<vad::VADInformer> vad_informer = std::make_shared<vad::VADInformer>(config);
   
    // online test
    #if 0
    std::cout << "----------------------Steam test start----------------------" << std::endl;
    int start = 0;
    for (size_t i = 0; i < wave_len; i += snippet_length)
    {
        float **online_start_ptr, **online_end_ptr;
        float *float_pcm = new float[snippet_length];
        int num_of_pairs;
        for (size_t j = 0; j < snippet_length; j++)
        {
            float_pcm[j] = wave_data[start];
            start++;
        }
        if (vad_informer->PushWave(float_pcm, snippet_length, num_of_pairs))
        {
            float *s = vad_informer->get_start_ptr();
            float *e = vad_informer->get_end_ptr();
            if ((*s != -1) && (*e != -1))
            {
                std::cout << "start: " << *s << std::endl;
                std::cout << "end: " << *e << std::endl;
            }
            vad_informer->Release();
        }
        else
        {
            std::cout << "Error" << std::endl;
        }
        delete [] float_pcm;
    }
    
    int num_of_pairs;
    float **offline_start_ptr, **offline_end_ptr;
    std::cout << "----------------------Offline test start----------------------" << std::endl;
    config.online = false;
    vad_informer.reset();
    vad_informer = std::make_shared<vad::VADInformer>(config);
    if (vad_informer->PushWave(wave_data, wave_len, num_of_pairs))
    {
        float *s = vad_informer->get_start_ptr();
        float *e = vad_informer->get_end_ptr();
        for (size_t idx = 0; idx < num_of_pairs; idx++)
        {
            float cur_start = s[idx];
            float cur_end = s[idx];
            std::cout << "start:" << cur_start << std::endl;
            std::cout << "end:" << cur_end << std::endl;
        }
        vad_informer->Release();
    }
    else
    {
        std::cout << "Error" << std::endl;
    }
    #endif
    #if 0
    int snippet_length = 512;
    InitVADSystem(1, config);
    int start = 0;
    std::cout << "----------------------Stream test start----------------------" << std::endl;
    for (size_t i = 0; i < wave_len; i += snippet_length)
    {
        float *float_pcm = new float[snippet_length];
        for (size_t j = 0; j < snippet_length; j++)
        {
            float_pcm[j] = wave_data[start];
            start++;
        }
        float **stream_start{nullptr}, **stream_end{nullptr};
        int *num_of_pairs;
        float s = -1, e = -1;
        float *p_s = &s, *p_e = &e;
        stream_start = &p_s;
        stream_end = &p_e;
        DetectEndPoints(0, float_pcm, snippet_length, stream_start, stream_end, num_of_pairs);
        std::cout << **stream_start << std::endl;
        if ((**stream_start != -1) && (**stream_end != -1))
        {
            std::cout << "Stream start: " << **stream_start << std::endl;
            std::cout << "Stream end: " << **stream_end << std::endl;
        }
        delete *stream_start;
        delete *stream_end;
        delete [] float_pcm;
    }
    Reset(0);
    ExitVADSystem();
    #endif

    #if 0
    std::cout << "----------------------Offline test start----------------------" << std::endl;
    config.online = false;
    InitVADSystem(1, config);
    int num_of_pairs = 1;
    int *num_of_pairs_ptr = &num_of_pairs;
    float *p_s{nullptr}, *p_e{nullptr};
    float **offline_start = &p_s, **offline_end = &p_e;
    DetectEndPoints(0, wave_data, wave_len, offline_start, offline_end, num_of_pairs_ptr);
    Reset(0);
    for (size_t i = 0; i < *num_of_pairs_ptr; i++)
    {
        std::cout << "++++++++++++++++" << i << "++++++++++++++++" << std::endl;
        std::cout << "Offline start: " << (*offline_start)[i] << std::endl;
        std::cout << "Offline end: " << (*offline_end)[i] << std::endl;
        int start_int = (*offline_start)[i]*16000;
        int end_int = (*offline_end)[i]*16000;
        std::cout << "start_int " << start_int << std::endl;
        std::cout << "start_end " << end_int << std::endl;
        float *write_segment = new float[end_int - start_int + 1];
        int start = 0;
        for (size_t j = start_int; j <= end_int; j++)
        {
            write_segment[start] = wave_data[j];
            start++;
        }
        vad::WavWriter wav_writer(write_segment, end_int-start_int+1, 1, sample_rate, bits_per_sample);
        std::string write_file_name = std::string(write_file_name_char) + "_" + to_string(i) + ".wav";
        wav_writer.Write(write_file_name);
        delete [] write_segment;
    }
    delete [] *offline_start;
    delete [] *offline_end;
    ExitVADSystem();
    #endif
    int thread_num = 2;
    config.online = false;
    InitVADSystem(thread_num, config);
    std::thread **thread_address = new std::thread *[thread_num];
    for (size_t i = 0; i < thread_num; i++)
    {
        std::thread
    }
    google::ShutdownGoogleLogging();
}   

int32_t process_file(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("stream_epd --wave-file wav_file_name\n");
        return -1;
    }

    int ans = ParseOptions(argc, argv);
    if (ans < 0)
    {
        std::cout << "Parameter is not enough \r\n";
        return -1;
    }
    ProcessSingleStream(single_wave_file, write_wave_file_char);
    return 0;
}

int main(int argc, char **argv)
{
    process_file(argc, argv);
    return 0;
}