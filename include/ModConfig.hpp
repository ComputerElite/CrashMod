#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,

    CONFIG_VALUE(Active, bool, "Active", false); // Change back to true once bt audio issue is in QuestSounds
    CONFIG_VALUE(Percentage, float, "Percentage", 95.0f);
    CONFIG_VALUE(PercentageActive, bool, "Percentage Crash Active", true);
    CONFIG_VALUE(MissCrash, bool, "Crash on Miss", true);
    CONFIG_VALUE(CrashOnPlay, bool, "Crash on Play", true);
    CONFIG_VALUE(CrashOnTurn, bool, "Crash on Turn", true);
    CONFIG_VALUE(CrashOnPause, bool, "CrashOnPause", true);
    CONFIG_VALUE(CrashOnUnpause, bool, "CrashOnUnpause", true);
    CONFIG_VALUE(CrashOnNotFullCombo, bool, "Crash on not Full Combo", true);
    CONFIG_VALUE(CrashOnNewHighscore, bool, "Crash on new highscore", true);
    CONFIG_VALUE(CrashCounter, int, "CrashCounter", 0);
    CONFIG_VALUE(OnCrashAction, int, "OnCrashAction", 0);
    CONFIG_VALUE(CrashOnNoFailOn, bool, "Crash if you start a level with no fail", true);
    CONFIG_VALUE(CrashOnOver5PerBattery, bool, "Crash if your Quest has more battery than above", false);
    CONFIG_VALUE(CrashOnGoodCut, bool, "Crash on good cut", true);
    CONFIG_VALUE(BatteryThreshold, float, "Battery threshold", 5.0f);
    CONFIG_VALUE(CrashOnFirstBlockMiss, bool, "Crash on miss of first block", true);
    CONFIG_VALUE(CrashOnLowBPM, bool, "Crash on high BPM maps", true);
    CONFIG_VALUE(CrashOnHighBPM, bool, "Crash on low BPM maps", true);
    CONFIG_VALUE(LowBPMValue, float, "Min BPM", 100.0f);
    CONFIG_VALUE(HighBPMValue, float, "Max BPM", 200.0f);
    CONFIG_VALUE(CrashOn115, bool, "Crash on 115", true);
    CONFIG_VALUE(CrashOnNE, bool, "Crash on NE", false);
    CONFIG_VALUE(CrashOnBomb, bool, "Crash on Bomb Cut", true);

    CONFIG_VALUE(AudioFix, bool, "Enable BT Audio Fix", true);
    CONFIG_VALUE(AudioBuffer, int, "Audio Buffer", 128);
)