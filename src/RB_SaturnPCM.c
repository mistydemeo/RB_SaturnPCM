#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <dir.h>

#include "WaveFile.h"
#include "RB_RETVAL.h"
#include "RB_PCM.h"


// ERRORS:
// converting and reconverting don't lead to a binary identic file
//  ? testSign() and testMux() don't report errors
// -> fixed: 44 offset
//
// bubble sort on one buffer
// PCM->WAV is OK
// WAV->PCM got every byte wrong
//  ? maybe because to contains wav header now ?
// -> fixed in this app

#define version "050429"
// attention: as the data is a byte array,
// you should care when accessing the WaveFile data structures
// on machines which don't allow unaligned access
// like Saturn
// ? does malloc return aligned memory?

typedef enum {
    NONE,
    UNKNOWN,
    PCM,
    SND,
    WAV
} fileType;





int writeC(char *fname, unsigned char *data, unsigned int length, unsigned char stereo, unsigned char bits, unsigned short pitch)
{
    FILE *file;
    int i, j;
    char *vname = (char *)strdup(fname);

    *((char *)strrchr(vname, '.')) = '\0';

    if((file = fopen(fname, "w")) == NULL) {
        printf("Error opening file %s!\n", fname);
        return -1;
    }
        
    fprintf(file, "// Built with RB_SaturnPCM.exe,\n// a .WAV <-> .PCM conversion tool to create SEGA Saturn sound FX.\n// Check http://www.Rockin-B.de for other console dev stuff.\n\n");
    fprintf(file, "#include    \"sl_def.h\"\n\nchar %s[] = {\n", vname);
    for(i = 0,j = 0; i < length; i++) {
        if(j == 16) {
            j = 0;
            fprintf(file, "\n");
        }
        j++;
        fprintf(file, "0x%02X,", *(data + i));
    }
    // PCM_DATA
    fprintf(file, "} ;\n\nUint32 %s_size = sizeof(%s);\n\nPCM %s_dat = {(%s | %s) , 0 , 127 , 0 , 0x%x , 0 , 0 , 0 , 0} ;\n", vname, vname, vname, (stereo == 0 ? "_Mono" : "_Stereo"), (bits == 8 ? "_PCM8Bit" : "_PCM16Bit"), pitch);

    if(fclose(file) != 0) {
        printf("Error closing file %s!\n", fname);
        return -1;
    }

    printf("File %s successfully written!\n", fname);
    return 1;
}

int writeData(char *fname, unsigned char *data, unsigned int length)
{
    FILE *file;
    int temp;
    
    if((file = fopen(fname, "wb")) == NULL) {
        printf("Error opening file %s!\n", fname);
        return -1;
    }
    temp = fwrite(data, sizeof(unsigned char), length, file);
    if(temp != length) {
        printf("Error while writing file %s!\n", fname);
        printf("Only %i of %i bytes written!\n", temp, length);
        return -1;
    }
    if(fclose(file) != 0) {
        printf("Error closing file %s!\n", fname);
        return -1;
    }

    printf("File %s successfully written!\n", fname);
    return 1;
}


void WaveFile_Get(WaveFile *wf, unsigned char *data)
{
    memcpy(wf, data, sizeof(WaveFile));
    // point to first chunk
//    *data = *data + (sizeof(WaveFile) / sizeof(unsigned char));
    
    // now perform endianess corrections
}




// debug tests
extern void PCM_toUnsigned(unsigned char *data, int length);
extern void PCM_toSigned(unsigned char *data, int length);
extern ReturnValue PCM_muxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits);
extern ReturnValue PCM_demuxChannels(unsigned char *to, unsigned char *from, unsigned int length, unsigned char bits);

