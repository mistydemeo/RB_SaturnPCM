/*
        File: WaveFile.Hpp

        By Yung-hsiang Lee, Univ. of Southern California

        All Copyright reserved.

        Header file for .Wav File I/Os

        Distribution of the source file is permitted in original form.
          Modified version of the source file distribution is permitted
          if author's name is retained.

          Further modification or incorporation into other programs are 
          allowed without obtaining further agreement from author.

*/

#include <inttypes.h>

#ifndef _WAVEFILE_
  #define _WAVEFILE_

#define WAVE_FORMAT_PCM 1

typedef struct {
/* File header */
  char riff[4];
  int32_t filesize;
  char rifftype[4];
} RiffHeader;

typedef struct {
  char chunk_id[4];
  int32_t chunksize;
} Chunk; 

typedef struct {
  int16_t wFormatTag;
  int16_t nChannels;
  int32_t nSamplesPerSec;
  int32_t nAvgBytesPerSec;
  int16_t nBlockAlign;
} WAVEFORMAT;

typedef struct {
//  WAVEFORMAT wf;
// new
  int16_t wFormatTag;
  int16_t nChannels;
  int32_t nSamplesPerSec;
  int32_t nAvgBytesPerSec;
  int16_t nBlockAlign;

  int16_t wBitsPerSample;
} PCMWAVEFORMAT;

typedef struct {
  RiffHeader header;
  Chunk ch_format;
  PCMWAVEFORMAT format;
  Chunk ch_data;
  // Data follows here.
} WaveFile;

void WaveFile_Init(WaveFile *wf);

void WaveFile_Set(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize);

void WaveFile_Fin(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize,
  long totalsamples);

short WaveFile_Write(const char *filename, WaveFile *wf, void *data);

short WaveFile_Read(const char *filename, WaveFile *wf, void **data);

float wave_length(WaveFile *wf);

#endif //_WAVEFILE_
