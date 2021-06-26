#include "main.hpp"
#include "extern/config-utils/shared/config-utils.hpp"
#include "ModConfig.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/PauseController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/OVRPlugin_OVRP_1_1_0.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/Application.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"

#include "CrashModViewController.hpp"

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace QuestUI;

static ModInfo modInfo;
DEFINE_CONFIG(ModConfig);

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

void Crash() {
    getModConfig().CrashCounter.SetValue(getModConfig().CrashCounter.GetValue() + 1);
    getLogger().info("Crash number " + std::to_string(getModConfig().CrashCounter.GetValue()) + " by Crash mod");
    if(getModConfig().OnCrashAction.GetValue() == 0) CRASH_UNLESS(false);
    else if (getModConfig().OnCrashAction.GetValue() == 1) Application::Quit();
    else if (getModConfig().OnCrashAction.GetValue() == 2) while (true) malloc(1024);
}

MAKE_HOOK_OFFSETLESS(ScoreController_HandleNoteWasMissed, void, ScoreController* self, NoteController* note) {
    ScoreController_HandleNoteWasMissed(self, note);
    if(getModConfig().Active.GetValue() && getModConfig().MissCrash.GetValue()) Crash();
}

MAKE_HOOK_OFFSETLESS(ScoreController_HandleNoteWasCut, void, ScoreController* self, NoteController* note, NoteCutInfo* info) {
    ScoreController_HandleNoteWasCut(self, note, info);
    if(getModConfig().Active.GetValue() && info->get_allIsOK() && getModConfig().CrashOnGoodCut.GetValue()) Crash();
}

MAKE_HOOK_OFFSETLESS(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, void, RelativeScoreAndImmediateRankCounter* self, int score, int modifiedscore, int maxscore, int maxmodfifiedscore) {
    RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedscore, maxscore, maxmodfifiedscore);
    if(getModConfig().Active.GetValue() && getModConfig().PercentageActive.GetValue() && self->get_relativeScore() < getModConfig().Percentage.GetValue() / 100) Crash();
}

MAKE_HOOK_OFFSETLESS(StandardLevelScenesTransitionSetupDataSO_Init, void, StandardLevelScenesTransitionSetupDataSO* self, Il2CppString* gameMode, IDifficultyBeatmap* dbm, IPreviewBeatmapLevel previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, Il2CppString* backButtonText, bool useTestNoteCutSoundEffects) {
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, dbm, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
    if(getModConfig().Active.GetValue()) {
        if(getModConfig().CrashOnPlay.GetValue() || getModConfig().CrashOnNoFailOn.GetValue() && gameplayModifiers->noFailOn0Energy) Crash();
    }
}

MAKE_HOOK_OFFSETLESS(PlayerTransforms_Update, void, PlayerTransforms* self) {
    PlayerTransforms_Update(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnTurn.GetValue() && self->headWorldRot.get_eulerAngles().y > 165 && self->headWorldRot.get_eulerAngles().y < 195) Crash();
}

MAKE_HOOK_OFFSETLESS(PauseController_HandleMenuButtonTriggered, void, PauseController* self) {
    PauseController_HandleMenuButtonTriggered(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnPause.GetValue()) Crash();
}

MAKE_HOOK_OFFSETLESS(PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, void, PauseController* self) {
    PauseController_HandlePauseMenuManagerDidFinishResumeAnimation(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnUnpause.GetValue()) Crash();
}

MAKE_HOOK_OFFSETLESS(SceneManager_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene previousActiveScene, UnityEngine::SceneManagement::Scene nextActiveScene) {
    SceneManager_ActiveSceneChanged(previousActiveScene, nextActiveScene);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnOver5PerBattery.GetValue() && GlobalNamespace::OVRPlugin::OVRP_1_1_0::ovrp_GetSystemBatteryLevel() > getModConfig().BatteryThreshold.GetValue() / 100) Crash();
}

MAKE_HOOK_OFFSETLESS(ResultsViewController_Init, void, ResultsViewController* self, LevelCompletionResults* result, IDifficultyBeatmap* beatmap, bool practice, bool newHighScore) {
    ResultsViewController_Init(self, result, beatmap, practice, newHighScore);
    if(getModConfig().Active.GetValue() && ((getModConfig().CrashOnNotFullCombo.GetValue() && !result->fullCombo) || (getModConfig().CrashOnNewHighscore.GetValue() && self->newHighScore))) Crash();
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getLogger().info("Completed setup!");
}

extern "C" void load() {
    getLogger().info("Installing hooks...");
    il2cpp_functions::Init();
    getModConfig().Init(modInfo);
    QuestUI::Init();

    LoggerContextObject logger = getLogger().WithContext("load");
    //custom_types::Register::RegisterType<CrashMod::CrashModViewController>();
    // Register our mod settings menu
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    // Install our hooks
    INSTALL_HOOK_OFFSETLESS(logger, PlayerTransforms_Update, il2cpp_utils::FindMethodUnsafe("", "PlayerTransforms", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, ScoreController_HandleNoteWasMissed, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "HandleNoteWasMissed", 1));
    INSTALL_HOOK_OFFSETLESS(logger, ScoreController_HandleNoteWasCut, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "HandleNoteWasCut", 2));
    INSTALL_HOOK_OFFSETLESS(logger, RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, il2cpp_utils::FindMethodUnsafe("", "RelativeScoreAndImmediateRankCounter", "UpdateRelativeScoreAndImmediateRank", 4));
    INSTALL_HOOK_OFFSETLESS(logger, StandardLevelScenesTransitionSetupDataSO_Init, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 10));
    INSTALL_HOOK_OFFSETLESS(logger, SceneManager_ActiveSceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(logger, ResultsViewController_Init, il2cpp_utils::FindMethodUnsafe("", "ResultsViewController", "Init", 4));
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandleMenuButtonTriggered, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandleMenuButtonTriggered", 0));
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandlePauseMenuManagerDidFinishResumeAnimation", 0));
    getLogger().info("Installed all hooks!");
}