#include <iostream>

#include "model/vad_informer.h"
#include "model/vad_api.h"
#include "torch/torch.h"
#include "torch/script.h"
#include "utils/logging.h"

static vad::VADInformer **g_informer_ptr = nullptr;
static vad::VADConfig g_config;
static int g_thread_num = 1;

int InitVADSystem(int num_of_thread, vad::VADConfig &config)
{
    g_thread_num = num_of_thread;
    g_config = config;
    using VADInformerPtr = vad::VADInformer **;
    VADInformerPtr engine_array = new vad::VADInformer *[g_thread_num];
    for (size_t i = 0; i < g_thread_num; i++)
    {
        engine_array[i] = new vad::VADInformer(g_config);
        if (engine_array[i] != nullptr)
            LOG(INFO) << "Thread " << i << " init successfully";
    }
    g_informer_ptr = engine_array;
    if (g_informer_ptr == nullptr)
        return SYSTEM_ERROR_NULL_LINE;
    return SYSTEM_OK;
}

int ExitVADSystem()
{
    if (g_informer_ptr == nullptr)
    {
        return SYSTEM_ERROR_VOICE_DETECTOR;
    }
    for (size_t i = 0; i < g_thread_num; i++)
    {
        delete g_informer_ptr[i];
        g_informer_ptr[i] == nullptr;
    }
    return SYSTEM_OK;
}

int DetectEndPoints(int engine_id, const float *buffer, int buffer_length, float **start, float **end, int *num_of_pairs)
{
    // float *float_buffer = new float[buffer_length];
    // for(size_t i = 0; i < buffer_length; i++)
    // {
    //     float_buffer[i] = static_cast<float>(buffer[i]);
    // }
    LOG(INFO) << "INFO: Running vad system, detecting endpoints";
    g_informer_ptr[engine_id]->PushWave(buffer, buffer_length, num_of_pairs);
    // delete [] float_buffer;
    float *tmp_start = g_informer_ptr[engine_id]->get_start_ptr();
    float *tmp_end = g_informer_ptr[engine_id]->get_end_ptr();
    if (g_config.online == true)
    {
        int state = StreamLogic(tmp_start, tmp_end, num_of_pairs, *start, *end);
        g_informer_ptr[engine_id]->Release();
        tmp_start = nullptr;
        tmp_end = nullptr;
        return state;
    }
    else
    {
        int state = OfflineLogic(tmp_start, tmp_end, num_of_pairs, *start, *end);
        g_informer_ptr[engine_id]->Release();
        tmp_start = nullptr;
        tmp_end = nullptr;
        return state;
    }
}

int StreamLogic(float *start_ptr, float *end_ptr, int *&num_of_pairs, float *&start, float *&end)
{
    float float_start = *start_ptr;
    float float_end = *end_ptr;

    if ((float_start == -1) && (float_end == -1))
    {
        start = new float(float_start);
        end = new float(float_end);
        LOG(INFO) << "INFO: In stream mode, found nothing";
        return STREAM_FOUND_NOTHING;
    }
    else if ((float_start != -1) && (float_end == -1))
    {
        start = new float(float_start);
        end = new float(float_end);
        LOG(INFO) << "INFO: In stream mode, found start only";
        return STREAM_FOUND_START_ONLY;
    }
    else if ((float_start != -1) && (float_end != -1))
    {
        start = new float(float_start);
        end = new float(float_end);
        LOG(INFO) << "INFO: In stream mode, found pairs";
        return STREAM_FOUND_PAIRS;
    }
}

int OfflineLogic(float *start_ptr, float *end_ptr, int *&num_of_pairs, float *&start, float *&end)
{
    start = new float[*num_of_pairs];
    end = new float[*num_of_pairs];
    if (*num_of_pairs == 0)
    {
        LOG(INFO) << "INFO: In offline mode, found nothing";
        return OFFLINE_FIND_START_ONLY_OR_NO_PAIRS;
    }
    else
    {
        LOG(INFO) << "INFO: In offline mode, found pairs";
        for (size_t i = 0; i < *num_of_pairs; i++)
        {
            start[i] = start_ptr[i];
            end[i] = end_ptr[i];
        }
        return OFFLINE_FIND_PAIRS;
    }
    
}

int Reset(int engine_id)
{
    g_informer_ptr[engine_id]->Reset();
}
