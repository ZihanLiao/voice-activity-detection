#include <cassert>
#include <exception>
#include "frontend/WaveUtil.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <cmath>
#include <climits>
#include <boost/filesystem.hpp>

namespace bf = boost::filesystem;

//#include <boost/algorithm/string.hpp>



VADConfig::VADConfig()
{
}

VADConfig::~VADConfig()
{
}



//////////////////////////////////////////////////////////////////////////


WAVE::WAVE(const char* filename, const char* wave_id)
{
	bf::path file_path(filename);
	if (filename && wave_id)
	{
		if (! bf::exists(file_path))
		{
			std::cout << "#File is not exist ! Please check it ！\r\n";
			exit(-1);
		}
		m_waveHeader = ReadWaveHeader(filename);
		m_waveData = ReadWaveDate(filename);

#ifdef _DEBUG
		string tmp_out_wave = filename;
		tmp_out_wave.append("_debug.wav"); //only for debug
		FILE* tmp_fp = fopen(tmp_out_wave.data(), "wb");
		WriteWaveHeader(m_waveHeader, tmp_fp);
		fwrite(m_waveData, sizeof(short), m_waveHeader->data_size / 2, tmp_fp);
		fclose(tmp_fp);
#endif
		strcpy(m_waveId, wave_id);
		m_waveId[strlen(m_waveId) + 1] = '\0';
		strcpy(m_waveDir, filename);
		m_waveDir[strlen(m_waveDir) + 1] = '\0';

	}
	else{
		m_waveData = 0;
		m_waveHeader = 0;
		m_waveId[0] = '\0';
		m_waveDir[0] = '\0';
	}
	sample_rate_ = m_waveHeader->sample_rate;
	byte_num_ = m_waveHeader->bits_per_sample;

}

WAVE::WAVE(const char* file_name, int sample_rate, int byte_num) :byte_num_(byte_num), sample_rate_(sample_rate)
{
	bf::path file_path(file_name);
	if (file_name)
	{
		if ( ! bf::exists(file_path))
		{
			std::cout << "File is not exist ! Please check it !" << std::endl;
			exit(-1);
		}
		m_waveHeader = ReadWaveHeader(file_name);
		m_waveData = ReadWaveDate(file_name);

		m_waveId[0] = '\0';
		strcpy(m_waveDir, file_name);
		m_waveDir[strlen(m_waveDir) + 1] = '\0';

	}
	else{
		m_waveData = 0;
		m_waveHeader = 0;
		m_waveId[0] = '\0';
		m_waveDir[0] = '\0';
	}
}
WAVE::~WAVE()
{
	if (m_waveHeader)
	{
		delete m_waveHeader; m_waveHeader = nullptr;

	}
	if (m_waveData)
	{
		delete [] m_waveData; m_waveData = nullptr;
	}
}

const WAV_HEADER* WAVE::getWaveHeader() const
{
	return m_waveHeader;
}


