/*-
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) Copyright (c) 2016 Martin Weise
 */

#include <err.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <stdexcept>

#include "mixer_device.hpp"
#include "dragonfly_manager.hpp"

static const char *names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

DragonFlyManager::DragonFlyManager(std::string device) : MixerManager{device} {

    auto volLeft  = 0u;
    auto volRight = 0u;

    auto devmask = 0;
    auto recmask = 0;
    auto recsrc  = 0;
    auto bar     = 0;

    if ((this->deviceHndl = open(device.c_str(), O_RDWR)) < 0){
        throw std::runtime_error("Could not open device \""+device+"\".");
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_DEVMASK, &devmask) == -1){
        throw std::runtime_error("Could not read device mask.");
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_RECMASK, &recmask) == -1){
        throw std::runtime_error("Could not read recording device mask.");
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_RECSRC, &recsrc) == -1){
        throw std::runtime_error("Could not read recording device.");
    }
    //int orecsrc = recsrc;

    for(auto i=0u; i < SOUND_MIXER_NRDEVICES; i++){
        if (!((1 << i) & devmask))
            continue;
        if (ioctl(this->deviceHndl, MIXER_READ(i),&bar) == -1) {
            warn("MIXER_READ");
            continue;
        }
        volLeft  = bar & 0x7f;
        volRight = (bar >> 8) & 0x7f;

        MixerDevice dev{i, std::string(names[i]), volLeft, volRight};
        this->mixers.push_back(std::move(dev));
    }
}

void DragonFlyManager::updateMixer(MixerDevice &mixer){
    auto vol = mixer.getVolumeLeft() | (mixer.getVolumeRight() << 8);	
    if (ioctl(this->deviceHndl, MIXER_WRITE(mixer.getNumber()), &vol) == -1){
        warn("WRITE_MIXER");
    }
}
