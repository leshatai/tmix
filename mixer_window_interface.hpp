#ifndef __MIXER_WINDOW_INTERFACE_H__
#define __MIXER_WINDOW_INTERFACE_H__

#include <tuple>

class MixerWindowInterface {
public:
    virtual std::tuple<uint, uint> getSize() = 0;
};

#endif
