#ifndef __MIXER_PANEL_H__
#define __MIXER_PANEL_H__

#include "mixer_device.hpp"

#include <ncurses.h>
#include <memory>

class MixerPanel{
protected:
    static const uint WIDTH_LABEL  = 8;
    static const uint WIDTH_MAIN   = 10;
    static const uint WIDTH_SCALE  = 4;

    static const uint HEIGHT_LABEL = 3;
    static const uint HEIGHT_MAIN  = 30;
    static const uint HEIGHT_SCALE = 25;

	uint nr;
	MixerDevice &device;
	std::shared_ptr<WINDOW> mainWindow;
	std::shared_ptr<WINDOW> labelWindow;
	std::shared_ptr<WINDOW> scaleWindow;

    bool isInitialized();
    void initPanel();
    void drawLabel();
    void drawScale();
public:
	MixerPanel(uint nr, MixerDevice &device);
	~MixerPanel();
    MixerDevice& getMixer();
	void refresh();
	void highlight();
	void draw();

    void decreaseVolume();
    void increaseVolume();
};

#endif
