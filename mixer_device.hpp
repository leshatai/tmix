/*
 * Class to store volume settings of a mixer
 *
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

#ifndef __MIXER_DEVICE_H__
#define __MIXER_DEVICE_H__

#include <string>

typedef unsigned int uint;

class MixerDevice {
    protected:
        uint nr;
        std::string name;
        uint volLeft;
        uint volRight;

        bool mutedLeft;
        bool mutedRight;
        uint muteVolLeft;
        uint muteVolRight;
    public:
        MixerDevice(uint mixerNr, std::string name, uint volLeft, uint volRight);

        uint getNumber() { return nr; }
        std::string getName() { return name; }
        uint getVolumeLeft() { return volLeft; }
        uint getVolumeRight() { return volRight; }
        std::pair<uint, uint> getVolume() {  return std::make_pair(volLeft, volRight); }

        void setVolume(uint vol);
        void setVolume(uint left, uint right);

        void incVolume();
        void incVolume(uint step);
        void incVolume(uint left, uint right);

        void decVolume();
        void decVolume(uint step);
        void decVolume(uint left, uint right);

        bool isMuted() { return (mutedLeft && mutedRight); }
        bool isMutedLeft() { return mutedLeft; }
        bool isMutedRight() { return mutedRight; }

        void mute();
        void muteLeft();
        void muteRight();

        void unmute();
        void unmuteLeft();
        void unmuteRight();
};

#endif
