#ifndef __MIXER_PANEL_H__
#define __MIXER_PANEL_H__

#include "mixer_device.hpp"
#include "mixer_window_interface.hpp"

#include <ncurses.h>
#include <memory>

class MixerPanel{
    protected:
        static const uint WIDTH_LABEL  = 7;
        static const uint WIDTH_SCALE  = 4;

        static const uint HEIGHT_LABEL = 3;
        static const uint HEIGHT_MAIN  = 20;
        static const uint HEIGHT_SCALE = 17;

        static const uint COLOR_PAIR_GREEN       = 20;
        static const uint COLOR_PAIR_RED         = 21;
        static const uint COLOR_PAIR_WHITE       = 22;
        static const uint COLOR_PAIR_HIGHLIGHT   = 23;

        static const uint SCALE_LEFT  = 1;
        static const uint SCALE_RIGHT = 2;

        uint heightMain;
        uint heightLabel;
        uint heightScale;

        uint pos;
        MixerDevice &device;
        WINDOW *mainWindow;
        WINDOW *labelWindow;
        WINDOW *scaleWindow;
        MixerWindowInterface &window;

        /**
         * Calculates the size of all windows, depending
         * on the size of the main window
         */
        void calculateSizes();
        bool isInitialized();
        void initPanel();
        void drawLabel();
        void drawScale();
        void drawSingleScale(uint numLines, uint height, uint leftRight);

    public:
        static const uint WIDTH_MAIN   = 10;

        MixerPanel(uint pos, MixerDevice &device, MixerWindowInterface &window);
        ~MixerPanel();
        MixerDevice& getMixer();
        void highlight();
        void draw();
        void resize();

        void decreaseVolume();
        void increaseVolume();
        void mute();
};

#endif
