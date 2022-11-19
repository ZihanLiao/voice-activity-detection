#ifndef TORCH_VAD_MODEL_H
#define TORCH_VAD_MODEL_H

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "model/vad_state.h"
#include "torch/torch.h"
#include "torch/script.h"

namespace vad
{
    typedef enum _VAD_ERROR_CODE
    {
        ENGINE_INIT_FAILED = 0,
        ENGINE_INIT_SUCCESSED = 1,

        MODEL_FORWARD_FAILED = 0,
        MODEL_FORWARD_SUCCESSED = 1,

        ITERATION_FAILED = 0,
        ITERATION_SUCCESSED = 1,

    }VAD_ERROR_CODE;

    class VADIterator {
        public:
            VADIterator(const VADIterator&) = delete;
            VADIterator& operator=(const VADIterator&) = delete;
            VADIterator(float threshold, float neg_threshold, int sample_rate,
                        int min_silence_duration_ms, int speech_pad_ms, float min_speech_length_second, float max_speech_length_second,
                        std::string jit_file_path):
            threshold_(threshold), neg_threshold_(neg_threshold), sample_rate_(sample_rate),
            min_speech_length_second_(min_speech_length_second), max_speech_length_second_(max_speech_length_second),
            min_silence_duration_ms_(min_silence_duration_ms), speech_pad_ms_(speech_pad_ms)
            {min_silence_samples_ = sample_rate_ * min_silence_duration_ms_ / 1000;
             speech_pad_samples_ = sample_rate_ * speech_pad_ms_ / 1000;
             model_ = std::make_shared<torch::jit::script::Module>(torch::jit::load(jit_file_path));};
            
            int ConsumeWaveSnippet(const float *buffer, int data_len);
            float get_time_point()const{return current_timepoint_;};
            float get_audio_snippet_score()const{return score_;};
            VADIteratorState get_state()const{return state_;};
            // std::vector<std::pair<float, float>> get_time_stamps(){return timestamps_;};
            int get_sample_rate()const{return sample_rate_;};
            void ResetState();
            inline void ResetIterator()
            {
                state_ = VADIteratorState::BeforeSpeech;
                current_timepoint_ = 0.0;
            };
        private:
            int Forward(const float* buffer, int data_len);
            void PostJudge();

            float threshold_{0.5};
            float neg_threshold_{0.35};
            int min_silence_duration_ms_{100};
            int speech_pad_ms_{30};
            int sample_rate_{16000};

            int min_silence_samples_{0};
            int speech_pad_samples_{0};

            float score_{0.0};
            int temp_end_{0};
            bool triggered_{false};
            float current_timepoint_{0.0};
            int current_sample_{0};
            float min_speech_length_second_{5.0};
            float max_speech_length_second_{60.0};
            int data_len_{512};
            // int pair_idx_{0};
            VADIteratorState state_{VADIteratorState::BeforeSpeech};
            std::pair<float, float> single_timestamp_{0.0, 0.0};
            std::shared_ptr<torch::jit::script::Module> model_;
    };
}
#endif