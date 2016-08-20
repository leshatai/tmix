#include <ncurses.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "mixer_window.hpp"

MixerWindow::MixerWindow(const MixerManager &mgr) :
 width(80),
 height(25), 
 curPanelPos(0),
 mgr(mgr),
 curPanel(nullptr),
 minPanelPos(0),
 maxPanelPos(6){
    this->viewport = nullptr;
	this->initMixers();
}

MixerWindow::~MixerWindow(){
	this->close();
    this->viewport = nullptr;
}

void MixerWindow::show(){
	this->init();
	this->handleInput();	
}

std::tuple<uint, uint> MixerWindow::getSize(){
    return std::make_tuple(this->height, this->width); 
}

WINDOW* MixerWindow::getViewport(){
    return this->viewport;
}

void MixerWindow::resize(){
    endwin();
    clear();
    getmaxyx(stdscr, this->height, this->width);
	box(stdscr, 0, 0);
	refresh();

    uint numPanels    = this->mixerPanels.size();
    uint numVisPanels = this->getNumVisiblePanels();
    uint viewportCol  = this->curPanelPos*MixerPanel::WIDTH_MAIN;
    uint viewRows     = this->height-2;
    uint viewCols     = this->width-6;
    
    this->maxPanelPos = numVisPanels-1;
    // can we display all panels on one screen ?
    if (numPanels <= numVisPanels){
        this->maxPanelPos = numPanels-1;
        this->minPanelPos = 0;
        viewportCol       = 0;
        viewCols          = numPanels*MixerPanel::WIDTH_MAIN;
    }

    // are we still on first screen ?
    if (this->curPanelPos < this->maxPanelPos){
        viewportCol = 0;
    }

    // do we have to scroll to current position ?
    if (this->curPanelPos >= this->maxPanelPos && numPanels > numVisPanels){
        this->minPanelPos  = this->curPanelPos - (numVisPanels - 1);
        this->maxPanelPos  = this->curPanelPos;
        viewportCol        = this->minPanelPos*MixerPanel::WIDTH_MAIN;
    }

    prefresh(this->viewport, 0, viewportCol, 4, 1, viewRows, viewCols);
/*
    for(auto &panel : this->mixerPanels){
        panel.resize();
    }
*/
}

void MixerWindow::init(){
	initscr();
	start_color();
	clear();
	// enable getting 'arrow up/down etc.'
	keypad(stdscr, true);
	noecho();
	cbreak();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	//nodelay(stdscr, true);
	box(stdscr, 0, 0);
	wnoutrefresh(stdscr);

    getmaxyx(stdscr, this->height, this->width);
    this->maxPanelPos = this->getNumVisiblePanels()-1;
    uint viewportRows = this->height - 2;
    uint viewportCols = this->mgr.getMixers().size()*MixerPanel::WIDTH_MAIN;
    this->viewport    = newpad(viewportRows, viewportCols);
	this->drawMixers();
	this->selectMixer(0);

    if (this->maxPanelPos < this->mixerPanels.size()-1){
        this->drawScroller(DIR_RIGHT);
    }

    //uint viewCols = (this->maxPanelPos*MixerPanel::WIDTH_MAIN)-2;
    //prefresh(this->viewport, 0, 0, 1, 1, viewportRows, viewCols);
    //refresh();
}

void MixerWindow::close(){
	endwin();
}

void MixerWindow::initMixers(){
	uint panelNr   = 0;
    uint numPanels = this->mgr.getMixers().size();

	for(MixerDevice &dev : this->mgr.getMixers()){
		MixerPanel panel(panelNr, dev, *this);
		this->mixerPanels.push_back(panel);
		panelNr++;
	}

}

void MixerWindow::drawMixers(){
    for(auto &panel : this->mixerPanels){
        panel.draw();
    }
}

void MixerWindow::handleInput(){
	std::ostringstream txt;
	std::stringstream stringNr;
	int c;
	bool quit = false;
	while(!quit){
		c = getch();
		switch(c){
            case KEY_RESIZE:
                this->resize();
            break;
			case KEY_DOWN:
                this->adjustVolume(VOL_DOWN);
				break;
			case KEY_UP:
                this->adjustVolume(VOL_UP);
				break;
			case KEY_LEFT:
				this->selectMixer(this->curPanelPos-1);
				break;
			case KEY_RIGHT:
				this->selectMixer(this->curPanelPos+1);
				break;
			case 'q':
				quit = true;
				break;
			default:
				break;
		}
	}
}

bool MixerWindow::selectMixer(int nr){
    if (nr < 0 || nr > this->mixerPanels.size()-1){
        return false;
    }

    // scroll left
    if (nr < this->minPanelPos){
        this->scrollPanels(DIR_LEFT);
    }

    // scroll right
    if (nr > this->maxPanelPos){
        this->scrollPanels(DIR_RIGHT);
    }

    if (this->minPanelPos > 0){
        this->drawScroller(DIR_LEFT);
    }

    if (this->maxPanelPos < this->mixerPanels.size()-1){
        this->drawScroller(DIR_RIGHT);
    }

    if(this->curPanel){
        this->curPanel.get()->draw();
    }

    this->curPanelPos = nr;
    this->curPanel    = std::make_shared<MixerPanel>(this->mixerPanels.at(nr));
    this->curPanel.get()->highlight();

    uint viewportRows = this->height-2;
    uint viewportCol  = this->minPanelPos*MixerPanel::WIDTH_MAIN;
    uint viewCols     = this->width-6;

    prefresh(this->viewport, 0, viewportCol, 4, 1, viewportRows, viewCols);
    refresh();

    return true;
}

void MixerWindow::drawScroller(int dir){
    uint rows = 9;
    uint col  = dir == DIR_LEFT ? 1 : this->width - 2; 
    std::string dirSymbol = dir == DIR_LEFT ? "<" : ">";
    

    for (uint i=0; i < rows; i++){
        mvprintw(6+i, col, dirSymbol.c_str());
    }
}

void MixerWindow::scrollPanels(int dir){
    uint numPanels    = this->mixerPanels.size()-1;

    if ((this->minPanelPos == 0 && dir == DIR_LEFT) ||
        (this->maxPanelPos == numPanels && dir == DIR_RIGHT)){
        return;
    }    

    this->minPanelPos += dir;
    this->maxPanelPos += dir;
}

uint MixerWindow::getNumVisiblePanels(){
    return this->width  / MixerPanel::WIDTH_MAIN;
}

void MixerWindow::adjustVolume(uint dir){
    MixerPanel *panel = this->curPanel.get();

    if (dir == VOL_DOWN){
        panel->decreaseVolume();
    }
    if (dir == VOL_UP){
        panel->increaseVolume();
    }
    
    this->mgr.updateMixer(panel->getMixer());
    panel->highlight();
    panel->draw();

    uint viewportRows = this->height-2;
    uint viewportCol  = this->minPanelPos*MixerPanel::WIDTH_MAIN;
    uint viewCols     = this->width-6;

    pnoutrefresh(this->viewport, 0, viewportCol, 4, 1, viewportRows, viewCols);
    doupdate();
}
