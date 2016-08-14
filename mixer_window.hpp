#ifndef __MIXER_WINDOW_H__
#define __MIXER_WINDOW_H__

#include "mixer_window_interface.hpp"
#include "mixer_manager.hpp"
#include "mixer_panel.hpp"

#include <tuple>
#include <memory>
#include <ncurses.h>

class MixerWindow : MixerWindowInterface {
protected:
    uint width;
    uint height;
	uint curPanelNr;
	MixerManager mgr;
	std::shared_ptr<MixerPanel> curPanel;
	std::vector<MixerPanel> mixerPanels;

	void init();
	void close();
	void initMixers();
	void drawMixers();
	void handleInput();
	bool selectMixer(uint nr);
    void resize();
public:
	MixerWindow(const MixerManager &mgr);
	~MixerWindow();
	void show();
    std::tuple<uint, uint> getSize();
};

#endif
