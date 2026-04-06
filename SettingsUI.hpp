#ifndef SETTINGS_UI_HPP
#define SETTINGS_UI_HPP

class Sequencer;

class SettingsUI {
public:
    void render(Sequencer& seq);

private:
    int selectedMenu = 0;

    void renderPlayback(Sequencer& seq);
    void renderRecording(Sequencer& seq);
    void renderOptions(Sequencer& seq);
    void renderMisc();
};

#endif
