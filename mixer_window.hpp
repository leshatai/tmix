/*
 * Class to handle UI rendering and input.
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

#ifndef __MIXER_WINDOW_H__
#define __MIXER_WINDOW_H__

#include "mixer_manager.hpp"
#include "mixer_panel.hpp"

#include <tuple>
#include <memory>
#include <ncurses.h>

class MixerWindow {
    protected:
        static const int DIR_LEFT  = -1;
        static const int DIR_RIGHT = 1;
        static const int VOL_UP    = 1;
        static const int VOL_DOWN  = 2;

        static const int PAD_HEIGHT_VIEWPORT = 3;
        static const int PAD_WIDTH_VIEWPORT  = 6;

        static const std::string IND_LEFT;
        static const std::string IND_RIGHT;
        static const std::string BLANK;

        uint width;
        uint height;
        uint curPanelPos;
        MixerManager &mgr;
        std::shared_ptr<MixerPanel> curPanel;
        std::vector<std::shared_ptr<MixerPanel>> mixerPanels;
        /** lower boundary in view port */
        uint minPanelPos;
        /** upper boundary in view port */
        uint maxPanelPos;
        WINDOW *viewport;

        /**
         * Initializes ncurses window and mixer panels
         */
        void init();

        /**
         * Initializes all mixer devices
         */
        void initMixers();

        /**
         * Returns the page of the currently selected panel
         */
        uint getCurrentPage();

        /**
         * Returns the number of panel pages
         */
        uint getNumPages();

        /**
         * Resizes window/viewport 
         */
        void resize();

        /**
         * Selects mixer with given number
         */
        void selectMixer(uint pos);

        /**
         * Mutes/Unmutes current mixer
         */
        void muteMixer();

        /**
         * Draws the scroll indicators
         */
        void drawScroller(int dir, bool remove);

        /**
         * Scrolls viewport left/right to show current selected mixer
         */
        void scrollPanels(int dir, bool pagewise);

        /**
         * In-/Decreases volume according to given direction
         *
         * @param dir Direction indicator
         */
        void adjustVolume(uint dir);

        /**
         * Aligns volume of left/right channel to the louder one
         */
        void alignVolume();

        /**
         * Returns the number of panels that would fit in current window
         */
        uint getNumVisiblePanels();

        /**
         * Refreshes the viewport to current window size
         */
        void updateViewport();

        /**
         * Shows/hides scroll left/right indicators 
         */
        void updateScrollers();

        /**
         * Makes changes to mixer volume to specific channel
         */
        void toogleMixerChannel(uint channel);

        /**
         * Listens to user input (arrow keys) and calls the appropriate method
         */
        void handleInput();

    public:
        /**
         * Std. constructor
         */
        MixerWindow(MixerManager &mgr);

        /**
         * Std. destructor
         */
        ~MixerWindow();

        /**
         * Shows the mixer window
         */
        void show();
};

#endif