WAV_HEADER* WAVE::ReadWaveHeader(const char* file_name)
{
	if (!file_name)
	{
		return 0;
	}
	if (!IsFileAWave(file_name))
	{
		return 0;
	}
	FILE* fp = fopen(file_name, "rb");
	WAV_HEADER* currHeader = new WAV_HEADER;

	fread(currHeader[0].riff_id, sizeof(char), 4, fp); //find 'RIFF' flag

	if ((currHeader[0].riff_id[0] == 'R') && (currHeader[0].riff_id[1] == 'I') &&
		(currHeader[0].riff_id[2] == 'F') && (currHeader[0].riff_id[3] == 'F'))
	{
		fread(&currHeader[0].file_size, sizeof(unsigned int), 1, fp); // 
		fread(currHeader[0].wave_id, sizeof(char), 4, fp);         // 


		if ((currHeader[0].wave_id[0] == 'W') && (currHeader[0].wave_id[1] == 'A') &&
			(currHeader[0].wave_id[2] == 'V') && (currHeader[0].wave_id[3] == 'E'))
		{
			fread(currHeader[0].fmt_id, sizeof(char), 4, fp);     //
			fread(&currHeader[0].format_length, sizeof(unsigned int), 1, fp);
			fread(&currHeader[0].format_tag, sizeof(short), 1, fp); // 
			fread(&currHeader[0].channels, sizeof(short), 1, fp);    // 
			fread(&currHeader[0].sample_rate, sizeof(unsigned int), 1, fp);   // 
			fread(&currHeader[0].avg_bytes_sec, sizeof(unsigned int), 1, fp); // 
			fread(&currHeader[0].block_align, sizeof(short), 1, fp);     // 
			fread(&currHeader[0].bits_per_sample, sizeof(short), 1, fp);       // 
			fread(currHeader[0].data_id, sizeof(char), 4, fp);                      // 
			fread(&currHeader[0].data_size, sizeof(unsigned int), 1, fp);     // 
		}
		else
		{
			printf("Error: RIFF file but not a wave file\n");
			fclose(fp);
			return 0;
		}
	}
	else
	{
		printf("Error: not a RIFF file\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return currHeader;
}

short* WAVE::ReadWaveDate(const char* file_name)
{
	if(file_name == nullptr) return nullptr;
	bool isWave = false;
	if (IsFileAWave(file_name))
	{
		isWave = true;
	}
	short* data = 0;
	try{
		FILE* fp = fopen(file_name, "rb");
		long long dataSize = 0;
		fseek(fp, 0, SEEK_END);
		if (isWave)
		{
			dataSize = ftell(fp) - 44;
		}
		else{
			dataSize = ftell(fp);
		}
		fseek(fp, 44, SEEK_SET);
		data = new short[dataSize / 2];
		fread(data, sizeof(short), dataSize / 2, fp);
		fclose(fp);
	}
	catch(std::exception* e){
		std::cout << e->what() << std::endl;
	}
	
	return data;
}


int WAVE::WriteWaveHeader(const WAV_HEADER* wave_header, FILE* fp)
{
	if (fp)
	{
		//fwrite(&wave_header,sizeof(WAV_HEADER),1,fp);
		fwrite(&(wave_header->riff_id), sizeof(char), 4, fp);
		fwrite(&(wave_header->file_size), sizeof(unsigned int), 1, fp);
		fwrite(wave_header->wave_id, sizeof(char), 4, fp);

		fwrite(wave_header->fmt_id, sizeof(char), 4, fp);     // 
		fwrite(&(wave_header->format_length), sizeof(unsigned int), 1, fp);
		fwrite(&(wave_header->format_tag), sizeof(short), 1, fp); //
		fwrite(&(wave_header->channels), sizeof(short), 1, fp);    //
		fwrite(&wave_header->sample_rate, sizeof(unsigned int), 1, fp);   //
		fwrite(&wave_header->avg_bytes_sec, sizeof(unsigned int), 1, fp); //
		fwrite(&wave_header->block_align, sizeof(short), 1, fp);     //
		fwrite(&wave_header->bits_per_sample, sizeof(short), 1, fp);       //
		fwrite(&wave_header->data_id, sizeof(char), 4, fp);                  //
		fwrite(&wave_header->data_size, sizeof(unsigned int), 1, fp);     // 

		return 0;
	}
	return -1;
}




//////////////////////////////////////////////////////////////////////////

bool IsFileAWave(const char* file_name)
{
	if (!file_name){
		std::cout << "#File name is empty !" << std::endl;
		return false;
	}
	char fileHeader[5] = { '\0' };
	try
	{
		FILE* fp = fopen(file_name, "rb");
		fread(fileHeader, sizeof(char), 4, fp);
		fclose(fp);
	}
	catch (std::exception* e)
	{
		std::cout << e->what() << std::endl;
	}
	if (strcmp(fileHeader, "RIFF") == 0)
	{
		return true;
	}
	return false;
}



WAV_HEADER* MakeWaveHeader(const unsigned long file_size_, unsigned short channel_nums_,
	unsigned long sample_rate_, unsigned long avg_bytes_sec, unsigned short bits_per_sample_)
{
	//size of block is : 44 chars
	WAV_HEADER* wave_header = new WAV_HEADER;
	memset(wave_header, 0, sizeof(WAV_HEADER));
	wave_header[0].riff_id[0] = 'R';
	wave_header[0].riff_id[1] = 'I';
	wave_header[0].riff_id[2] = 'F';
	wave_header[0].riff_id[3] = 'F';
	// 4 chars for storing 'RIFF'.4 chars and one '\0' can be useful for using strcmp
	wave_header[0].file_size = file_size_ + 44 - 8;	// int type, size of file
	//"WAVE" notification
	wave_header[0].wave_id[0] = 'W';
	wave_header[0].wave_id[1] = 'A';
	wave_header[0].wave_id[2] = 'V';
	wave_header[0].wave_id[3] = 'E';
	//"fmt ", 4 chars
	wave_header[0].fmt_id[0] = 'f';
	wave_header[0].fmt_id[1] = 'm';
	wave_header[0].fmt_id[2] = 't';
	wave_header[0].fmt_id[3] = ' ';

	wave_header[0].format_length = 16; // 16 bits data
	wave_header[0].format_tag = 1;//short, for pcm, it is equal to 1

	wave_header[0].channels = channel_nums_;//channel number
	wave_header[0].sample_rate = sample_rate_; //sample rate
	wave_header[0].avg_bytes_sec = avg_bytes_sec;
	wave_header[0].block_align = channel_nums_ * 16 / 8;
	wave_header[0].bits_per_sample = bits_per_sample_;


	wave_header[0].data_id[0] = 'd';
	wave_header[0].data_id[1] = 'a';
	wave_header[0].data_id[2] = 't';
	wave_header[0].data_id[3] = 'a';
	wave_header[0].data_size = file_size_;

	return wave_header;
}



//////////////////////////////////////////////////////////////////////////

bool IsFileExist(const char* filename)
{
	if (!filename){
		std::cout << "file name is empty !\r\n";
		return false;
	}
	std::ifstream fin(filename);
	if (!fin)
	{
#ifdef _DEBUG
		std::cout << "can not open this file or file is not exist" << endl;
#endif
		return false;
	}
	fin.close();
	return true;
}

bool IsDirectoryExist(const char* dirname)
{
	if (!dirname){
		std::cout << "Directory name is empty !\r\n";
		return false;
	}

	return true;
}

std::string MakeStandardIndexFileName(const char* name_base, int curr_index)
{
	std::string rest = "";
	if (! name_base)
	{
		return rest;
	}
	char format_name[512] = { '\0' };
#ifdef _MSC_VER
#if _MSC_VER > 1600
	sprintf_s(format_name, "%s_%08d%s", name_base, curr_index, ".txt");
#endif
#else
	sprintf(format_name, "%s_%08d%s", name_base, curr_index, ".txt");
#endif
	rest.assign(format_name);
	return rest;
}


int RoundUpToNearestPowerOfTwo(int n) {
	assert(n > 0);
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return n + 1;
}


template <typename DataT>
DataT MidianFiltering(DataT* array, int win_length)
{
	if (array == 0 || win_length % 2 != 1)
	{
		std::cout << "# Data is empty or window length is not order number !\r\n";
		return std::numeric_limits<int>::max();
	}
}

bool IsContainDirectory(const char* char_str)
{
	bool test_result = false;
	std::string str(char_str);
	std::string::const_iterator delimiter_pos = str.end();
	if (std::find(str.begin(), str.end(), '\\') != delimiter_pos || std::find(str.begin(), str.end(), '\/') != delimiter_pos)
	{
		return true;
	}
	return test_result;
}


std::string RemovePartFromString(const std::string& original_str, const std::string& part_str)
{
	//using namespace boost::algorithm;
	std::string result = "";
	if (original_str.empty() || part_str.empty())
	{
		return result;
	}
	std::string local_original_str = original_str.c_str(), local_part_str = part_str.c_str();
	//std::transform(local_part_str.begin, local_part_str.end, local_part_str.begin(), ::tolower);
	//std::transform(local_original_str.begin, local_original_str.end, local_original_str.begin(), ::tolower);
	std::string::size_type part_pos = local_original_str.find(local_part_str.c_str());
	if (std::string::npos != part_pos)
	{
		std::string first_part, next_part, tmp_result;
		first_part = local_original_str.substr(0, part_pos);
		next_part = local_original_str.substr(part_pos + local_part_str.size());
		tmp_result = first_part + next_part;
		if (! tmp_result.empty())
		{
			return result.assign(tmp_result);
		}
	}
	return result;
}



