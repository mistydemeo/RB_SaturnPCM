RB_SaturnPCM.EXE

Saturn PCM <-> Windows WAV
sound file conversion tool
for windows PC
by The Rockin'-B, www.Rockin-B.de
version 05/04/29



purpose:
--------------------------------------------------------------------------------
Converting .WAV(sound format for PC) <--> .PCM files(used on Saturn)
--> listen to sound from Saturn game CD's
--> possibility to use custom sounds in homebrew Saturn games!
--> patch games with custom sounds
--> make sounds for playback with my SoundPlayer for SEGA Saturn



features:
--------------------------------------------------------------------------------
    + converts .WAV files to .PCM and .C files
        - only wave files with PCM chunk,
          no type of encoding/compression supported
        + The pitch setting is automatically computed!
          Just an advice:
          For speach, use 8 bits mono with 11025Hz.
          For music, use 16 bits.
        - the generated .PCM files are not prepared to be placed into
          low workRAM for playback with SGL
    + converts .PCM files to .WAV files
        - further information must be provided on the command line,
          because raw PCM files don't contain any sound attributes
          (you may have to guess and try, if you don't know them)
    + use my SoundPLayer for SEGA Saturn to verify your sounds on real hardware
    + 'mem' optionally cuts down memory usage by half
      for conversion from WAV to PCM(like done on Saturn with SoundPlayer)
    + glitch on some files fixed


usage:
--------------------------------------------------------------------------------
    command line tool:
        
    PCM -> WAV conversion:
    SaturnPCM.EXE file.[pcm | snd | ton | WhatYouLike] [mono | stereo] [<bits>] [<sampleRate>]
    example: SaturnPCM.EXE sound.pcm mono 8 11025
    
    WAV -> PCM (+ C) conversion:
    SaturnPCM.EXE file.wav [c]
    example: SaturnPCM.EXE sound.wav c
    
    command line options:
    Options are optional, order doesn't matter.
    If an option is not supplied, default values are taken.
    The filename instead must be given.




Where to find .PCM files?
--------------------------------------------------------------------------------
Panzer Dragoon Saga
The most biggest amount of such files can be found there.
(sidenote: they are all mono, 16bits resolution, 22050Hz samplerate)
Big .STM files can be found on Shockwave assault. Try mono,
16 bits and 11025 Hz.

Myst
Got a lot of sound files with strange fileextension like .EN1, .EL2.
The files look like this SE00?.E?? and seem to be usual PCM files.

And if you find .SND or .TON files: they are a concatenation of various .PCM
files that can have different settings(channels, resolution, samplerate).
You can try to convert these files, too.
Find them on:
	Shinig Wisdom
	BUG

WARNING:
Road Rash
Got several .PCM files, but they are ADPCM encoded
(devnote: file header FORM???AIFFCOMM.........ADPCM)

I hope you enjoy it!
The Rockin'-B

http://www.rockin-b.de

