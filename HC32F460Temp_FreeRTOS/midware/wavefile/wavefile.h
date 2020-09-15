#ifndef WAVEFILE_H
#define WAVEFILE_H
#include "hc32_ddl.h"
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
	uint32_t Format;
}ChunkRIFF;
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
	uint16_t AudioFormat;
	uint16_t NumOfChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint32_t BlockAlign;
	uint16_t BitsPerSample;
}ChunkFMT;
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
	uint32_t DataFactSize;
}ChunkFACT;
typedef struct
{
	uint32_t ChunkID;
	uint32_t ChunkSize;
}ChunkDATA;
typedef struct
{
	ChunkRIFF riff;
	ChunkFMT fmt;
//	ChunkFACT fact;
	ChunkDATA data;
}WaveHeader;

void wavefileinfo_init(WaveHeader* wavehead);
#endif
