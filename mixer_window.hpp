#ifndef __MIXER_WINDOW_H__
#define __MIXER_WINDOW_H__

#include "mixer_window_interface.hpp"
#include "mixer_manager.hpp"
#include "mixer_panel.hpp"

#include <tuple>
#include <memory>
#include <ncurses.h>

class MixerWindow : MixerWindowInterface {
    protected:
        static const int DIR_LEFT  = -1;
        static const int DIR_RIGHT = 1;
        static const int VOL_UP    = 1;
        static const int VOL_DOWN  = 2;

        static const int PAD_HEIGHT_VIEWPORT = 3;
        static const int PAD_WIDTH_VIEWPORT = 6;

        static const std::string IND_LEFT;
        static const std::string IND_RIGHT;

        uint width;
        uint height;
        uint curPanelPos;
        MixerManager mgr;
        std::shared_ptr<MixerPanel> curPanel;
        std::vector<MixerPanel> mixerPanels;
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
         * Draws all mixers to viewport
         */
        void drawMixers();

        /**
         * Resizes window/viewport 
         */
        void resize();

        /**
         * Selects mixer with given number
         */
        void selectMixer(int pos);

        /**
         * Mutes/Unmutes current mixer
         */
        void muteMixer();

        /**
         * Draws the scroll indicators
         */
        void drawScroller(int dir);

        /**
         * Scrolls viewport left/right to show current selected mixer
         */
        void scrollPanels(int dir);

        /**
         * In-/Decreases volume according to given direction
         *
         * @param dir Direction indicator
         */
        void adjustVolume(uint dir);

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

    public:
        /**
         * Listens to user input (arrow keys) and calls the appropriate method
         */
        void handleInput();
        /**
         * Std. constructor
         */
        MixerWindow(const MixerManager &mgr);

        /**
         * Std. destructor
         */
        ~MixerWindow();

        /**
         * Shows the mixer window
         */
        void show();

        /**
         * Returns current viewport size
         */
        std::tuple<uint, uint> getSize();
        WINDOW* getViewport();
};

#endif
