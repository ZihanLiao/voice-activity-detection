#ifndef _WAVEUTIL_H_
#define _WAVEUTIL_H_

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#include "utils/my_utils.h"

typedef enum WaveType
{
    RAWPCM = 0,
    MICROSOFT_WAVE = 1,
    OTHERS = 2
};

typedef struct WaveOptions
{
    std::string wave_kaldi_scp = "";
    int sample_rate = 8000;
    int byte_num = 2;
};

class VADConfig
{
public:
    VADConfig();
    ~VADConfig();

    void Init();

protected:
private:
    std::map<std::string, std::string> config_;
    int win_length_;
    int forward_frame_length_;
    int backward_frame_length_;
};

typedef struct
{
    char riff_id[4];                // 4 chars for storing 'RIFF'.4 chars and one '\0' can be useful for using strcmp
    unsigned int file_size;         // 4chars, size of file
    char wave_id[4];                // 4chars, "WAVE"
    char fmt_id[4];                 // 4chars, "fmt ", the last char is blank
    long int format_length;         // 4 chars
    unsigned short format_tag;      // 2chars
    unsigned short channels;        // 2chars
    unsigned int sample_rate;       // 4chars
    unsigned int avg_bytes_sec;     // 4 chars
    unsigned short block_align;     // 2 chars
    unsigned short bits_per_sample; // 2 chars, 16 bits data
    char data_id[4];                // 4chars
    unsigned int data_size;         // 4chars,  32 bits data
} WAV_HEADER;

class WAVE
{
public:
    WAVE(const char *filename, const char *wave_id);
    WAVE(const char *file_name, int sample_rate = 8000, int byte_num = 2);
    ~WAVE();

    inline int WaveLength() { return m_waveHeader->data_size/ (m_waveHeader->bits_per_sample / 8); }
    inline int SampleRate() { return m_waveHeader->sample_rate; }
    inline int BitsPerSample() { return m_waveHeader->bits_per_sample; }
    inline char *GetWaveId() { return &m_waveId[0]; }
    inline char *GetWaveDir() { return &m_waveDir[0]; }
    int MakeWavHeader();
    // int PrintWaveInfo();
    inline void ChangeWaveLength(unsigned new_length) { m_waveHeader->data_size = new_length; }
    inline void ChangeWaveSampleRate(unsigned new_samplerate) { m_waveHeader->sample_rate = new_samplerate; }

    int DoSpeechSplit(unsigned subseg_id, float start, float end, float offset,
                      unsigned sample_rate, const char *output_dir);
    int WriteWaveHeader(const WAV_HEADER *wave_header, FILE *fp);

    const WAV_HEADER *getWaveHeader() const;
    WAV_HEADER *ReadWaveHeader(const char *file_name);
    short *ReadWaveDate(const char *file_name);

    short *GetWaveData() { return m_waveData; }

private:
    short *m_waveData;
    WAV_HEADER *m_waveHeader;
    char m_waveId[128];
    char m_waveDir[512];
    int sample_rate_;
    int byte_num_;
};

bool IsFileAWave(const char *file_name);

int GetFilePath(std::string &strFilePath);

WAV_HEADER *MakeWaveHeader(const unsigned long file_size_, unsigned short channel_nums_,
                           unsigned long sample_rate_, unsigned long avg_bytes_sec, unsigned short bits_per_sample_);

std::vector<std::string> SplitLine(const std::string &original_line, int split_num = 2);

//////////////////////////////////////////////////////////////////////////

bool IsFileExist(const char *filename);
bool IsDirectoryExist(const char *dirname);
std::string MakeStandardIndexFileName(const char *name_base, int curr_index);
int RoundUpToNearestPowerOfTwo(int n);
template <typename DataT>
DataT MidianFiltering(DataT *array, int win_length);
bool IsContainDirectory(const char *char_str);
std::string RemovePartFromString(const std::string &original_str, const std::string &part_str);

#endif
