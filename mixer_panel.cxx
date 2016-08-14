#include "mixer_panel.hpp"

#include <ncurses.h>
#include <iostream>
#include <sstream>

MixerPanel::MixerPanel(uint nr, MixerDevice &device, MixerWindowInterface &window) :
 heightMain(20),
 heightLabel(3),
 heightScale(17),
 nr(nr),
 device(device),
 mainWindow(nullptr),
 labelWindow(nullptr),
 scaleWindow(nullptr),
 window(window){
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

void MixerPanel::resize(){
    if(!this->isInitialized()){
        this->initPanel();
    } else {
        this->calculateSizes(); 
        wresize(this->mainWindow.get(), this->heightMain, WIDTH_MAIN);
        wresize(this->scaleWindow.get(), this->heightScale, WIDTH_SCALE);
        mvwin(this->labelWindow.get(), this->heightScale+5, this->nr*10);
    }

    this->draw();
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
        this->calculateSizes();
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

	init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);

    uint beginX = this->nr*10;

	if (!this->mainWindow){
		std::shared_ptr<WINDOW> wnd(newwin(this->heightMain, WIDTH_MAIN, 4, beginX));
		this->mainWindow = wnd;
	}
    if (!this->scaleWindow){
        std::shared_ptr<WINDOW> wnd(newwin(this->heightScale, WIDTH_SCALE, 5, beginX+3));
        this->scaleWindow = wnd;
    }
	if (!this->labelWindow){
		std::shared_ptr<WINDOW> wnd(newwin(this->heightLabel, WIDTH_LABEL, this->heightScale+5, beginX+1));
		this->labelWindow = wnd;
	}
} 

bool MixerPanel::isInitialized(){
    return (this->mainWindow && this->labelWindow && this->scaleWindow);
}

void MixerPanel::drawSingleScale(uint numLines, uint height, uint leftRight){
    uint numWhite = numLines*0.2;
    uint numGreen = numLines-(numWhite*2);
    uint cp       = COLOR_PAIR_WHITE;

    for(uint i=0; i < height; i++){
        if(i > numWhite && i < numGreen){
            cp = COLOR_PAIR_GREEN;
        }
        if(i > numGreen){
            cp = COLOR_PAIR_RED;
        }
        wattron(this->scaleWindow.get(), COLOR_PAIR(cp));
	    mvwprintw(this->scaleWindow.get(), numLines-i, leftRight, "%c", 177);
        wattroff(this->scaleWindow.get(), COLOR_PAIR(cp));
    }
}

void MixerPanel::clearScale(uint numLines){
    for(uint i=0; i < numLines; i++){
	    mvwprintw(this->scaleWindow.get(), numLines-i, 1, " ");
	    mvwprintw(this->scaleWindow.get(), numLines-i, 2, " ");
    }
}

void MixerPanel::drawScale(){
	box(this->scaleWindow.get(), 0, 0);
    // as we draw a border, the first and the last line are already
    // occupied with the border symbols
    uint numLines    = this->heightScale-2;
    uint heightLeft  = this->device.getVolumeLeft()*(numLines/100.0);
    uint heightRight = this->device.getVolumeRight()*(numLines/100.0);

    this->clearScale(numLines);
    this->drawSingleScale(numLines, heightLeft, SCALE_LEFT);
    this->drawSingleScale(numLines, heightRight, SCALE_RIGHT);

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

void MixerPanel::calculateSizes(){
    std::tuple<uint, uint> size = this->window.getSize();
    uint height = std::get<0>(size);
    //uint width  = std::get<1>(size);

    if (height < 25){
        return;
    }

    this->heightMain  = height - 8; 
    this->heightLabel = 3;
    this->heightScale = this->heightMain - this->heightLabel - 3;
}