void testSign()
{
    unsigned char b[256], bu[256], bu2[256], bs[256], bs2[256];
    int i, eu, es;
    
    printf("testing signed <-> unsigned conversion\n");
    // generate test data
    for(i = 0; i < 256; i++) {
        b[i] = bu[i] = bs[i] = (unsigned char)i;
    }
    // convert
    PCM_toUnsigned(bu, 256);
    PCM_toSigned(bs, 256);
    // re-convert
    memcpy(bu2, bu, 256);
    memcpy(bs2, bs, 256);
    PCM_toSigned(bu2, 256);
    PCM_toUnsigned(bs2, 256);
    // print results
    eu = es = 0;
    for(i = 0; i < 256; i++) {
        printf("b %i, bu %i, bu2 %i; bs %i, bs2 %i\n", b[i], bu[i], bu2[i], bs[i], bs2[i]);
        // count errors
        if(b[i] != bu2[i])
            eu++;
        if(b[i] != bs2[i])
            es++;
    }
    printf("ERRORS: unsigned %i, signed %i\n", eu, es);
}

void testMux()
{
    unsigned char b[256], bm[256], bd[256];
    unsigned short w[256], wm[256], wd[256];
    int i, eb, ew;
    ReturnValue ret;
    
    printf("testing mux <-> demux conversion\n");
    // generate test data
    for(i = 0; i < 256; i++) {
        b[i] = (unsigned char)i;
        w[i] = (unsigned short)(i + ((255 - i) << 8));
    }
    // convert
    ret = PCM_muxChannels(bm, b, 256, 8);
    if(ret != RETURN_OK)
        printf("ERROR converting data!\n");
        
    ret = PCM_muxChannels((unsigned char *)wm, (unsigned char *)w, 256*2, 16);
    if(ret != RETURN_OK)
        printf("ERROR converting data!\n");
    // re-convert
    ret = PCM_demuxChannels(bd, bm, 256, 8);
    if(ret != RETURN_OK)
        printf("ERROR converting data!\n");
        
    ret = PCM_demuxChannels((unsigned char *)wd, (unsigned char *)wm, 256*2, 16);
    if(ret != RETURN_OK)
        printf("ERROR converting data!\n");
    // print results
    eb = ew = 0;
    for(i = 0; i < 256; i++) {
        printf("b %x, bd %x; w %x, wd %x\n", b[i], bd[i], w[i], wd[i]);
        // count errors
        if(b[i] != bd[i])
            eb++;
        if(w[i] != wd[i])
            ew++;
    }
    printf("ERRORS: byte %i, word %i\n", eb, ew);
}

extern void PCM_demuxQSortB(unsigned char *data, int l, int r);
extern void PCM_demuxQSort2B(unsigned char *data, int l, int r);
#define QSBL        16
int testDemux4(unsigned int length)
{
    unsigned char b[length];
    unsigned int l, r, o;
    int errors = 0;
    
//    printf("Testing demux for %i bytes\n", length);
    // write wav format
    for(o = 0, l = length/2, r = 0; r < (length/2); o += 2, l++, r++) {
        b[o] = l;
        b[o+1] = r;
//        printf("wav l %i\n", l);
//        printf("wav r %i\n", r);
    }
    // sort to Saturn PCM format
    PCM_demuxQSort2B(b, 0, length-1);
    // print result
        // right channel
    for(r = 0; r < (length/2); r++) {
//        printf("pcm r %i: %i\n", r, b[r]);
        if(((unsigned char)r) != b[r])
            errors++;
    }    
        // left channel
    for(l = 0; l < (length/2); l++) {
//        printf("pcm l %i: %i\n", (length/2)+l, b[(length/2)+l]);
        if( ((unsigned char)((length/2)+l)) != b[(length/2)+l])
            errors++;
    }    

    printf("Finished %i bytes with %i errors!\n", length, errors);
    
    if(errors > 0)
        return 0;
    else return 1;
}



