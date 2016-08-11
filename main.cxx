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

/*
std::unique_ptr<MixerWindow> window{nullptr};
void signalHandler(int signal){
    window.get()->onResize();
}
*/

int main(){


	try {
		MixerManager mixerMgr("/dev/mixer");
		MixerWindow window(mixerMgr);
        //window = std::make_unique<MixerWindow>(MixerWindow(mixerMgr));
        //signal(SIGWINCH, signalHandler);
		window.show();
	} catch(std::exception &e) {
		std::cout << "err" << std::endl;
	}

	return EXIT_SUCCESS;
}
