#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>




#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME)

#endif

#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100-volume)))


class Sound
{
private:
	LPDIRECTSOUNDBUFFER g_lpDSBG;

	LPDIRECTSOUND8  g_lpDS;
	BOOL            g_bPlay;

public:
	Sound()
	{
		g_lpDSBG = NULL;
		g_lpDS = NULL;
		g_bPlay = NULL;
	}

	BOOL CreateDirectSound(HWND hWnd);
	BOOL LoadWave(LPSTR lpFileName);
	BOOL SetVolume(LONG lVolume);
	BOOL SetPan(LONG lPan);
	~Sound() {};

	void DeleteDirectSound();
	void Play(BOOL Loop);
	void Stop();

};