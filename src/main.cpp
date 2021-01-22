#include "main.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/PauseController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/Application.hpp"

#include "CrashModViewController.hpp"

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace QuestUI;

static ModInfo modInfo;
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void Crash() {
    getConfig().config["CrashCounter"].SetInt(getConfig().config["CrashCounter"].GetInt() + 1);
    getLogger().info("Crash number " + std::to_string(getConfig().config["CrashCounter"].GetInt()) + " by Crash mod");
    getConfig().Write();
    if(getConfig().config["OnCrashAction"].GetInt() == 0) {
        CRASH_UNLESS(false);
    } else if (getConfig().config["OnCrashAction"].GetInt() == 1){
        Application::Quit();
    } else if (getConfig().config["OnCrashAction"].GetInt() == 2){
        while (true)
        {
            malloc(1024);
        }
    }
}

MAKE_HOOK_OFFSETLESS(ScoreController_Update, void, ScoreController* self) {
    ScoreController_Update(self);
}

MAKE_HOOK_OFFSETLESS(ScoreController_HandleNoteWasMissed, void, ScoreController* self, NoteController* note) {
    if(getConfig().config["Active"].GetBool()) {
        if(getConfig().config["MissCrash"].GetBool()) CRASH_UNLESS(false);
    }
    ScoreController_HandleNoteWasMissed(self, note);
}

MAKE_HOOK_OFFSETLESS(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, void, RelativeScoreAndImmediateRankCounter* self, int score, int modifiedscore, int maxscore, int maxmodfifiedscore) {
    if(getConfig().config["Active"].GetBool()) {

        float percentage = self->get_relativeScore();
        getLogger().info("current Score percentage: " + std::to_string(percentage));
        if(getConfig().config["PercentageActive"].GetBool()) {
            if(percentage < getConfig().config["Percentage"].GetFloat()) Crash();
        }
    }
    RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedscore, maxscore, maxmodfifiedscore);
}

MAKE_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO_Init, void, StandardLevelScenesTransitionSetupDataSO* self, Il2CppString* gameMode, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, Il2CppString* backButtonText, bool useTestNoteCutSoundEffects) {
    if(getConfig().config["Active"].GetBool()) {
        if(getConfig().config["CrashOnPlay"].GetBool()) {
            Crash();
        }
    }
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
}

MAKE_HOOK_OFFSETLESS(PlayerTransforms_Update, void, PlayerTransforms* self) {
    PlayerTransforms_Update(self);
    if(getConfig().config["Active"].GetBool()) {
        if(getConfig().config["CrashOnTurn"].GetBool()) {
            getLogger().info("Current y turn: " + std::to_string(self->headWorldRot.get_eulerAngles().y));
            if(self->headWorldRot.get_eulerAngles().y > 165 && self->headWorldRot.get_eulerAngles().y < 195) {
                Crash();
            }
            
        }
    }
}

MAKE_HOOK_OFFSETLESS(PauseController_HandleMenuButtonTriggered, void, PauseController* self) {
    PauseController_HandleMenuButtonTriggered(self);
    if(getConfig().config["Active"].GetBool()) {
        if(getConfig().config["CrashOnPause"].GetBool()) {
            Crash();
        }
    }
    
}

MAKE_HOOK_OFFSETLESS(PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, void, PauseController* self) {
    PauseController_HandlePauseMenuManagerDidFinishResumeAnimation(self);
    if(getConfig().config["Active"].GetBool()) {
        if(getConfig().config["CrashOnUnpause"].GetBool()) {
            Crash();
        }
    }
}
    

void createDefaultConfig()  {

    rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("Percentage"))) {
        getConfig().config.AddMember("Percentage", rapidjson::Value().SetFloat(0.95), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("PercentageActive"))) {
        getConfig().config.AddMember("PercentageActive", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("MissCrash"))) {
        getConfig().config.AddMember("MissCrash", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("CrashOnPlay"))) {
        getConfig().config.AddMember("CrashOnPlay", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("CrashOnTurn"))) {
        getConfig().config.AddMember("CrashOnTurn", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("CrashOnPause"))) {
        getConfig().config.AddMember("CrashOnPause", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("CrashOnUnpause"))) {
        getConfig().config.AddMember("CrashOnUnpause", rapidjson::Value().SetBool(true), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("CrashCounter"))) {
        getConfig().config.AddMember("CrashCounter", rapidjson::Value().SetInt(0), allocator);
    }

    if(getConfig().config.HasMember("Active") && !(getConfig().config.HasMember("OnCrashAction"))) {
        getConfig().config.AddMember("OnCrashAction", rapidjson::Value().SetInt(0), allocator);
    }

    if(getConfig().config.HasMember("Active")) {return;}

    // Add all the default options
    getConfig().config.RemoveAllMembers(); // Empty the config - it should already be empty but just to be sure
    getConfig().config.SetObject(); // Set the base of the config to a value that can contain keys

    // Get what is used to allocate memory in the config file
    // rapidjson::Document::AllocatorType& allocator = getConfig().config.GetAllocator();

    // Add a member to the config, using the allocator

    getConfig().config.AddMember("Active", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("Percentage", rapidjson::Value().SetFloat(0.95), allocator);
    getConfig().config.AddMember("PercentageActive", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("MissCrash", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("CrashOnPlay", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("CrashOnTurn", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("CrashOnPause", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("CrashOnUnpause", rapidjson::Value().SetBool(true), allocator);
    getConfig().config.AddMember("CrashCounter", rapidjson::Value().SetInt(0), allocator);
    getConfig().config.AddMember("OnCrashAction", rapidjson::Value().SetInt(0), allocator);

    getConfig().Write(); // Write the config back to disk
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    createDefaultConfig();
    getLogger().info("Completed setup!");
}

extern "C" void load() {
    getLogger().info("Installing hooks...");
    il2cpp_functions::Init();
    QuestUI::Init();

    LoggerContextObject logger = getLogger().WithContext("load");
    custom_types::Register::RegisterType<CrashMod::CrashModViewController>();
    // Register our mod settings menu
    QuestUI::Register::RegisterModSettingsViewController<CrashMod::CrashModViewController*>(modInfo);
    // Install our hooks
    INSTALL_HOOK_OFFSETLESS(logger, ScoreController_Update, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, PlayerTransforms_Update, il2cpp_utils::FindMethodUnsafe("", "PlayerTransforms", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, ScoreController_HandleNoteWasMissed, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "HandleNoteWasMissed", 1));
    INSTALL_HOOK_OFFSETLESS(logger, RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, il2cpp_utils::FindMethodUnsafe("", "RelativeScoreAndImmediateRankCounter", "UpdateRelativeScoreAndImmediateRank", 4));
    INSTALL_HOOK_OFFSETLESS(logger, StandardLevelScenesTransitionSetupDataSO_Init, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 9));
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandleMenuButtonTriggered, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandleMenuButtonTriggered", 0));
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandlePauseMenuManagerDidFinishResumeAnimation", 0));
    getLogger().info("Installed all hooks!");
}