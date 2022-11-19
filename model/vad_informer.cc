#include <iostream>

#include "model/vad_informer.h"
#include "model/torch_vad_model.h"
#include "torch/torch.h"
#include "torch/script.h"
#include "utils/logging.h"

namespace vad
{
    VADInformer::VADInformer(const VADConfig &config)
    {
        vad_iterator_ = std::make_shared<VADIterator>(config.threshold, config.neg_threshold, 
                                config.sample_rate, config.min_silence_duration_ms,
                                config.speech_pad_ms, config.min_speech_length_second, config.max_speech_length_second,
                                config.jit_file_path);
        audio_snippet_length_ = config.audio_snippet_length;
        sample_rate_ = config.sample_rate;
        online_ = config.online;
        if (online_ == true)
        {
            LOG(INFO) << "---------------Stream Mode---------------";
        }
        else
        {
            LOG(INFO) << "---------------Offline Mode---------------";
        }
    }

    void VADInformer::ConsumeWave(const float *buffer, int data_len)
    {
        int start = 0;   
        for (size_t i = 0; i < data_len; i+= audio_snippet_length_)
        {
            auto *float_pcm = new float[audio_snippet_length_];
            for (size_t count = 0; count < audio_snippet_length_; count++)
            {
                float_pcm[count] = buffer[start];
                start++;
            }
            vad_iterator_->ConsumeWaveSnippet(float_pcm, audio_snippet_length_);
            JudgeIteratorState();
            delete [] float_pcm;
        }
        if (!online_)
        {
            // In offline mode, if the human voice is still keeping till the end
            // cut it manually and push a pair
            if (state_ == VADInformerState::FoundStart)
            {
                pair_.second = round(data_len / sample_rate_ * 100) / 100;
                pair_queue_.push(pair_);
                pair_count_++;
            }
        }
        
        LOG(INFO) << "INFO: " << pair_count_ << " pairs found so far";
    }

    int VADInformer::PushWave(const float *buffer, int data_len, int *&num_of_pairs)
    {
        if (online_ == true)
        {
            if (data_len != audio_snippet_length_)
            {
                LOG(ERROR) << "Error: Input data length should be equal to configured snippet length (by default data_len = 512)";
                return 0;
            }
            start_array_ = new float(-1.0);
            end_array_ = new float(-1.0);
            if (start_array_ == nullptr || end_array_ == nullptr)
            {
                LOG(ERROR) << "Error: Can't new a pointer";
                return 0;
            }
            VADInformerState system_state = StreamPushWave(buffer, data_len);
            *num_of_pairs = 1;
            return 1;
        }
        else
        {
            std::queue<std::pair<float, float>> pair_collector;
            VADInformerState system_state = OfflinePushWave(buffer, data_len, pair_collector, num_of_pairs);
            if (system_state == VADInformerState::FoundNothing && *num_of_pairs == 0)
            {
                LOG(INFO) << "INFO: Found nothing";
                return 1;
            }
            start_array_ = new float[*num_of_pairs];
            end_array_ = new float[*num_of_pairs];
            if (start_array_ == nullptr || end_array_ == nullptr)
            {
                LOG(ERROR) << "Error: Can't new a pointer";
                return 0;
            }
            for (size_t i = 0; i < *num_of_pairs; i++)
            {
                std::pair<float, float> cur_pair = pair_collector.front();
                start_array_[i] = cur_pair.first;
                end_array_[i] = cur_pair.second;
                pair_collector.pop();
            }
            return 1;
        }
    }
    
    void VADInformer::Release()
    {   
        if (online_)
        {
            delete start_array_;
            start_array_ = nullptr;
            delete end_array_;
            end_array_ = nullptr;
        }
        else
        {
            delete [] start_array_;
            start_array_ = nullptr;
            delete [] end_array_;
            end_array_ = nullptr;
        }
    }

    VADInformerState VADInformer::StreamPushWave(const float *buffer, int data_len)
    {
        // Stream mode, state transfermation is under supervised
        ConsumeWave(buffer, data_len);
        StreamJudgeInformerState();
        return state_;
    }

    VADInformerState VADInformer::OfflinePushWave(const float *buffer, int data_len, 
                                    std::queue<std::pair<float, float>> &pair_collector, int *&num_of_pairs)
    {
        // Offline mode, state transfermation is not under supervised
        ConsumeWave(buffer, data_len);
        *num_of_pairs = pair_queue_.size();
        for (size_t i = 0; i < *num_of_pairs; i++)
        {
            std::pair<float, float> cur_pair = pair_queue_.front();
            pair_collector.push(cur_pair);
            pair_queue_.pop();
        }
        return state_;
    }

    void VADInformer::StreamJudgeInformerState()
    {
        if (state_ == VADInformerState::FoundNothing)
        {
            // Found nothing
            *start_array_ = -1.0;
            *end_array_ = -1.0;
        }
        else if ((state_ == VADInformerState::FoundStart) && (pair_count_ == 0))
        {
            // Found start only
            *start_array_ = pair_.first;
            *end_array_ = -1.0;
        }
        else if (state_ == VADInformerState::FoundPair)
        {
            // Found pairs
            *start_array_ = pair_.first;
            *end_array_ = pair_.second;
            pair_ = std::make_pair(0.0, 0.0);
        }
        else if ((state_ == VADInformerState::FoundStart) && (pair_count_ != 0))
        {
            // Found pairs and end with start
            *start_array_ = pair_.first;
            *end_array_ = -1.0;
        }
    }

    void VADInformer::JudgeIteratorState()
    {
        vad_iterator_state_ = vad_iterator_->get_state();
        switch(vad_iterator_state_)
        {
            case VADIteratorState::BeforeSpeech:
            {
                state_ = VADInformerState::FoundNothing;
                break;
            }
            case VADIteratorState::HumanVoiceFound:
            {
                timepoint_ = vad_iterator_->get_time_point();
                pair_.first = timepoint_;
                state_ = VADInformerState::FoundStart;
                break;
            }
            case VADIteratorState::StillInSpeech:
            {
                // Do nothing
                break;
            }
            case VADIteratorState::HumanVoiceEnded:
            {
                timepoint_ = vad_iterator_->get_time_point();
                pair_.second = timepoint_;
                vad_iterator_->ResetState();
                state_ = VADInformerState::FoundPair;
                pair_queue_.push(pair_);
                pair_count_++;
                if (!online_)
                    pair_ = std::make_pair(0.0, 0.0);
                break;
            }
            default:
            {
                LOG(ERROR) << "ERROR: Iterator state doesn't match";
            }
        }
    }

    void VADInformer::CollectPairs(std::queue<std::pair<float, float>>& pair_collector)
    {
        // Collect pairs
        while (!pair_queue_.empty())
        {
            std::pair<float, float> cur_pair = pair_queue_.front();
            pair_queue_.pop();
            pair_collector.push(cur_pair);
        }
        pair_count_ = 0;
    }
}