int main(int argc, char *argv[])
{
    // options to set with command line
    char *fname = NULL;
    unsigned char fname_length = 0;
    fileType ftype = NONE;
    unsigned char C = 0;
    unsigned char stereo = 0;
    unsigned char bits = 8;
    unsigned short sampleRate = 11025;
    // debug
    unsigned char mem = 0;
    
    // stuff set on my own
    WaveFile waveFile;
    unsigned char *to = NULL;
    unsigned char *from = NULL;
    unsigned int length = 0;
    int i, temp;
    unsigned short pitch;
    
    // file stuff
    int filehandle;
    FILE *file;
    
#if 0
//    if(argc > 1)
{
    int i, errors = 0;
    
    for(i = 2; i <= 65536; i += 2) {
        if(testDemux4(i) != 1)
            errors++;
    }
    printf("Finished all with %i error test results!\n", errors);
}
//    testSign();
//    testMux();
    return 0;
#endif

    printf(
#       include "RB_license.h"
    );
    printf("\n\n\tRB_SaturnPCM.EXE\n\ta Wave(Windows) <-> PCM(SEGA Saturn)\n\tconversion utility by The Rockin'-B, www.rockin-b.de\n\n");
    printf("\tversion %s\n", version);
    printf("\tUsage:\n");
    printf("\tSaturnPCM.EXE file.pcm [mono | stereo] [<bits>] [<sampleRate>] [mem]\n");
    printf("\tSaturnPCM.EXE file.wav [c]\n\n");
    printf("\tOptions are optional, order doesn't matter.\n");
    printf("\tIf an option is not supplied, default values are taken.\n");
    printf("\tThe filename instead must be given.\n\n");
    printf("\tExamples:\n");
    printf("\tSaturnPCM.EXE sound.pcm mono 8 11025\n");
    printf("\tSaturnPCM.EXE sound.wav c\n\n");
    
    if(argc == 1) {
        system("PAUSE");
    }    
        
    // read in the commandline
    // expect:
    //  - filename of the source
    //  - keyword mono or stereo
    //  - bit resolution
    //  - samplerate
    for(i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "C") || !strcmp(argv[i], "c"))
            C = 1;
        else if(!strcmp(argv[i], "mem"))
            mem = 1;
        else if(!strcmp(argv[i], "mono"))
            stereo = 0;
        else if(!strcmp(argv[i], "stereo"))
            stereo = 1;
        // check if it is a filename
        else if((strstr(argv[i], ".wav") != NULL) || (strstr(argv[i], ".WAV") != NULL)) {
            fname = argv[i];
            ftype = WAV; 
        } else if((strstr(argv[i], ".pcm") != NULL) || (strstr(argv[i], ".PCM") != NULL)) {
            // normal PCM
            fname = argv[i];
            ftype = PCM; 
        } else if((strstr(argv[i], ".snd") != NULL) || (strstr(argv[i], ".SND") != NULL) || (strstr(argv[i], ".ton") != NULL) || (strstr(argv[i], ".TON") != NULL)) {
            // multiple PCM
            fname = argv[i];
            ftype = SND; 
            printf( "WARNING: this file may contain multiple sounds\n"
                    "which can have different attributes\n"
                    "(samplerate, mono/stereo, 8/16 bit resolution)\n"
                    "so it may sound wierd!\n\n");            
        } else if(strstr(argv[i], ".") != NULL) {
            // unknown file extension 
            fname = argv[i];
            ftype = UNKNOWN; 
            printf( "WARNING: unknown file extension!\n"
                    "Will treat it as a pcm file,\n"
                    "result is undefined!\n\n");            
        } else {
            temp = (int)atoi(argv[i]);
            if((temp == 8) || (temp == 16))
                bits = temp;
            else if((temp > 1000) && (temp < 100000))
                sampleRate = temp;
        }
    }

    // open and read the file
    if((fname == NULL) || (ftype == NONE)) {
        printf("Wrong filename!\n");
        return -1;
    }
    fname_length = strlen(fname);    

    if((file = fopen(fname, "rb")) == NULL) {
        printf("Error opening file %s!\n", fname);
        return -1;
    }

    /* need to find out file size of source file */
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 9, SEEK_SET);

    if((from = (void *)malloc(size)) == NULL) {
        printf("Not enough memory to read file!\n");
        return -1;
    }
