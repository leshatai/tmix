#include "mixer_device.hpp"

MixerDevice::MixerDevice(uint mixerNr, std::string name, uint volLeft, uint volRight) : 
    nr(mixerNr),
    name(name),
    volLeft(volLeft),
    volRight(volRight),
    muted(false),
    muteVolLeft(volLeft),
    muteVolRight(volRight) {
    }

void MixerDevice::setVolume(uint vol){
    this->setVolume(vol, vol);
}

void MixerDevice::setVolume(uint left, uint right){
    this->volLeft = left > 100 ? 100 : left;
    this->volRight = right > 100 ? 100 : right;
}

void MixerDevice::incVolume(){
    return this->incVolume(1);
}

void MixerDevice::incVolume(uint vol){
    this->incVolume(vol, vol);
}

void MixerDevice::incVolume(uint left, uint right){
    this->setVolume(this->volLeft+left, this->volRight+right);
}

void MixerDevice::decVolume(){
    return this->decVolume(1);
}

void MixerDevice::decVolume(uint vol){
    this->decVolume(vol, vol);
}

void MixerDevice::decVolume(uint left, uint right){
    int volLeft = this->volLeft - left;
    int volRight = this->volRight - right;

    volLeft  = volLeft  < 0 ? 0 : volLeft;
    volRight = volRight < 0 ? 0 : volRight;

    this->setVolume(volLeft, volRight);
}

bool MixerDevice::isMuted(){
    return this->muted;
}

void MixerDevice::mute(){
    if (this->muted){
        return;
    }

    this->muteVolLeft  = this->volLeft;
    this->muteVolRight = this->volRight;
    this->muted      = true;

    this->setVolume(0);
}

void MixerDevice::unmute(){
    if (!this->muted){
        return;
    }

    this->setVolume(muteVolLeft, muteVolRight);
    this->muted = false;
}
