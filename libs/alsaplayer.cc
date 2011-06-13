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

Rawplayeralsa::Rawplayeralsa()
      : _device_handle(0)
      , _hw_params(0)
{
}
  
Rawplayeralsa::~Rawplayeralsa()
{
    if (_device_handle) snd_pcm_close(_device_handle);
    snd_pcm_hw_params_free(_hw_params);
}

bool
Rawplayeralsa::initialize(const char* filename)
{
    int alsa_error;
    
    // allocate the hw_params structure, open the alsa device and get its capabilities
    alsa_error = snd_pcm_hw_params_malloc(&_hw_params);
    if (!alsa_error) alsa_error = snd_pcm_open(&_device_handle, filename, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (!alsa_error) alsa_error = snd_pcm_hw_params_any(_device_handle, _hw_params);
    if (alsa_error) return seterrorcode(SOUND_ERROR_DEVOPENFAIL);

    return true;
}

bool 
Rawplayeralsa::setsoundtype(int stereo,int samplesize,int speed)
{
    int alsa_error;
    snd_pcm_format_t format = (samplesize == 16) ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_UNKNOWN;
    _channels = stereo ? 2 : 1;
    alsa_error = snd_pcm_hw_params_set_access(_device_handle, _hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (!alsa_error) alsa_error = snd_pcm_hw_params_set_channels(_device_handle, _hw_params, _channels);
    if (!alsa_error) alsa_error = snd_pcm_hw_params_set_format(_device_handle, _hw_params, format);
    if (!alsa_error) alsa_error = snd_pcm_hw_params_set_rate(_device_handle, _hw_params, speed, 0);
    if (!alsa_error) alsa_error = snd_pcm_hw_params(_device_handle, _hw_params);
    if (!alsa_error) alsa_error = snd_pcm_prepare(_device_handle);
    if (alsa_error) seterrorcode(SOUND_ERROR_DEVCTRLERROR);
    return (alsa_error == 0) ? true : false;
}

bool
Rawplayeralsa::putblock(void *buffer, int size)
{
    bool error = false;
    unsigned int framesize = (_channels == 2 ? 4 : 2);
    int remaining = size/framesize;
    char *buf = (char*)buffer;
    
    while (remaining)
    {
        snd_pcm_wait(_device_handle, 1000);
        int result = snd_pcm_writei(_device_handle, buf, remaining);
        if (result == -EAGAIN)
        {   // write won't fit - buffer must be full
            continue;
        }
        else if (result == -EPIPE)
        {   // underrun
            std::cerr << "underrun" << std::endl;
            continue;
        }
        else if (result < 0)
        {
            error = true;
            break;
        }
        remaining -= result;
        buf += result * framesize;
    }
    return !error;
}

void
Rawplayeralsa::abort(void)
{
    snd_pcm_reset(_device_handle);
}

#endif // ALSA
