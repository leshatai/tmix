#include <ncurses.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "mixer_window.hpp"

const std::string MixerWindow::IND_LEFT  = "<";
const std::string MixerWindow::IND_RIGHT = ">";

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
    endwin();
    this->viewport = nullptr;
}

void MixerWindow::show(){
    this->init();
    this->drawMixers();
    this->selectMixer(0);
    this->handleInput();	
}

WINDOW* MixerWindow::getViewport(){
    return this->viewport;
}

void MixerWindow::resize(){
    uint oldHeight = this->height;
    getmaxyx(stdscr, this->height, this->width);

    uint numPanels    = this->mixerPanels.size();
    uint numVisPanels = this->getNumVisiblePanels();

    this->maxPanelPos = numVisPanels-1;
    this->height      = this->height >= 10 ? this->height : 10;

    // do we have to resize the panels ?
    if (this->height != oldHeight){
        wresize(this->viewport, this->height-2, numPanels*MixerPanel::WIDTH_MAIN);
        for(auto &panel : this->mixerPanels){
            panel.resize();
        }
    }

    // can we display all panels on one screen ?
    if (numPanels <= numVisPanels){
        this->maxPanelPos = numPanels-1;
        this->minPanelPos = 0;
    }

    // do we have to scroll to current position ?
    if (this->curPanelPos >= this->maxPanelPos && numPanels > numVisPanels){
        this->minPanelPos  = this->curPanelPos - (numVisPanels - 1);
        this->maxPanelPos  = this->curPanelPos;
    }

    clear();
    box(stdscr, 0, 0);
    refresh();
    this->updateViewport();
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
    //nodelay(stdscr, true); // this causes 100% cpu usage

    getmaxyx(stdscr, this->height, this->width);
    this->maxPanelPos = this->getNumVisiblePanels()-1;
    uint viewportRows = this->height - 2;
    uint viewportCols = this->mgr.getMixers().size()*MixerPanel::WIDTH_MAIN;
    this->viewport    = newpad(viewportRows, viewportCols);

    box(stdscr, 0, 0);
    refresh();
}

void MixerWindow::initMixers(){
    uint panelNr   = 0;

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
            case 'm':
                    this->muteMixer();
                break;
            case 'q':
                quit = true;
                break;
            default:
                break;
        }
    }
}

void MixerWindow::selectMixer(int pos){
    if (pos < 0 || pos > this->mixerPanels.size()-1){
        return;
    }

    // scroll left
    if (pos < this->minPanelPos){
        this->scrollPanels(DIR_LEFT);
    }

    // scroll right
    if (pos > this->maxPanelPos){
        this->scrollPanels(DIR_RIGHT);
    }

    if(this->curPanel){
        this->curPanel.get()->draw();
    }

    this->curPanelPos = pos;
    this->curPanel    = std::make_shared<MixerPanel>(this->mixerPanels.at(pos));

    this->updateViewport();
    //this->updateScrollers();
}

void MixerWindow::drawScroller(int dir){
    uint rows = 9;
    uint col  = dir == DIR_LEFT ? 1 : this->width - 2; 
    std::string dirSymbol = dir == DIR_LEFT ? IND_LEFT : IND_RIGHT;


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
    this->updateViewport();
}

void MixerWindow::updateViewport(){
    uint viewportCols = this->mixerPanels.size()*MixerPanel::WIDTH_MAIN;
    uint viewportRows = this->height-2;
    uint viewportCol  = this->minPanelPos*MixerPanel::WIDTH_MAIN;
    uint viewCols     = this->width-6;

    if (this->curPanelPos < this->maxPanelPos){
        viewportCol = 0;
    }

    if (viewCols > viewportCols){
        viewCols = viewportCols;
    }

    if (this->curPanel){
        this->curPanel.get()->highlight();
    }

    prefresh(this->viewport, 0, viewportCol, 1, 1, viewportRows, viewCols);
}

void MixerWindow::updateScrollers(){
    if (this->minPanelPos > 0){
        this->drawScroller(DIR_LEFT);
    }

    if (this->maxPanelPos < this->mixerPanels.size()-1){
        this->drawScroller(DIR_RIGHT);
    }
}

void MixerWindow::muteMixer(){
    if (!this->curPanel){
        return;
    }

    this->curPanel->mute();
    this->mgr.updateMixer(this->curPanel->getMixer());
    this->updateViewport();
}
