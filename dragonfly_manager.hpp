#ifndef __DRAGONFLY_MIXER_MANAGER_H__
#define __DRAGONFLY_MIXER_MANAGER_H__

#include "mixer_manager.hpp"

class DragonFlyManager : public MixerManager {
    private:
        int deviceHndl;
    public:
        DragonFlyManager(std::string dev);
        void updateMixer(MixerDevice &mixer);
};

#endif
