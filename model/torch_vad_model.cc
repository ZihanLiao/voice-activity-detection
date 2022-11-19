#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>
#include <iterator>
#include <exception>

#include "model/torch_vad_model.h"
#include "model/vad_state.h"
#include "utils/logging.h"
#include "torch/script.h"
#include "torch/torch.h"

namespace vad
{
    int VADIterator::ConsumeWaveSnippet(const float* buffer, int data_len)
    {
        if (Forward(buffer, data_len))
        {
            PostJudge();
            return ITERATION_SUCCESSED;
        }
        else
        {
            return ITERATION_FAILED;
        }
    }

    int VADIterator::Forward(const float* buffer, int data_len)
    {
        data_len_ = data_len;  
        torch::NoGradGuard no_grad;
        torch::Tensor tensor_buffer = torch::from_blob(const_cast<float*>(buffer), {data_len}, torch::kFloat);
        std::vector<torch::jit::IValue> inputs = {std::move(tensor_buffer), sample_rate_};
        try
        {
            at::Tensor output = model_->forward(inputs).toTensor();
            score_ = output[0].item<float>();
            current_sample_ += data_len;
            return MODEL_FORWARD_SUCCESSED;
        }
        catch (std::exception *e)
        {
            std::cout << e->what() << std::endl;
            return MODEL_FORWARD_FAILED;
        }
    }

    void VADIterator::PostJudge()
    {
        if ((score_ >= threshold_) && temp_end_ != 0)
        {
            temp_end_ = 0;
        }

        if ((score_ < threshold_) && (state_ == VADIteratorState::BeforeSpeech))
        {
            return;
        }

        if ((score_ >= threshold_) && (!triggered_) && (state_ == VADIteratorState::BeforeSpeech))
        {
            triggered_ = true; // Triggered, human voice started
            LOG(INFO) << "INFO: Triggered, human voice started, current score is " << score_;
            state_ = VADIteratorState::HumanVoiceFound;
            float speech_start = current_sample_ - speech_pad_samples_;
            current_timepoint_ = round(speech_start / sample_rate_ * 100) / 100;
            single_timestamp_.first = current_timepoint_;
            return;
        }

        if ((score_ >= threshold_) && (triggered_) && (state_ == VADIteratorState::HumanVoiceFound || state_ == VADIteratorState::StillInSpeech))
        {   
            current_timepoint_ = round(current_sample_ / sample_rate_ * 100) / 100;
            if ((current_timepoint_ - single_timestamp_.first) < max_speech_length_second_)
            {
                state_ = VADIteratorState::StillInSpeech;
                return;
            }
            else
            {
                // temp_end_ = 0;
                triggered_ = false;
                state_ = VADIteratorState::HumanVoiceEnded;
                LOG(WARNING) << "WARNING: Human voice is too long, forced to clip the snippet according to the max_speech_length_second";
                LOG(WARNING) << "WARNING: Oiriginal timepoint is " << current_timepoint_;
                float new_current_timepoint_ = round((current_sample_ - data_len_) / sample_rate_ * 100) / 100;
                LOG(WARNING) << "WARNING: Adjust the original timepoint to the new timepoint which is " << new_current_timepoint_;
                current_timepoint_ = new_current_timepoint_;
                single_timestamp_.second = current_timepoint_;
                return;
            }
        }

        if ((score_ < neg_threshold_) && (triggered_) && (state_ == VADIteratorState::StillInSpeech || state_ == VADIteratorState::HumanVoiceFound))
        {
            if (temp_end_ == 0)
            {
                temp_end_ = current_sample_;
            }

            if ((current_sample_ - temp_end_) < min_silence_samples_)
            {
                return;
            }
            else
            {
                float speech_end = temp_end_ + speech_pad_samples_;
                current_timepoint_ = round(speech_end / sample_rate_ * 100) / 100;
                if ((current_timepoint_ - single_timestamp_.first) < min_speech_length_second_)
                {
                    temp_end_ = 0;
                    state_ = VADIteratorState::StillInSpeech;
                    LOG(WARNING) << "WARNING: Human voice is too short, continue, current score is : " << score_;
                    return;
                }
                else if ((current_timepoint_ - single_timestamp_.first) > max_speech_length_second_)
                {
                    temp_end_ = 0;
                    triggered_ = false; // Triggered, human voice ended
                    state_ = VADIteratorState::HumanVoiceEnded;
                    LOG(WARNING) << "WARNING: Human voice is too long, forced to clip the snippet according to the max_speech_length_second";
                    LOG(WARNING) << "WARNING: Oiriginal timepoint is " << current_timepoint_;
                    float new_current_timepoint_ = round((speech_end - data_len_) / sample_rate_ * 100) / 100;
                    LOG(WARNING) << "WARNING: Adjust the original timepoint to the new timepoint which is " << new_current_timepoint_;
                    current_timepoint_ = new_current_timepoint_;
                    single_timestamp_.second = current_timepoint_;
                    return;
                }
                else
                {                    
                    temp_end_ = 0;
                    triggered_ = false; // Triggered, human voice ended
                    state_ = VADIteratorState::HumanVoiceEnded;
                    LOG(INFO) << "INFO: Triggered, human voice ended, current score is " << score_;
                    single_timestamp_.second = current_timepoint_;
                    return;    
                }

            }
        }

    }

    void VADIterator::ResetState()
    {
        state_ = VADIteratorState::BeforeSpeech;
        temp_end_ = 0;
    }
}