//    printf("Now reading %i bytes of file %s!\n", size, fname);
    /* then read in the data */
    temp = fread(from, sizeof(unsigned char), size, file);
    if(temp != size) {
        printf("Error while reading file %s!\n", fname);
        printf("Only %i of %i bytes read!\n", temp, size);
        return -1;
    }
    if(fclose(file) != 0) {
        printf("Error closing file %s!\n", fname);
        return -1;
    }
    // only wav conversion on one buffer
    if((ftype != WAV) || (mem == 0)) {  
        if((to = (void *)malloc(size)) == NULL) {
            printf("Not enough memory to convert file!\n");
            return -1;
        }
    } else to = from;    
    
    
    // print the used options:
    switch(ftype) {
        case UNKNOWN:
        case SND:
        case PCM:
            printf("Performing PCM -> WAV conversion\n");
            printf("on file %s.\n", fname);
            printf("Options taken are:\n");
            printf("%s, %ibit resolution, %i samples per second\n", (stereo == 0 ? "mono" : "stereo"), bits, sampleRate);
            
            // convert
            length = size;
            if(PCM_toWAV(to, from, length, stereo, bits) != RETURN_OK) {
                printf("Error during data conversion!\n");
                return -1;
            }
//            printf("Conversion complete:\n");
            // write
            /* got all necessary data */
            WaveFile_Fin(&waveFile, (stereo == 1 ? 2 : 1), sampleRate, bits, size/((stereo == 1 ? 2 : 1) * bits / 8));    
            memcpy(fname + fname_length - 3, "wav", 3);
//            printf("Writing Wave:\n");
            if(WaveFile_Write(fname, &waveFile, to))
                printf("File %s successfully written!\n", fname);
            else printf("Error writing file %s!\n", fname);
                                  
            break;

        case WAV:
            printf("Performing WAV -> PCM%s conversion\n", (C == 0 ? "" : "+C"));
            printf("on file %s.\n", fname);
#if 0       
            // get sound type data from wave file
            WaveFile_Get(&waveFile, from);
            from += 44; // skip to first chunk
            
            /* print out the wav file specs */
            printf("channels: %i\n", waveFile.format.nChannels);
            printf("resolution: %i\n", waveFile.format.wBitsPerSample);
            printf("samplerate: %i\n", waveFile.format.nSamplesPerSec);
            printf("bytes: %i\n", waveFile.ch_data.chunksize);
            
            stereo = (waveFile.format.nChannels == 2);
            bits = waveFile.format.wBitsPerSample;
            sampleRate = waveFile.format.nSamplesPerSec;
            /* wrong size error message 
             * solve problems with win98 audio recorder */
            if(waveFile.ch_data.chunksize > (size - 44)) {
                printf("Wrong length in wavefile, taking real file size!\n");
                length = size - 44;
            } else
                length = waveFile.ch_data.chunksize;
            
            // convert
            if(PCM_toPCM(to, from, length, stereo, bits) != RETURN_OK) {
                printf("Error during data conversion!\n");
                return -1;
            }
#else
            length = size;
            if(PCM_toPCM(to, from, &length, &pitch, &stereo, &bits, 0) != RETURN_OK) {
                printf("Error during data conversion!\n");
                return -1;
            }
            /* print out the wav file specs */
            printf("channels: %i\n", (stereo == 0 ? 1 : 2));
            printf("resolution: %i\n", bits);
            printf("samplerate: %i\n", PCM_computeSampleRate(pitch));
            printf("bytes: %i\n", length);
#endif 
            // write PCM
            if(mem == 1)
                to = from + 44;
            memcpy(fname + fname_length - 3, "pcm", 3);
            writeData(fname, to, length);
            // write C
            memcpy(fname + fname_length - 3, "c\0", 2);
            if(C == 1)
                writeC(fname, to, length, stereo, bits, pitch);
                
            break;
    }

    
    return 1;
}
