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

#include "mixer_panel.hpp"

#include <ncurses.h>
#include <iostream>
#include <sstream>

MixerPanel::MixerPanel(uint pos, MixerDevice &device) :
    heightLabel{3},
    heightScale{17},
    pos{pos},
    channel{CHANNEL_BOTH},
    device{device}, 
    labelWindow{nullptr},
    scaleWindow{nullptr} {
    }

MixerPanel::~MixerPanel(){
    delwin(this->labelWindow.get());
    delwin(this->scaleWindow.get());
}

void MixerPanel::mute(){
    switch(this->channel){
        case CHANNEL_LEFT:
            if (this->device.isMutedLeft()){
                this->device.unmuteLeft();
            } else {
                this->device.muteLeft();
            }
            break;
        case CHANNEL_RIGHT:
            if (this->device.isMutedRight()){
                this->device.unmuteRight();
            } else {
                this->device.muteRight();
            }
            break;
        case CHANNEL_BOTH:
        default:
            if (this->device.isMuted()){
                this->device.unmute();
            } else {
                this->device.mute();
            }
            break;
    }

    this->draw();
}

MixerDevice& MixerPanel::getMixer(){
    return this->device;
}

void MixerPanel::increaseVolume(){
    auto vol = this->getChannelOffsets();

    this->device.incVolume(vol.first, vol.second);
    this->draw();
}

void MixerPanel::decreaseVolume(){
    auto vol = this->getChannelOffsets();

    this->device.decVolume(vol.first, vol.second);
    this->draw();
}

void MixerPanel::resize(uint viewportHeight){
    werase(this->labelWindow.get());
    werase(this->scaleWindow.get());
    this->calculateSizes(viewportHeight);

    auto beginX = this->pos*WIDTH;
    mvderwin(this->labelWindow.get(), this->heightScale+1, beginX+2);
    wresize(this->scaleWindow.get(), this->heightScale, WIDTH_SCALE);

    this->draw();
}

void MixerPanel::highlight(){
    auto *label = this->labelWindow.get();

    wattron(label, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
    this->drawLabel();
    wattroff(label, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
}

void MixerPanel::draw(){
    wclear(this->labelWindow.get());
    wclear(this->scaleWindow.get());

    this->drawLabel();
    this->drawScale();
}

void MixerPanel::draw(uint viewportHeight){
    this->calculateSizes(viewportHeight);
    this->draw();
}

void MixerPanel::init(WINDOW &viewport){
    auto height = 0;
    auto width  = 0;
    getmaxyx(&viewport, height, width);
    this->calculateSizes(height);

    init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);

    auto beginX = this->pos*WIDTH;

    if (!this->scaleWindow){
        this->scaleWindow = std::shared_ptr<WINDOW>{derwin(&viewport, this->heightScale, WIDTH_SCALE, 1, beginX+3)};
    }
    if (!this->labelWindow){
        this->labelWindow = std::shared_ptr<WINDOW>{derwin(&viewport, this->heightLabel, WIDTH_LABEL, this->heightScale+1, beginX+2)};
    }
} 

bool MixerPanel::isInitialized(){
    return (this->labelWindow && this->scaleWindow);
}

void MixerPanel::drawSingleScale(uint numLines, uint height, uint leftRight){
    uint numWhite = numLines*0.2;
    uint numRed   = numLines*0.1;
    auto numGreen = numLines-(numWhite+numRed);
    auto cp       = COLOR_PAIR_WHITE;

    auto *scaleWindow = this->scaleWindow.get();
    for(auto i=0u; i < height; i++){
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

void MixerPanel::drawScale(){

    box(this->scaleWindow.get(), 0, 0);
    // as we draw a border, the first and the last line are already
    // occupied with the border symbols
    auto numLines    = this->heightScale-2;
    auto heightLeft  = this->device.getVolumeLeft()*(numLines/100.0);
    auto heightRight = this->device.getVolumeRight()*(numLines/100.0);

    this->drawSingleScale(numLines, heightLeft, SCALE_LEFT);
    this->drawSingleScale(numLines, heightRight, SCALE_RIGHT);
}

void MixerPanel::drawLabel(){
    auto *label     = this->labelWindow.get();
    auto vol        = this->device.getVolume();
    auto channelInd = "";

    std::ostringstream txt;

    if (this->device.isMutedLeft()){
        txt << "M";
    } else {
        txt << vol.first;
    }

    txt << ":";

    if (this->device.isMutedRight()){
        txt << "M";
    } else {
        txt << vol.second;
    }

    // print number
    mvwprintw(label, 0, 0, txt.str().c_str());
    txt.str("");
    txt.clear();
    // fill with spaces until position 15
    //txt << std::setw(8-this->device.getName().length()) << std::setfill(' ') << std::right;
    // print mixer name
    if (this->channel != CHANNEL_BOTH) {
        channelInd = this->channel == CHANNEL_LEFT ? "L)" : "R)";
    }
    txt << channelInd << this->device.getName();
    mvwprintw(label, 1, 0, txt.str().c_str());
}

void MixerPanel::calculateSizes(uint viewportHeight){
    auto height = viewportHeight;

    if (height < 10){
        height = 10;
    }

    this->heightLabel = 2;
    this->heightScale = (height - this->heightLabel) - 3; // 3 = border
}

void MixerPanel::toogleChannel(uint channel){
    this->channel = channel;
}

std::pair<uint, uint> MixerPanel::getChannelOffsets(){
    auto left  = 3u;
    auto right = 3u;

    switch (this->channel){
        case CHANNEL_LEFT:
            right = 0;
        break;
        case CHANNEL_RIGHT:
            left = 0;
        break;
        case CHANNEL_BOTH:
        default:
        break;
    }

    return std::make_pair(left, right);
}

void MixerPanel::alignVolume(){
    auto vol   = this->device.getVolume();
    auto left  = vol.first;
    auto right = vol.second;

    if (left < right){
        left = right;
    } else {
        right = left;
    }

    // unmute no matter what
    this->device.unmuteLeft();
    this->device.unmuteRight();

    this->device.setVolume(left, right);
}
