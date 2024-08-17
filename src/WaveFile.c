/*
        File: WaveFile.Cpp

        By Yung-hsiang Lee, Univ. of Southern California

        All Copyright reserved.

        Source file for .Wav File I/Os

        Distribution of the source file is permitted in original form.
          Modified version of the source file distribution is permitted
          if author's name is retained.

          Further modification or incorporation into other programs are 
          allowed without obtaining further agreement from author.

*/
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include "WaveFile.h"

void WaveFile_Init(WaveFile *wf)
{
  memcpy(wf->header.riff , (const void *)"RIFF", 4);
  memcpy(wf->header.rifftype, (const void *)"WAVE", 4);
  memcpy(wf->ch_format.chunk_id, (const void *)"fmt ", 4);
  wf->ch_format.chunksize=16;
//  wf->format.wf.wFormatTag=WAVE_FORMAT_PCM;
// new
  wf->format.wFormatTag=WAVE_FORMAT_PCM;
  memcpy(wf->ch_data.chunk_id, (const void *)"data", 4);
};

void WaveFile_Set(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize)
{
  WaveFile_Init(wf);
//  wf->format.wf.nChannels=channels;
//  wf->format.wf.nSamplesPerSec=samplerate;
// new
  wf->format.nChannels=channels;
  wf->format.nSamplesPerSec=samplerate;

// initially wrong
// wf->format.wf.nAvgBytesPerSec=samplerate;

//  wf->format.wf.nAvgBytesPerSec=samplerate*channels*datasize/8;
//  wf->format.wf.nBlockAlign=datasize*channels/8;
// new
  wf->format.nAvgBytesPerSec=samplerate*channels*datasize/8;
  wf->format.nBlockAlign=datasize*channels/8;

  wf->format.wBitsPerSample=datasize;
};

void WaveFile_Fin(WaveFile *wf,
  short channels,
  long samplerate,
  short datasize,
  long totalsamples)
{
  WaveFile_Set(wf, channels, samplerate, datasize);
//  wf->ch_data.chunksize=totalsamples * wf->format.wf.nBlockAlign;
// new
  wf->ch_data.chunksize=totalsamples * wf->format.nBlockAlign;

  wf->header.filesize=wf->ch_data.chunksize + 36;
};

short WaveFile_Write(const char *filename, WaveFile *wf, void *data)
{
  FILE *wfile;

  if( ( wfile=fopen(filename, "wb") )!=NULL ) {
    fwrite( wf, sizeof(WaveFile), 1, wfile);
    fwrite( data, wf->ch_data.chunksize , 1, wfile);
    fclose(wfile);
    return(1);
  } else {
//    printf("Error in opening the file!\n");
    return(0);
  }
}

float wave_length(WaveFile *wf)
/* Return wave length in secs. */
{
//  float len=(wf->ch_data.chunksize /
//    wf->format.wf.nChannels) /
//    (float)(wf->format.wf.nSamplesPerSec);
// new
  float len=(wf->ch_data.chunksize /
    wf->format.nChannels) /
    (float)(wf->format.nSamplesPerSec);

  return(len);
};

short WaveFile_Read(const char *filename, WaveFile *wf, void **data)
{
  FILE *wfile;

  if( ( wfile=fopen(filename, "rb") )!=NULL ) {
    fread( wf, sizeof(WaveFile), 1, wfile);
    *data=(char*)malloc(wf->ch_data.chunksize);
    fread( *data, wf->ch_data.chunksize , 1, wfile);
    fclose(wfile);
    return(1);
  } else {
//    printf("Error in opening the file!\n");
    return(0);
  }
}

