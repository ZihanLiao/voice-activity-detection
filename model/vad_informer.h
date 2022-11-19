#ifndef VAD_INFORMER_H
#define VAD_INFORMER_H

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <queue>

#include "model/torch_vad_model.h"
#include "model/vad_state.h"
#include "torch/torch.h"
#include "torch/script.h"

namespace vad
{
    
struct VADConfig
{
    std::string jit_file_path = "/Users/marlowe/workspace/epd-system/model/silero_vad.jit";
    int audio_snippet_length{512};
    int sample_rate{16000};
    float threshold{0.5};
    float neg_threshold{0.35};
    int min_silence_duration_ms{30};
    int speech_pad_ms{20};
    float min_speech_length_second{5.0};
    float max_speech_length_second{60.0};
    bool online = true;
};

class VADInformer{
    public:
        VADInformer() = delete;
        VADInformer(const VADInformer& other) = delete;
        VADInformer& operator=(const VADInformer&) = delete;
        VADInformer(const VADConfig &config);
        int PushWave(const float* buffer, int data_len, int *&num_of_pairs);
        float inform()const{return timepoint_;};

        VADInformerState StreamPushWave(const float *buffer, int data_lenx);
        VADInformerState OfflinePushWave(const float *buffer, int data_len,
                                        std::queue<std::pair<float, float>> &pair_collector,
                                        int *&num_of_pairs);
        void Release();
        inline void Reset()
        {
            vad_iterator_->ResetIterator();
            state_ = VADInformerState::FoundNothing;
        };
        int get_pair_count()const{return pair_count_;};
        float* get_start_ptr(){return start_array_;};
        float* get_end_ptr(){return end_array_;};
    private:

        void ConsumeWave(const float *buffer, int data_len);
        void JudgeIteratorState();
        void StreamJudgeInformerState();
        void CollectPairs(std::queue<std::pair<float, float>>& pair_collector);
        float timepoint_{0.0};
        int audio_snippet_length_{512};
        int sample_rate_{16000};
        std::shared_ptr<VADIterator> vad_iterator_;
        VADIteratorState vad_iterator_state_{VADIteratorState::BeforeSpeech};
        VADInformerState state_{VADInformerState::FoundNothing};
        std::pair<float, float> pair_ = std::make_pair(0.0, 0.0);
        int pair_count_{0};
        std::queue<std::pair<float, float>> pair_queue_;
        bool online_{true};
        float *start_array_{nullptr};
        float *end_array_{nullptr};
};
}

#endif