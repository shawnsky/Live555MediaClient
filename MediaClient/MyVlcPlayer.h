/**
 * @file MyVlcPlayer.h
 *
 * Wrapper libvlc API
 *
 *
 * @author Chen xiaotian
 * @version 0.1 19/12/2019
 *
 */
#pragma once
#include "vlc.h"
#include <string>

#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")

using namespace std;

class MyVlcPlayer
{
private:
	libvlc_instance_t* vlc_ins = NULL;
	libvlc_media_player_t* vlc_player = NULL;
	libvlc_media_t* vlc_media = NULL;
	libvlc_time_t total_time;

	const char *  vlc_args[3] = {
			   "-I", "dummy", /* Don't use any interface */
			   "--ignore-config" /* Don't use VLC's config */
			   };

public:

	MyVlcPlayer()
	{

	}
	~MyVlcPlayer()
	{
		libvlc_media_release(vlc_media);
		libvlc_media_player_release(vlc_player);
		libvlc_release(vlc_ins);
	}

	bool Initialize(string path)
	{
		// Create VLC instance
		vlc_ins = libvlc_new(0, NULL);
		if (vlc_ins == NULL)
		{
			return false;
		}
		vlc_player = libvlc_media_player_new(vlc_ins);
		if (vlc_player == NULL)
		{
			return false;
		}
		// Set media file path
		const char *p = path.c_str();
		vlc_media = libvlc_media_new_path(vlc_ins, p);
		if (vlc_media == NULL)
		{
			return false;
		}
		// Parse media
		libvlc_media_parse(vlc_media);
		// Get total time in ms
		total_time = libvlc_media_get_duration(vlc_media);
		// Set media to player
		libvlc_media_player_set_media(vlc_player, vlc_media);

		return true;
	}

	void Play()
	{
		libvlc_media_player_play(vlc_player);
	}

	void Stop()
	{
		libvlc_media_player_stop(vlc_player);
	}

	libvlc_time_t GetTotalTime()
	{
		return total_time;
	}

	
};

