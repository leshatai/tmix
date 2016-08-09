#include "mixer_panel.hpp"

#include <ncurses.h>
#include <iostream>
#include <sstream>

MixerPanel::MixerPanel(uint nr, MixerDevice &device) :
 nr(nr),
 device(device),
 mainWindow(nullptr),
 labelWindow(nullptr),
 scaleWindow(nullptr) {
}

MixerPanel::~MixerPanel(){
	if (this->mainWindow){
		delwin(this->mainWindow.get());
	}
	if (this->labelWindow){
		delwin(this->labelWindow.get());
	}
	if (this->scaleWindow){
		delwin(this->scaleWindow.get());
	}
}

MixerDevice& MixerPanel::getMixer(){
    return this->device;
}

void MixerPanel::increaseVolume(){
    this->device.incVolume(3);
    this->draw();
}

void MixerPanel::decreaseVolume(){
    this->device.decVolume(3);
    this->draw();
}

void MixerPanel::refresh(){
	if (!this->isInitialized()){
		return;
	}

	//wnoutrefresh(stdscr);
	//wnoutrefresh(this->mainWindow.get());
	doupdate();
	//wrefresh(this->mainWindow.get());
}

void MixerPanel::highlight(){
	if (!this->mainWindow){
		return;
	}

	wattron(this->labelWindow.get(), COLOR_PAIR(1));
	this->drawLabel();
	wattroff(this->labelWindow.get(), COLOR_PAIR(1));
}

void MixerPanel::draw(){
    if(!this->isInitialized()){
        this->initPanel();
    }

	wrefresh(this->mainWindow.get());

    this->drawLabel();
    this->drawScale();
}

void MixerPanel::initPanel(){
    if (this->isInitialized()){
        return;
    }

    uint beginX = this->nr*10;

	if (!this->mainWindow){
		std::shared_ptr<WINDOW> wnd(newwin(HEIGHT_MAIN, WIDTH_MAIN, 10, beginX));
		this->mainWindow = wnd;
	}
	if (!this->labelWindow){
		std::shared_ptr<WINDOW> wnd(newwin(HEIGHT_LABEL, WIDTH_LABEL, 36, beginX+1));
		this->labelWindow = wnd;
	}
    if (!this->scaleWindow){
        std::shared_ptr<WINDOW> wnd(newwin(HEIGHT_SCALE, WIDTH_SCALE, 11, beginX+3));
        this->scaleWindow = wnd;
    }
} 

bool MixerPanel::isInitialized(){
    return (this->mainWindow && this->labelWindow && this->scaleWindow);
}

void MixerPanel::drawScale(){
	box(this->scaleWindow.get(), 0, 0);
    // as we draw a border, the first and the last line are already
    // occupied with the border symbols
    uint numLines    = HEIGHT_SCALE-2;
    uint heightLeft  = this->device.getVolumeLeft()*(numLines/100.0);
    uint heightRight = this->device.getVolumeRight()*(numLines/100.0);

    // clear the scale
    for(uint i=0; i < numLines; i++){
	    mvwprintw(this->scaleWindow.get(), numLines-i, 1, " ");
	    mvwprintw(this->scaleWindow.get(), numLines-i, 2, " ");
    }

    // draw left/right scale
    for(uint i=0; i < heightLeft; i++){
	    mvwprintw(this->scaleWindow.get(), numLines-i, 1, "%c", 177);
    }

    for(uint i=0; i < heightRight; i++){
	    mvwprintw(this->scaleWindow.get(), numLines-i, 2, "%c", 177);
    }

	wrefresh(this->scaleWindow.get());
}

void MixerPanel::drawLabel(){
	std::ostringstream txt;

	// add current vol
	txt << this->device.getVolumeLeft() << ":" << this->device.getVolumeRight() << std::endl;
	// print number
	mvwprintw(this->labelWindow.get(), 0, 0, txt.str().c_str());
	txt.str("");
	txt.clear();
	// fill with spaces until position 15
	//txt << std::setw(8-this->device.getName().length()) << std::setfill(' ') << std::right;
	// print mixer name
	txt << this->device.getName() << std::endl;
	mvwprintw(this->labelWindow.get(), 1, 0, txt.str().c_str());

	wrefresh(this->labelWindow.get());
}
