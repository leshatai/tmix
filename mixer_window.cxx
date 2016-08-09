#include <ncurses.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "mixer_window.hpp"

MixerWindow::MixerWindow(const MixerManager &mgr) : curPanelNr(0), mgr(mgr), curPanel(nullptr){
}

MixerWindow::~MixerWindow(){
	this->close();
}

void MixerWindow::show(){
	this->init();
	this->handleInput();	
}

void MixerWindow::init(){
	initscr();
	start_color();
	clear();
	// enable getting 'arrow up/down etc.'
	keypad(stdscr, true);
	noecho();
	cbreak();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	attron(COLOR_PAIR(1));
	//nodelay(stdscr, true);
	box(stdscr, 0, 0);
	this->initMixers();
	wnoutrefresh(stdscr);
	this->drawMixers();
	this->selectMixer(1);
}

void MixerWindow::close(){
	endwin();
}

void MixerWindow::initMixers(){
	uint panelNr = 1;
	for(MixerDevice &dev : this->mgr.getMixers()){
		MixerPanel panel(panelNr, dev);
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
			case KEY_DOWN:
                this->curPanel.get()->decreaseVolume();
                this->mgr.updateMixer(this->curPanel.get()->getMixer());
                this->curPanel.get()->highlight();
				break;
			case KEY_UP:
                this->curPanel.get()->increaseVolume();
                this->mgr.updateMixer(this->curPanel.get()->getMixer());
                this->curPanel.get()->highlight();
				break;
			case KEY_LEFT:
				this->selectMixer(this->curPanelNr-1);
				break;
			case KEY_RIGHT:
				this->selectMixer(this->curPanelNr+1);
				break;
			case 'q':
				quit = true;
				break;
			default:
				break;
		}
	}
}

bool MixerWindow::selectMixer(uint nr){
	if (nr == 0 || nr > this->mixerPanels.size()){
		return false;
	}

	if (this->curPanel){
		this->curPanel.get()->draw();
	}

	this->curPanelNr = nr;
	this->curPanel   = std::make_shared<MixerPanel>(this->mixerPanels.at(--nr));
	this->curPanel.get()->highlight();

	return true;
}
