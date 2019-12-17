#include "stdafx.h"
#include "VLCPlayer.h"

VLCPlayer::VLCPlayer(void)
{
	vlcInstance = NULL;
	vlcMediaPlayer = NULL;
	vlcMedia = NULL;
	isCaching = false;
	volume = 100;
	vlcInstance = libvlc_new(0, NULL);

}


VLCPlayer::~VLCPlayer(void)
{
	if (vlcMediaPlayer)
	{
		libvlc_media_player_release(vlcMediaPlayer);
		libvlc_release(vlcInstance);
	}

}
void VLCPlayer::getCacheLength() {
	m_cacheLength = libvlc_media_player_get_length(vlcMediaPlayer);
}
void VLCPlayer::getLength(int64_t len) {
	m_length = len;
}
void VLCPlayer::getTime() {
	m_time = libvlc_media_player_get_time(vlcMediaPlayer);
}

void VLCPlayer::getMediaPlayer(string path) {
	m_path = path;
	if (vlcInstance != NULL)
	{
		vlcMedia = libvlc_media_new_path(vlcInstance, path.c_str());
		if (vlcMedia != NULL)
		{
			vlcMediaPlayer = libvlc_media_player_new(vlcInstance);
			libvlc_media_player_set_media(vlcMediaPlayer, vlcMedia);
		}
	}

}
void VLCPlayer::onPlay() {
	libvlc_audio_set_volume(vlcMediaPlayer, volume);
	libvlc_media_player_play(vlcMediaPlayer);

}
void VLCPlayer::onPause() {
	libvlc_media_player_pause(vlcMediaPlayer);

}
void VLCPlayer::onStop() {
	libvlc_media_player_stop(vlcMediaPlayer);
}
// 设置音量
void VLCPlayer::setVolume(int vol)
{
	volume = vol;
	if (vlcMediaPlayer) {
		libvlc_audio_set_volume(vlcMediaPlayer, vol);
	}
}
// 获取播放状态
int VLCPlayer::getPlayState()
{
	libvlc_state_t res = libvlc_media_player_get_state(vlcMediaPlayer);
	return res;

}
