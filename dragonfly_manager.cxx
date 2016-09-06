#include <err.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include "mixer_device.hpp"
#include "dragonfly_manager.hpp"

static const char *names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

DragonFlyManager::DragonFlyManager(std::string device) : MixerManager{device} {

    uint volLeft  = 0;
    uint volRight = 0;

    int devmask = 0;
    int recmask = 0;
    int recsrc  = 0;
    int bar;

    if ((this->deviceHndl = open(device.c_str(), O_RDWR)) < 0){
        throw std::exception();
        //throw std::exception("Could not open device \""+device+"\".");
        //err(1, "%s", name);
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_DEVMASK, &devmask) == -1){
        throw std::exception();
        //throw std::exception("Could not read device mask.");
        //err(1, "SOUND_MIXER_READ_DEVMASK");
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_RECMASK, &recmask) == -1){
        throw std::exception();
        //throw std::exception("Could not read recording device mask.");
        //err(1, "SOUND_MIXER_READ_RECMASK");
    }
    if (ioctl(this->deviceHndl, SOUND_MIXER_READ_RECSRC, &recsrc) == -1){
        throw std::exception();
        //throw std::exception("Could not read recording device.");
        //err(1, "SOUND_MIXER_READ_RECSRC");
    }
    //int orecsrc = recsrc;

    for(uint i=0; i < SOUND_MIXER_NRDEVICES; i++){
        if (!((1 << i) & devmask))
            continue;
        if (ioctl(this->deviceHndl, MIXER_READ(i),&bar) == -1) {
            warn("MIXER_READ");
            continue;
        }
        volLeft  = bar & 0x7f;
        volRight = (bar >> 8) & 0x7f;

        MixerDevice dev{i, std::string(names[i]), volLeft, volRight};
        this->mixers.push_back(dev);
    }
}

void DragonFlyManager::updateMixer(MixerDevice &mixer){
    int vol = mixer.getVolumeLeft() | (mixer.getVolumeRight() << 8);	
    if (ioctl(this->deviceHndl, MIXER_WRITE(mixer.getNumber()), &vol) == -1){
        warn("WRITE_MIXER");
    }
}