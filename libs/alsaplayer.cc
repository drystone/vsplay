/**
 * alsaplayer.cc (c) 2004 John Hedges
 *
 * Play raw data to alsa device
 *
 * Based on rawplayer.cc (c) 1997 Jung woo-jae
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2.1 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *   License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software Foundation,
 *   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ALSA

#include <alsa/asoundlib.h>
#include <iostream>

#include "mpegsound.h"

class Alsa_error {
public:
    int errnum;
    Alsa_error(int e) : errnum(e) { if (errnum != -EAGAIN) std::cout << "alsa error: " << errnum << std::endl; };
};

inline int alsa_throw(int result) throw(Alsa_error) {
    if (result < 0) throw Alsa_error(result);
    return result;
}

Rawplayeralsa::Rawplayeralsa(const char * devicename)
    throw (Vsplayexception)
    : _device_handle(0)
    , _hw_params(0)
{
    try
    {
        // allocate the hw_params structure, open the alsa device and get its capabilities
        alsa_throw(snd_pcm_hw_params_malloc(&_hw_params));
        alsa_throw(snd_pcm_open(&_device_handle, devicename, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK));
        alsa_throw(snd_pcm_hw_params_any(_device_handle, _hw_params));
    }
    catch (Alsa_error &e)
    {
        if (_hw_params) snd_pcm_hw_params_free(_hw_params);
        if (_device_handle) snd_pcm_close(_device_handle);
        throw Vsplayexception(SOUND_ERROR_DEVOPENFAIL);
    }
}
  
Rawplayeralsa::~Rawplayeralsa()
{
    if (_device_handle) snd_pcm_close(_device_handle);
    if (_hw_params) snd_pcm_hw_params_free(_hw_params);
}

void Rawplayeralsa::setsoundtype(int stereo, int samplesize, int speed)
  throw (Vsplayexception)
{
    snd_pcm_format_t format = (samplesize == 16) ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_UNKNOWN;
    int channels = stereo ? 2 : 1;

    _framesize = channels << 1;
    try
    {
        alsa_throw(snd_pcm_hw_params_set_access(_device_handle, _hw_params, SND_PCM_ACCESS_RW_INTERLEAVED));
        alsa_throw(snd_pcm_hw_params_set_channels(_device_handle, _hw_params, channels));
        alsa_throw(snd_pcm_hw_params_set_format(_device_handle, _hw_params, format));
        alsa_throw(snd_pcm_hw_params_set_rate(_device_handle, _hw_params, speed, 0));
        alsa_throw(snd_pcm_hw_params(_device_handle, _hw_params));
        alsa_throw(snd_pcm_prepare(_device_handle));
    }
    catch (Alsa_error &e)
    {
        throw Vsplayexception(SOUND_ERROR_DEVCTRLERROR);
    }
}

void Rawplayeralsa::putblock(void *buffer, int size)
  throw (Vsplayexception)
{
    snd_pcm_uframes_t frames = size / _framesize;
    while (frames)
    {
        try {
            if (alsa_throw(snd_pcm_wait(_device_handle, 100)))
            {
                void * pdata = (char*)buffer + size - frames * _framesize;
                frames -= alsa_throw(snd_pcm_writei(_device_handle, pdata, frames));
            }
        }
        catch (Alsa_error &e)
        {
            switch (e.errnum)
            {
            case -EAGAIN:
                continue;
            case -EPIPE:
                if (snd_pcm_recover(_device_handle, -EPIPE, 1) < 0)
                    throw Vsplayexception(SOUND_ERROR_DEVCTRLERROR);
                break;
            default:
                throw Vsplayexception(SOUND_ERROR_DEVWRITEFAIL);
            };
        }
    }
}

#endif // ALSA
