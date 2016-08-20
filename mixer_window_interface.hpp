#ifndef __MIXER_WINDOW_INTERFACE_H__
#define __MIXER_WINDOW_INTERFACE_H__

#include <tuple>
#include <ncurses.h>

class MixerWindowInterface {
public:
    virtual std::tuple<uint, uint> getSize() = 0;
    virtual WINDOW* getViewport() = 0;
};

#endif
