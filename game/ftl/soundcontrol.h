#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

DECLSYM(SoundControl_Sounds);
#define SCSounds ((SoundControl*)symAddr(ftlbase, SoundControl_Sounds))

// takes a vector of basic_strings
typedef void (*FUNCTYPE(SoundControl_StartPlaylist))(SoundControl* self, vector* newPlayList);
DECLFUNC(SoundControl_StartPlaylist);
#define SoundControl_StartPlaylist(self, newPlayList) \
    FCALL(ftlbase, SoundControl_StartPlaylist, self, newPlayList)

typedef unsigned int (*FUNCTYPE(SoundControl_PlaySoundMix))(SoundControl* this, basic_string* name,
                                                            float volume, bool loop);
DECLFUNC(SoundControl_PlaySoundMix);
#define SoundControl_PlaySoundMix(self, name, volume, loop) \
    FCALL(ftlbase, SoundControl_PlaySoundMix, self, name, volume, loop)
