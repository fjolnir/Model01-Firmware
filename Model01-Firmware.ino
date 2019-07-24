// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#include "Kaleidoscope.h"
// Support for storing the keymap in EEPROM
    #include "Kaleidoscope-EEPROM-Settings.h"
    #include "Kaleidoscope-EEPROM-Keymap.h"
// Support for communicating with the host via a simple Serial protocol
    #include "Kaleidoscope-FocusSerial.h"
// LED
    #include "Kaleidoscope-LED-Stalker.h"
    // Support for an LED mode that lets one configure per-layer color maps
    #include "Kaleidoscope-Colormap.h"

// Support for Keyboardio's internal keyboard testing mode
    #include "Kaleidoscope-HardwareTestMode.h"
// Support for host power management (suspend & wakeup)
    #include "Kaleidoscope-HostPowerManagement.h"
// Support for USB quirks, like changing the key state report protocol
    #include "Kaleidoscope-USB-Quirks.h"

// See defs:
//    https://github.com/keyboardio/Kaleidoscope/blob/master/src/key_defs_keyboard.h
//    https://github.com/keyboardio/Kaleidoscope/blob/master/src/key_defs_consumerctl.h
//    https://github.com/keyboardio/Kaleidoscope/blob/master/src/key_defs_sysctl.h
//    https://github.com/keyboardio/Kaleidoscope/blob/master/src/key_defs_keymaps.h
//
// ___ to let keypresses fall through to the previously active layer
// XXX to mark a keyswitch as 'blocked' on this layer
// ShiftToLayer() and LockLayer() keys to change the active keymap.

// Keyboard layers
enum { QWERTY, LFUNCTION, RFUNCTION };

KEYMAPS(
// ----
    [QWERTY] = KEYMAP_STACKED(
        // Left
        ___,          Key_1, Key_2,  Key_3,  Key_4,  Key_5,   Key_6,
        Key_Backtick, Key_Q, Key_W,  Key_E,  Key_R,  Key_T,   Key_Tab,
        Key_PageUp,   Key_A, Key_S,  Key_D,  Key_F,  Key_G,
        Key_PageDown, Key_Z, Key_X,  Key_C,  Key_V,  Key_B,   Key_Esc,

        Key_LeftControl, Key_Spacebar, Key_LeftGui, Key_LeftShift,
        ShiftToLayer(LFUNCTION),

        // Right
        Key_5,                      Key_6,  Key_7,  Key_8,      Key_9,       Key_0,           Key_Lang2,
        Key_Enter,                  Key_Y,  Key_U,  Key_I,      Key_O,       Key_P,           Key_Equals,
                                    Key_H,  Key_J,  Key_K,      Key_L,       Key_Semicolon,   Key_Quote,
        LALT(LCTRL(Key_LeftGui)),   Key_N,  Key_M,  Key_Comma,  Key_Period,  Key_Slash,       Key_Minus,

        Key_RightShift, Key_RightAlt, Key_Backspace, Key_RightControl,
        ShiftToLayer(RFUNCTION)
    ),

// ----
    [LFUNCTION] = KEYMAP_STACKED(
        // Left
        ___,       Key_F1,  Key_F2,  Key_F3,  Key_F4,  Key_F5,   ___,
        ___,       ___,     ___,     ___,     ___,     ___,      ___,
        ___,       ___,     ___,     ___,     ___,     ___,
        Key_Lang1, ___,     ___,     ___,     ___,     ___,      ___,

        Key_LeftAlt, Key_Enter, ___, ___,
        ___,

        // Right
        ___,   ___,  ___,  ___,  ___,  ___,            Key_Lang1,
        ___,   ___,  ___,  ___,  ___,  ___,            ___,
        ___,   ___,  ___,  ___,  ___,  ___,
        ___,   ___,  ___,  ___,  ___,  Key_Backslash,  Key_Pipe,

        ___, ___, ___, ___,
        ___,
    ),
    [RFUNCTION] = KEYMAP_STACKED(
        // Left
        ___,   ___,  ___,  ___,  ___,  ___,   Key_LEDEffectNext,
        ___,   ___,  ___,  ___,  ___,  ___,   ___,
        ___,   ___,  ___,  ___,  ___,  ___,
        Key_Lang2,   ___,  ___,  ___,  ___,  ___,   ___,

        ___, ___, ___, ___,
        ___,

        // Right
        Consumer_PlaySlashPause,   Key_F6,                  Key_F7,                    Key_F8,                    Key_F9,            Key_F10,             Key_Lang1,
        ___,                       Consumer_ScanNextTrack,  Key_LeftCurlyBracket,      Key_RightCurlyBracket,     Key_LeftBracket,   Key_RightBracket,    ___,
                                   Key_LeftArrow,           Key_DownArrow,             Key_UpArrow,               Key_RightArrow,    ___,                 ___,
        ___,                       Consumer_Mute,           Consumer_VolumeDecrement,  Consumer_VolumeIncrement,  ___,               Key_Backslash,       Key_Pipe,

        ___, ___, Key_Delete, ___,
        ___
    )
)


// -----------------------------------

KALEIDOSCOPE_INIT_PLUGINS(
    // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an
    // editable keymap in EEPROM.
    EEPROMSettings,
    EEPROMKeymap,
    // Focus allows bi-directional communication with the host, and is the
    // interface through which the keymap in EEPROM can be edited.
    Focus,
    // FocusSettingsCommand adds a few Focus commands, intended to aid in
    // changing some settings of the keyboard, such as the default layer (via the
    // `settings.defaultLayer` command)
    FocusSettingsCommand,
    // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
    // both debugging, and in backing up one's EEPROM contents.
    FocusEEPROMCommand,
    // The HostPowerManagement plugin allows us to turn LEDs off when then host
    // goes to sleep, and resume them when it wakes up.
    HostPowerManagement,
    // The USBQuirks plugin lets you do some things with USB that we aren't
    // comfortable - or able - to do automatically, but can be useful
    // nevertheless. Such as toggling the key report protocol between Boot (used
    // by BIOSes) and Report (NKRO).
    USBQuirks,
    // The hardware test mode, which can be invoked by tapping Prog, LED and the
    // left Fn button at the same time.
    HardwareTestMode,
    // LED
    LEDControl,
    ColormapEffect,
    StalkerEffect
);

void setup()
{
    Kaleidoscope.setup();
    
    StalkerEffect.variant = STALKER(BlazingTrail); // see https://github.com/keyboardio/Kaleidoscope-LED-Stalker
    StalkerEffect.activate();

    EEPROMKeymap.setup(3);
    ColormapEffect.max_layers(3);
}

void loop() { Kaleidoscope.loop(); }

