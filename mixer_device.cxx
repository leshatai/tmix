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

#include "mixer_device.hpp"

MixerDevice::MixerDevice(uint mixerNr, std::string name, uint volLeft, uint volRight) : 
    nr{mixerNr},
    name{std::move(name)},
    volLeft{volLeft},
    volRight{volRight},
    mutedLeft{false},
    mutedRight{false},
    muteVolLeft{volLeft},
    muteVolRight{volRight} {
    }

void MixerDevice::setVolume(uint vol){
    this->setVolume(vol, vol);
}

void MixerDevice::setVolume(uint left, uint right){
    this->volLeft = left > 100 ? 100 : left;
    this->volRight = right > 100 ? 100 : right;
}

void MixerDevice::incVolume(){
    return this->incVolume(1);
}

void MixerDevice::incVolume(uint vol){
    this->incVolume(vol, vol);
}

void MixerDevice::incVolume(uint left, uint right){
    this->setVolume(this->volLeft+left, this->volRight+right);
}

void MixerDevice::decVolume(){
    return this->decVolume(1);
}

void MixerDevice::decVolume(uint vol){
    this->decVolume(vol, vol);
}

void MixerDevice::decVolume(uint left, uint right){
    auto volLeft = this->volLeft - left;
    auto volRight = this->volRight - right;

    volLeft  = volLeft  == 0 ? 0 : volLeft;
    volRight = volRight == 0 ? 0 : volRight;

    this->setVolume(volLeft, volRight);
}

void MixerDevice::mute(){
    if (this->isMuted()){
        return;
    }

    this->muteVolLeft  = this->volLeft;
    this->muteVolRight = this->volRight;
    this->mutedLeft    = true;
    this->mutedRight   = true;

    this->setVolume(0);
}

void MixerDevice::muteLeft(){
    if (this->mutedLeft){
        return;
    }

    this->muteVolLeft = this->volLeft;
    this->mutedLeft   = true;
    this->setVolume(0, this->volRight);
}

void MixerDevice::muteRight(){
    if (this->mutedRight){
        return;
    }

    this->muteVolRight = this->volRight;
    this->mutedRight   = true;
    this->setVolume(this->volLeft, 0);
}

void MixerDevice::unmute(){
    if (!this->isMuted()){
        return;
    }

    this->setVolume(muteVolLeft, muteVolRight);
    this->mutedLeft  = false;
    this->mutedRight = false;
}

void MixerDevice::unmuteLeft() {
    if (!this->mutedLeft){
        return;
    }

    this->volLeft   = this->muteVolLeft;
    this->mutedLeft = false;
    this->setVolume(this->muteVolLeft, this->volRight);
}

void MixerDevice::unmuteRight() {
    if (!this->mutedRight){
        return;
    }

    this->volRight   = this->muteVolRight;
    this->mutedRight = false;
    this->setVolume(this->volLeft, this->muteVolRight);
}

