#ifndef __MIXER_DEVICE_H__
#define __MIXER_DEVICE_H__

#include <string>

class MixerDevice {
    protected:
        uint nr;
        std::string name;
        uint volLeft;
        uint volRight;

        bool mutedLeft;
        bool mutedRight;
        uint muteVolLeft;
        uint muteVolRight;
    public:
        MixerDevice(uint mixerNr, std::string name, uint volLeft, uint volRight);

        uint getNumber() { return nr; }
        std::string getName() { return name; }
        uint getVolumeLeft() { return volLeft; }
        uint getVolumeRight() { return volRight; }
        std::pair<uint, uint> getVolume();

        void setVolume(uint vol);
        void setVolume(uint left, uint right);

        void incVolume();
        void incVolume(uint step);
        void incVolume(uint left, uint right);

        void decVolume();
        void decVolume(uint step);
        void decVolume(uint left, uint right);

        bool isMuted();
        bool isMutedLeft();
        bool isMutedRight();

        void mute();
        void muteLeft();
        void muteRight();

        void unmute();
        void unmuteLeft();
        void unmuteRight();
};

#endif
