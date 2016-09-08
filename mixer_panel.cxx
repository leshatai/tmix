#include "mixer_panel.hpp"

#include <ncurses.h>
#include <iostream>
#include <sstream>

MixerPanel::MixerPanel(uint pos, MixerDevice &device) :
    heightLabel(3),
    heightScale(17),
    pos(pos),
    channel(CHANNEL_BOTH),
    device(device){
        this->labelWindow = nullptr;
        this->scaleWindow = nullptr;
    }

MixerPanel::~MixerPanel(){
    delwin(this->labelWindow);
    delwin(this->scaleWindow);

    this->labelWindow = nullptr;
    this->scaleWindow = nullptr;
}

void MixerPanel::mute(){
    if (this->device.isMuted()){
        this->device.unmute();
    } else {
        this->device.mute();
    }

    this->draw();
}

MixerDevice& MixerPanel::getMixer(){
    return this->device;
}

void MixerPanel::increaseVolume(){
    std::pair<uint, uint> vol = this->getChannelOffsets();

    this->device.incVolume(vol.first, vol.second);
    this->draw();
}

void MixerPanel::decreaseVolume(){
    std::pair<uint, uint> vol = this->getChannelOffsets();

    this->device.decVolume(vol.first, vol.second);
    this->draw();
}

void MixerPanel::resize(uint viewportHeight){
    werase(this->labelWindow);
    werase(this->scaleWindow);
    this->calculateSizes(viewportHeight);

    uint beginX = this->pos*WIDTH;
    mvderwin(this->labelWindow, this->heightScale+1, beginX+2);
    wresize(this->scaleWindow, this->heightScale, WIDTH_SCALE);

    this->draw();
}

void MixerPanel::highlight(){
    WINDOW *label = this->labelWindow;

    wattron(label, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
    this->drawLabel();
    wattroff(label, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT));
}

void MixerPanel::draw(){
    wclear(this->labelWindow);
    wclear(this->scaleWindow);

    this->drawLabel();
    this->drawScale();
}

void MixerPanel::draw(uint viewportHeight){
    this->calculateSizes(viewportHeight);
    this->draw();
}

void MixerPanel::init(WINDOW &viewport){
    uint height = 0;
    uint width  = 0;
    getmaxyx(&viewport, height, width);
    this->calculateSizes(height);

    init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);

    uint beginX = this->pos*WIDTH;

    if (!this->scaleWindow){
        this->scaleWindow = derwin(&viewport, this->heightScale, WIDTH_SCALE, 1, beginX+3);
    }
    if (!this->labelWindow){
        this->labelWindow = derwin(&viewport, this->heightLabel, WIDTH_LABEL, this->heightScale+1, beginX+2);
    }
} 

bool MixerPanel::isInitialized(){
    return (this->labelWindow && this->scaleWindow);
}

void MixerPanel::drawSingleScale(uint numLines, uint height, uint leftRight){
    uint numWhite = numLines*0.2;
    uint numRed   = numLines*0.1;
    uint numGreen = numLines-(numWhite+numRed);
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

void MixerPanel::drawScale(){

    box(this->scaleWindow, 0, 0);
    // as we draw a border, the first and the last line are already
    // occupied with the border symbols
    uint numLines    = this->heightScale-2;
    uint heightLeft  = this->device.getVolumeLeft()*(numLines/100.0);
    uint heightRight = this->device.getVolumeRight()*(numLines/100.0);

    this->drawSingleScale(numLines, heightLeft, SCALE_LEFT);
    this->drawSingleScale(numLines, heightRight, SCALE_RIGHT);
}

void MixerPanel::drawLabel(){
    WINDOW *label = this->labelWindow;
    std::ostringstream txt;

    // add current vol
    if (this->device.isMuted()){
        txt << "M:M";
    } else {
        txt << this->device.getVolumeLeft() << ":" << this->device.getVolumeRight();
    }

    // print number
    mvwprintw(label, 0, 0, txt.str().c_str());
    txt.str("");
    txt.clear();
    // fill with spaces until position 15
    //txt << std::setw(8-this->device.getName().length()) << std::setfill(' ') << std::right;
    // print mixer name
    txt << this->device.getName();
    mvwprintw(label, 1, 0, txt.str().c_str());
}

void MixerPanel::calculateSizes(uint viewportHeight){
    uint height = viewportHeight;

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
    uint left = 3;
    uint right = 3;

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
