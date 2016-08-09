/*
 *	This is an example of a mixer program for Linux
 *
 *	updated 1/1/93 to add stereo, level query, broken
 *      	devmask kludge - cmetz@thor.tjhsst.edu
 *
 * (C) Craig Metz and Hannu Savolainen 1993.
 *
 * You may do anything you wish with this program.
 *
 * ditto for my modifications (John-Mark Gurney, 1997)
 *
 * $FreeBSD: head/usr.sbin/mixer/mixer.c 230611 2012-01-27 09:15:55Z mav $
 */

#include <err.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "mixer_manager.hpp"
#include "mixer_device.hpp"
#include "mixer_window.hpp"

void print_volume(const MixerDevice &device){
	std::cout << "#" << device.getNumber() << "  " << device.getName() << "  " << device.getVolumeLeft() << ":" << device.getVolumeRight() << std::endl;
}
void print_volumes(const std::vector<MixerDevice> &devices){
	std::cout << "#devs: " << devices.size() << std::endl;
	for(MixerDevice device : devices){
		print_volume(device);
	}
}

int main(){
	try {
		MixerManager mixerMgr("/dev/mixer");
		MixerWindow window(mixerMgr);
		window.show();
	/*
		std::vector<MixerDevice> &devices = mixerMgr.getMixers();

	print_volumes(devices);
	MixerDevice &pcm = devices.at(1);
	pcm.decVolume(5);
	mixerMgr.updateMixer(pcm);
	print_volume(pcm);
	pcm.incVolume(5);
	mixerMgr.updateMixer(pcm);
	print_volume(pcm);
	*/
	} catch(std::exception &e) {
		std::cout << "err" << std::endl;
	}

	return EXIT_SUCCESS;
}
