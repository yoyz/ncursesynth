#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

// Screen layout constants
namespace UILayout {
    const int TITLE_ROW = 0;
    const int SUBTITLE_ROW = 1;
    const int DIVIDER_ROW = 2;
    const int STATUS_ROW = 4;
    
    const int LEFT_COL = 2;
    const int RIGHT_COL = 45;  // Moved left to make room for voice display
    
    const int VOICE_START_ROW = 30;  // Row where voice display starts
    const int VOICE_INFO_ROW = 32;    // Row for voice information
    
    const int KEYBOARD_ROW = 38;
    const int MIDI_SECTION_ROW = 46;
    const int CONTROLS_ROW = 48;
    const int SELECTED_ROW = 56;
    const int STATUS_LINE_ROW = 58;
    
    // Section headers
    const int FILTER_SECTION_ROW = 6;
    const int AMP_ENV_SECTION_ROW = 18;
    const int FILTER_ENV_SECTION_ROW = 26;
    const int OSCILLATOR_SECTION_ROW = 6;
    const int DELAY_SECTION_ROW = 12;
    const int REVERB_SECTION_ROW = 19;
    const int CHORUS_SECTION_ROW = 26;
    const int DISTORTION_SECTION_ROW = 33;
    const int MASTER_SECTION_ROW = 40;
}

#endif
