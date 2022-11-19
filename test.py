## using VADIterator class
import torch
import soundfile as sf
from utils_vad import VADIterator,read_audio

def init_jit_model(model_path,
                   device=torch.device('cpu')):
    torch.set_grad_enabled(False)
    model = torch.jit.load(model_path, map_location=device)
    model.eval()
    return model

def make_tg(timestamp, duration, save_path):
    duration_second = duration
    tg = textgrid.TextGrid(minTime=0, maxTime=duration_second)

    tier_word = textgrid.IntervalTier(name="vad", minTime=0, maxTime=duration_second)

    for current_word_idx in range(len(timestamp)):
        if timestamp[current_word_idx][0] == timestamp[current_word_idx][1]:
            continue
        interval = textgrid.Interval(minTime=timestamp[current_word_idx][0], maxTime=timestamp[current_word_idx][1],
                                     mark=timestamp[current_word_idx][2])

        tier_word.addInterval(interval)

    tg.tiers.append(tier_word)
    tg.write(save_path)


def vad_result_to_timestamp(vad_list):

    start_pointer = 0
    end_pointer = 0
    last_value = None
    output_result = []
    label_list = []

    for frame_idx in range(len(vad_list)):
        frame_result = vad_list[frame_idx]
        if last_value is None:
            last_value = frame_result
            end_pointer += 1
        else:
            if frame_result == last_value:
                last_value = frame_result
                end_pointer += 1
            else:
                last_value = frame_result
                if sum(vad_list[start_pointer:end_pointer]) == 0:
                    output_result.append([start_pointer, end_pointer, '0'])
                else:
                    output_result.append([start_pointer, end_pointer, "1"])
                end_pointer += 1
                start_pointer = end_pointer
        if frame_idx == len(vad_list) - 1:
            if sum(vad_list[start_pointer:end_pointer]) == 0:
                output_result.append([start_pointer, end_pointer, "0"])
            else:
                output_result.append([start_pointer, end_pointer, "1"])

    return output_result


def process(vad_list, save_path):

    # frame_idx_list = vad_result_to_timestamp(vad_list)
    timestamp = [[i[0] * 0.01, i[1] * 0.01, i[2]] for i in frame_idx_list]
    duration = len(vad_list) * 0.01
    make_tg(timestamp, duration, save_path)
    print(timestamp)

if __name__ == "__main__":
  model = init_jit_model("/Users/marlowe/workspace/epd-system/model/silero_vad.jit")
  vad_iterator = VADIterator(model, sampling_rate=16000, threshold=0.5)
  wav = read_audio(f'/Users/marlowe/workspace/epd-system/build/bin/1019041_977102_2020-02-12_080000_2020-02-12_100000_16K.wav', sampling_rate=16000)
  audio, fs = sf.read(f'/Users/marlowe/workspace/epd-system/build/bin/1019041_977102_2020-02-12_080000_2020-02-12_100000_16K.wav')
  window_size_samples = 512 # number of samples in a single audio chunk
  timesteps = []
  count = 0
  s = []
  for i in range(0, len(wav), window_size_samples):
      chunk = wav[i: i+ window_size_samples]
      if len(chunk) < window_size_samples:
        break
      count += 1
      speech_dict = vad_iterator(chunk, return_seconds=True)
      
      if speech_dict:
          for ts in speech_dict.values():
            timesteps.append(ts)
          print(speech_dict, end=' ')
          

  import textgrid
  output_result = []
  output_result.append([0, timesteps[0], '0'])
  last_state = 0
  for i in range(len(timesteps)-1):
      if last_state == 0:
          last_state = 1
          cur_str = '1'
      else:
          last_state = 0
          cur_str = '0'
      output_result.append([timesteps[i], timesteps[i+1], cur_str])
  
  make_tg(output_result, len(audio)*0.01, "./textgrid")



