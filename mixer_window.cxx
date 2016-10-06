/*-
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) Copyright (c) 2016 Martin Weise
 */

#include <ncurses.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>


#include "mixer_device.hpp"
#include "mixer_window.hpp"

const std::string MixerWindow::IND_LEFT  = "<";
const std::string MixerWindow::IND_RIGHT = ">";
const std::string MixerWindow::BLANK     = " ";

MixerWindow::MixerWindow(MixerManager &mgr) :
    width{80},
    height{25}, 
    curPanelPos{0},
    mgr{mgr},
    curPanel{nullptr},
    minPanelPos{0},
    maxPanelPos{6} {
        this->initMixers();
    }

MixerWindow::~MixerWindow(){
    endwin();
}

void MixerWindow::show(){
    this->init();
    this->selectMixer(0);
    this->handleInput();
}

void MixerWindow::resize(){
    auto oldHeight = this->height;
    getmaxyx(stdscr, this->height, this->width);

    auto numPanels    = this->mixerPanels.size();
    auto numVisPanels = this->getNumVisiblePanels();
    this->maxPanelPos = numVisPanels-1;
    this->height      = this->height >= 10 ? this->height : 10;

    // do we have to resize the panels ?
    if (this->height != oldHeight){
        auto viewportHeight = this->height - PAD_HEIGHT_VIEWPORT;
        auto isBigger       = oldHeight < this->height;
        auto *viewport    = this->viewport.get();

        werase(viewport);
        if (isBigger ){
            wresize(viewport, viewportHeight, numPanels*MixerPanel::WIDTH);
        }

        for(auto &panel : this->mixerPanels){
            panel.resize(viewportHeight);
        }

        if (!isBigger ){
            wresize(viewport, viewportHeight, numPanels*MixerPanel::WIDTH);
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
    auto numVisPanels = this->getNumVisiblePanels() - 1;
    auto numPanels    = this->mixerPanels.size() - 1;
    this->maxPanelPos = numVisPanels > numPanels ? numPanels : numVisPanels;
    auto viewportRows = this->height - PAD_HEIGHT_VIEWPORT;
    auto viewportCols = this->mgr.getMixerCount()*MixerPanel::WIDTH;
    this->viewport    = std::shared_ptr<WINDOW>{newpad(viewportRows, viewportCols)};

    for(auto &panel : this->mixerPanels){
        panel.init(*this->viewport);
        panel.draw();
    }

    box(stdscr, 0, 0);
    refresh();
}

void MixerWindow::initMixers(){
    auto panelNr   = 0u;

    for(auto &dev : this->mgr.getMixers()){
        MixerPanel panel{panelNr, dev};
        this->mixerPanels.push_back(std::move(panel));
        panelNr++;
    }

}

void MixerWindow::handleInput(){
    auto c = 0;
    auto quit = false;

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
                this->scrollPanels(DIR_LEFT, false);
                break;
            case KEY_RIGHT:
                this->scrollPanels(DIR_RIGHT, false);
                break;
            case KEY_NPAGE:
                this->scrollPanels(DIR_LEFT, true);
                break;
            case KEY_PPAGE:
                this->scrollPanels(DIR_RIGHT, true);
                break;
            case 'a':
                    this->alignVolume();
                break;
            case 'b':
                    this->toogleMixerChannel(MixerPanel::CHANNEL_BOTH);
                break;
            case 'r':
                    this->toogleMixerChannel(MixerPanel::CHANNEL_RIGHT);
                break;
            case 'l':
                    this->toogleMixerChannel(MixerPanel::CHANNEL_LEFT);
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

void MixerWindow::selectMixer(uint pos){
    if (pos < 0 || pos > this->mixerPanels.size()-1){
        return;
    }

    if(this->curPanel){
        this->curPanel->drawLabel();
    }

    this->curPanelPos = pos;
    this->curPanel    = &this->mixerPanels.at(pos);
    this->updateViewport();
}

void MixerWindow::drawScroller(int dir, bool remove){
    auto rows = 9;
    auto col  = dir == DIR_LEFT ? 1 : this->width - 2; 
    auto dirSymbol = dir == DIR_LEFT ? IND_LEFT : IND_RIGHT;

    if (remove){
        dirSymbol = BLANK;
    }

    for (auto i=0; i < rows; i++){
        mvaddstr(6+i, col, dirSymbol.c_str());
    }
}

uint MixerWindow::getNumVisiblePanels(){
    return (this->width - PAD_WIDTH_VIEWPORT)  / MixerPanel::WIDTH;
}

void MixerWindow::adjustVolume(uint dir){
    if (dir == VOL_DOWN){
        this->curPanel->decreaseVolume();
    }
    if (dir == VOL_UP){
        this->curPanel->increaseVolume();
    }

    this->mgr.updateMixer(this->curPanel->getMixer());
    this->updateViewport();
}

void MixerWindow::updateViewport(){
    // we have to add one column, else we would right most col of the last panel
    // would be cut off
    auto viewportCols = (this->mixerPanels.size()*MixerPanel::WIDTH) + 1;
    auto viewportRows = this->height - (PAD_HEIGHT_VIEWPORT - 1);
    auto viewportCol  = this->minPanelPos * MixerPanel::WIDTH;
    auto viewCols     = (this->getNumVisiblePanels() * MixerPanel::WIDTH) + 1;

    if (viewCols > viewportCols){
        viewCols = viewportCols;
    }

    if (this->curPanel){
        this->curPanel->highlight();
    }

    mvaddstr(1, 2, "Keys: 'q': Quit | 'm': Mute | 'l','r','b' : Toogle Channel | 'a' : Align Volume");
    this->updateScrollers();
    wnoutrefresh(stdscr);
    pnoutrefresh(this->viewport.get(), 0, viewportCol, 2, 2, viewportRows, viewCols);
    doupdate();
}

void MixerWindow::updateScrollers(){
    auto removeRight = this->maxPanelPos == this->mixerPanels.size() - 1;
    auto removeLeft  = this->minPanelPos == 0;

    this->drawScroller(DIR_RIGHT, removeRight);
    this->drawScroller(DIR_LEFT, removeLeft);
}

void MixerWindow::muteMixer(){
    if (!this->curPanel){
        return;
    }

    this->curPanel->mute();
    this->mgr.updateMixer(this->curPanel->getMixer());
    this->updateViewport();
}

uint MixerWindow::getCurrentPage(){
    auto numVis = this->getNumVisiblePanels();

    return std::ceil((float)(this->curPanelPos+1) / (float)numVis);
}

uint MixerWindow::getNumPages(){
    auto numPanels = this->mixerPanels.size();
    auto numVis    = this->getNumVisiblePanels();

    return std::ceil((float)numPanels / (float)numVis);
}

void MixerWindow::scrollPanels(int dir, bool pagewise){
    if (dir != DIR_LEFT && dir != DIR_RIGHT){
        return;
    }

    auto numPanels    = this->mixerPanels.size();
    auto pos          = this->curPanelPos;

    if (!pagewise){
        if ((pos == 0 && dir == DIR_LEFT) || (pos == numPanels - 1 && dir == DIR_RIGHT)){
            return;
        }

        pos += dir;
        // we only update boundaries, if we step over them
        if (pos < this->minPanelPos || pos > this->maxPanelPos){
            this->minPanelPos += dir;
            this->maxPanelPos += dir;
        }
    } else {
        auto numVisPanels = this->getNumVisiblePanels();
        auto curPage      = this->getCurrentPage();
        auto numPages     = this->getNumPages();
        auto nextPage     = curPage + dir;

        if (nextPage > numPages || nextPage < 1){
            return;
        }

        pos               = nextPage == 1 ? 0 : (curPage * numVisPanels);
        auto maxPanelPos  = pos + (numVisPanels - 1);
        this->minPanelPos = pos;
        this->maxPanelPos = maxPanelPos < numPanels ? maxPanelPos : numPanels - 1;

        // avoid scrolling behind last mixer panel
        if (this->maxPanelPos - numVisPanels < this->minPanelPos){
            this->minPanelPos = this->maxPanelPos - (numVisPanels - 1);
            pos = this->maxPanelPos;
        }
    }

    this->selectMixer(pos);
}

void MixerWindow::toogleMixerChannel(uint channel){
    if (!this->curPanel){
        return;
    }

    this->curPanel->toogleChannel(channel);
    this->curPanel->draw();
    this->updateViewport();
}

void MixerWindow::alignVolume(){
    if (!this->curPanel){
        return;
    }

    this->curPanel->toogleChannel(MixerPanel::CHANNEL_BOTH);
    this->curPanel->alignVolume();
    this->mgr.updateMixer(this->curPanel->getMixer());
    this->curPanel->draw();
    this->updateViewport();
}
