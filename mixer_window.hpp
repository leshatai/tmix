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
    static const int DIR_LEFT  = -1;
    static const int DIR_RIGHT = 1;
    static const int VOL_UP    = 1;
    static const int VOL_DOWN  = 2;

    uint width;
    uint height;
	uint curPanelPos;
	MixerManager mgr;
	std::shared_ptr<MixerPanel> curPanel;
	std::vector<MixerPanel> mixerPanels;
    /** lower boundary in view port */
    uint minPanelPos;
    /** upper boundary in view port */
    uint maxPanelPos;
    WINDOW *viewport;

	void init();
	void close();
	void initMixers();
	void drawMixers();
    void resize();
	void handleInput();
	bool selectMixer(int nr);
    void drawScroller(int dir);
    void scrollPanels(int dir);
    void adjustVolume(uint dir);
    uint getNumVisiblePanels();

public:
	MixerWindow(const MixerManager &mgr);
	~MixerWindow();
	void show();
    std::tuple<uint, uint> getSize();
    WINDOW* getViewport();
};

#endif
