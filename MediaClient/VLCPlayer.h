#pragma once
#include "vlc.h"
#include <string>

#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")

using namespace std;

class VLCPlayer
{
private:


public:
	libvlc_instance_t* vlcInstance;
	libvlc_media_player_t* vlcMediaPlayer;
	libvlc_media_t* vlcMedia;
	string m_path;
	int64_t m_length;
	int64_t m_cacheLength;
	int64_t m_time;
	bool isCaching;
	int volume;
	void getCacheLength();
	void getTime();
	void getMediaPlayer(string path);
	void getLength(int64_t len);
	void onPlay();
	void onPause();
	void onStop();
	void setVolume(int vol);
	int getPlayState();
	VLCPlayer(void);
	~VLCPlayer(void);
};

