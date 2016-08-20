#include "mixer_panel.hpp"

#include <ncurses.h>
#include <iostream>
#include <sstream>

MixerPanel::MixerPanel(uint pos, MixerDevice &device, MixerWindowInterface &window) :
 heightMain(20),
 heightLabel(3),
 heightScale(17),
 pos(pos),
 device(device),
 window(window){
	this->mainWindow  = nullptr;
    this->labelWindow = nullptr;
	this->scaleWindow = nullptr;
}

MixerPanel::~MixerPanel(){
	delwin(this->mainWindow);
    delwin(this->labelWindow);
	delwin(this->scaleWindow);

	this->mainWindow  = nullptr;
    this->labelWindow = nullptr;
	this->scaleWindow = nullptr;
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

void MixerPanel::updatePosition(uint pos){
    this->pos = pos; 
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
        wresize(this->mainWindow, this->heightMain, WIDTH_MAIN);
        wresize(this->scaleWindow, this->heightScale, WIDTH_SCALE);
        mvwin(this->labelWindow, this->heightScale+5, this->pos*10);
    }

    this->draw();
}

void MixerPanel::highlight(){
	if (!this->mainWindow){
		return;
	}

    WINDOW *labelWindow = this->labelWindow;

    uint y = 0;
    uint x = 0;
    getmaxyx(this->labelWindow, y, x);

	wattron(this->labelWindow, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
	this->drawLabel();
	wattroff(this->labelWindow, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
}

void MixerPanel::draw(){
    if(!this->isInitialized()){
        this->calculateSizes();
        this->initPanel();
    }

    this->drawLabel();
    this->drawScale();

	//wrefresh(this->mainWindow);
}

void MixerPanel::initPanel(){
    if (this->isInitialized()){
        return;
    }

	init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);

    uint beginX = this->pos*WIDTH_MAIN;

    WINDOW *viewport = this->window.getViewport();
	if (!this->mainWindow){
		this->mainWindow = derwin(viewport, this->heightMain, WIDTH_MAIN, 0, beginX);
        //box(this->mainWindow, 0, 0);
	}
    if (!this->scaleWindow){
        this->scaleWindow = derwin(this->mainWindow, this->heightScale, WIDTH_SCALE, 1, 3);
    }
	if (!this->labelWindow){
        this->labelWindow = derwin(this->mainWindow, this->heightLabel, WIDTH_LABEL, this->heightScale+1, 2);
	}
} 

bool MixerPanel::isInitialized(){
    return (this->mainWindow && this->labelWindow && this->scaleWindow);
}

void MixerPanel::drawSingleScale(uint numLines, uint height, uint leftRight){
    uint numWhite = numLines*0.2;
    uint numGreen = numLines-(numWhite*2);
    uint cp       = COLOR_PAIR_WHITE;

    WINDOW *scaleWindow = this->scaleWindow;
    for(uint i=0; i < height; i++){
        if(i > numWhite && i < numGreen){
            cp = COLOR_PAIR_GREEN;
        }
        if(i > numGreen){
            cp = COLOR_PAIR_RED;
        }
        wattron(scaleWindow, COLOR_PAIR(cp));
	    mvwprintw(scaleWindow, numLines-i, leftRight, "%c", 177);
        wattroff(scaleWindow, COLOR_PAIR(cp));
    }
}

void MixerPanel::clearScale(uint numLines){
    WINDOW *scaleWindow = this->scaleWindow;

    for(uint i=0; i < numLines; i++){
	    mvwprintw(scaleWindow, numLines-i, 1, " ");
	    mvwprintw(scaleWindow, numLines-i, 2, " ");
    }
}

void MixerPanel::drawScale(){
    WINDOW *scaleWindow = this->scaleWindow;

	box(scaleWindow, 0, 0);
    // as we draw a border, the first and the last line are already
    // occupied with the border symbols
    uint numLines    = this->heightScale-2;
    uint heightLeft  = this->device.getVolumeLeft()*(numLines/100.0);
    uint heightRight = this->device.getVolumeRight()*(numLines/100.0);

    this->clearScale(numLines);
    this->drawSingleScale(numLines, heightLeft, SCALE_LEFT);
    this->drawSingleScale(numLines, heightRight, SCALE_RIGHT);

	wrefresh(scaleWindow);
}

void MixerPanel::drawLabel(){
    WINDOW *labelWindow = this->labelWindow;
	std::ostringstream txt;

	// add current vol
	txt << this->device.getVolumeLeft() << ":" << this->device.getVolumeRight() << std::endl;
	// print number
	mvwprintw(labelWindow, 0, 0, txt.str().c_str());
	txt.str("");
	txt.clear();
	// fill with spaces until position 15
	//txt << std::setw(8-this->device.getName().length()) << std::setfill(' ') << std::right;
	// print mixer name
	txt << this->device.getName() << std::endl;
	mvwprintw(labelWindow, 1, 0, txt.str().c_str());

	wrefresh(labelWindow);
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
