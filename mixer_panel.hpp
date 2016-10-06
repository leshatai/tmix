/*
 * Class to render the mixer UI.
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

#ifndef __MIXER_PANEL_H__
#define __MIXER_PANEL_H__

#include "mixer_device.hpp"

#include <ncurses.h>
#include <memory>

class MixerPanel{
    protected:
        static const uint WIDTH_LABEL  = 7;
        static const uint WIDTH_SCALE  = 4;

        static const uint HEIGHT_LABEL = 3;
        static const uint HEIGHT_SCALE = 17;

        static const uint COLOR_PAIR_GREEN     = 20;
        static const uint COLOR_PAIR_RED       = 21;
        static const uint COLOR_PAIR_WHITE     = 22;
        static const uint COLOR_PAIR_HIGHLIGHT = 23;

        static const uint SCALE_LEFT  = 1;
        static const uint SCALE_RIGHT = 2;

        uint heightLabel;
        uint heightScale;

        uint pos;
        uint channel;
        MixerDevice &device;
        std::shared_ptr<WINDOW> labelWindow;
        std::shared_ptr<WINDOW> scaleWindow;

        /**
         * Calculates the size of all windows, depending
         * on the size of the main window
         */
        void calculateSizes(uint viewportHeight);
        bool isInitialized();
        void drawScale();
        void drawSingleScale(uint numLines, uint height, uint leftRight);
        /**
         * Returns the values for left/right channel to be used
         * to de-/increase volume depending on active channel
         * 
         * First value is left, second value is right
         */
        std::pair<uint, uint> getChannelOffsets();

    public:
        static const uint WIDTH   = 10;

        static const uint CHANNEL_LEFT  = 1;
        static const uint CHANNEL_RIGHT = 2;
        static const uint CHANNEL_BOTH  = 3;

        MixerPanel(uint pos, MixerDevice &device);
        ~MixerPanel();
        MixerDevice& getMixer();
        void init(WINDOW &viewport);
        void highlight();
        void drawLabel();
        void draw();
        void draw(uint viewportHeight);
        void resize(uint viewportHeight);
        void toogleChannel(uint channel);

        void decreaseVolume();
        void increaseVolume();
        void alignVolume();
        void mute();
};

#endif
