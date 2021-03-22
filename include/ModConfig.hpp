#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,

    DECLARE_VALUE(Active, bool, "Active", true);
    DECLARE_VALUE(Percentage, float, "Percentage", 95.0f);
    DECLARE_VALUE(PercentageActive, bool, "Percentage Crash Active", true);
    DECLARE_VALUE(MissCrash, bool, "Crash on Miss", true);
    DECLARE_VALUE(CrashOnPlay, bool, "Crash on Play", true);
    DECLARE_VALUE(CrashOnTurn, bool, "Crash on Turn", true);
    DECLARE_VALUE(CrashOnPause, bool, "CrashOnPause", true);
    DECLARE_VALUE(CrashOnUnpause, bool, "CrashOnUnpause", true);
    DECLARE_VALUE(CrashCounter, int, "CrashCounter", 0);
    DECLARE_VALUE(OnCrashAction, int, "OnCrashAction", 0);
    DECLARE_VALUE(CrashOnNoFailOn, bool, "Crash if you start a level with no fail", true);
    DECLARE_VALUE(CrashOnOver5PerBattery, bool, "Crash if your Quest has more battery than above", false);
    DECLARE_VALUE(CrashOnGoodCut, bool, "Crash on good cut", true);
    DECLARE_VALUE(BatteryThreshold, float, "Battery threshold", 5.0f);

    INIT_FUNCTION(
        INIT_VALUE(Active);
        INIT_VALUE(Percentage);
        INIT_VALUE(PercentageActive);
        INIT_VALUE(MissCrash);
        INIT_VALUE(CrashOnPlay);
        INIT_VALUE(CrashOnTurn);
        INIT_VALUE(CrashOnPause);
        INIT_VALUE(CrashOnUnpause);
        INIT_VALUE(CrashCounter);
        INIT_VALUE(OnCrashAction);
        INIT_VALUE(CrashOnNoFailOn);
        INIT_VALUE(CrashOnOver5PerBattery);
        INIT_VALUE(CrashOnGoodCut);
        INIT_VALUE(BatteryThreshold);
    )
)