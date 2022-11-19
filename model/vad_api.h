#ifndef VAD_API_H
#define VAD_API_H

#include "model/vad_informer.h"
#include "torch/torch.h"
#include "torch/script.h"
#include "utils/logging.h"

typedef enum VADSystemCode_
{
    SYSTEM_OK = -1,
    SYSTEM_ERROR_NULL_LINE,
    SYSTEM_ERROR_VOICE_DETECTOR,


}VADSystemCode;

typedef enum VADSystemState_
{
    STREAM_FOUND_NOTHING,
    STREAM_FOUND_START_ONLY,
    STREAM_FOUND_PAIRS,
    OFFLINE_FIND_START_ONLY_OR_NO_PAIRS,
    OFFLINE_FIND_PAIRS

}VADSystemState;

int InitVADSystem(int num_of_thread, vad::VADConfig &config);
int ExitVADSystem();
int RunVADSystem();
int DetectEndPoints(int engine_id, const float *buffer, int buffer_length, float **start, float **end, int *num_of_pairs);
int StreamLogic(float *start_ptr, float *end_ptr, int *&num_of_pairs, float *&start, float *&end);
int OfflineLogic(float *start_ptr, float *end_ptr, int *&num_of_pairs, float *&start, float *&end);
int Reset(int engine_id);
#endif