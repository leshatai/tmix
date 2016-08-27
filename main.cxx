/*
 * Program to adjust volume levels for available mixer devices
 * using ncurses
 *
 * (C) Martin Weise 2016.
 *
 * You may do anything you wish with this program.
 */

#include <string>
#include <iostream>

#include "mixer_manager.hpp"
#include "mixer_window.hpp"

int main(int argc, char* argv[]){
    static const std::string DEFAULT_DEV = "/dev/mixer";

    std::string devName = argc > 1 ? argv[1] : DEFAULT_DEV;

    try {
        MixerManager mixerMgr(devName);
        MixerWindow  window(mixerMgr);
        window.show();
    } catch(std::exception &e) {
        std::cout << "Exception. Cannot start." << std::endl;
    }

    return EXIT_SUCCESS;
}
