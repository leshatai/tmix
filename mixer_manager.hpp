#ifndef __MIXER_MANAGER_INTERFACE_H__
#define __MIXER_MANAGER_INTERFACE_H__

#include <string>
#include <vector>

#include "mixer_device.hpp"

class MixerManager {
    protected:
        std::vector<MixerDevice> mixers;
        std::string device;
    public:
        MixerManager(std::string device) : device(device) {}

        std::vector<MixerDevice> &getMixers() { return this->mixers; }
        uint getMixerCount() { return this->mixers.size(); }

        virtual void updateMixer(MixerDevice &mixer) = 0;
};

#endif
