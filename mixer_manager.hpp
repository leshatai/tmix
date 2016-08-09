#ifndef __MIXER_MANAGER_H__
#define __MIXER_MANAGER_H__

#include <vector>
#include <string>
#include "mixer_device.hpp"

class MixerManager{
private:
	int deviceHndl;
	std::string device;
	std::vector<MixerDevice> mixers;
public:
	MixerManager(std::string device);
	std::vector<MixerDevice> &getMixers(){ return mixers; }
	void updateMixer(MixerDevice mixer);
	uint getMixerCount(){ return this->mixers.size(); }
};

#endif
