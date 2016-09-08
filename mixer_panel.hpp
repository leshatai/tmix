#ifndef __MIXER_PANEL_H__
#define __MIXER_PANEL_H__

#include "mixer_device.hpp"

#include <ncurses.h>

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
        WINDOW *labelWindow;
        WINDOW *scaleWindow;

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
        void mute();
};

#endif
