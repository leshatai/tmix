#include "mixer_device.hpp"

MixerDevice::MixerDevice(uint mixerNr, std::string name, uint volLeft, uint volRight) : 
    nr(mixerNr),
    name(name),
    volLeft(volLeft),
    volRight(volRight),
    mutedLeft(false),
    mutedRight(false),
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
    return (this->mutedLeft && this->mutedRight);
}

bool MixerDevice::isMutedLeft(){
    return this->mutedLeft;
}

bool MixerDevice::isMutedRight(){
    return this->mutedRight;
}

void MixerDevice::mute(){
    if (this->isMuted()){
        return;
    }

    this->muteVolLeft  = this->volLeft;
    this->muteVolRight = this->volRight;
    this->mutedLeft    = true;
    this->mutedRight   = true;

    this->setVolume(0);
}

void MixerDevice::muteLeft(){
    if (this->mutedLeft){
        return;
    }

    this->muteVolLeft = this->volLeft;
    this->mutedLeft   = true;
    this->setVolume(0, this->volRight);
}

void MixerDevice::muteRight(){
    if (this->mutedRight){
        return;
    }

    this->muteVolRight = this->volRight;
    this->mutedRight   = true;
    this->setVolume(this->volLeft, 0);
}

void MixerDevice::unmute(){
    if (!this->isMuted()){
        return;
    }

    this->setVolume(muteVolLeft, muteVolRight);
    this->mutedLeft  = false;
    this->mutedRight = false;
}

void MixerDevice::unmuteLeft() {
    if (!this->mutedLeft){
        return;
    }

    this->volLeft   = this->muteVolLeft;
    this->mutedLeft = false;
    this->setVolume(this->muteVolLeft, this->volRight);
}

void MixerDevice::unmuteRight() {
    if (!this->mutedRight){
        return;
    }

    this->volRight   = this->muteVolRight;
    this->mutedRight = false;
    this->setVolume(this->volLeft, this->muteVolRight);
}

std::pair<uint, uint> MixerDevice::getVolume(){
    return std::make_pair(this->volLeft, this->volRight);
}
