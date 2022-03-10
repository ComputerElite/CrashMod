#include "main.hpp"
#include "extern/includes/config-utils/shared/config-utils.hpp"
#include "ModConfig.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/ScoreModel_NoteScoreDefinition.hpp"
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
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/ScoreFormatter.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/BeatmapObjectExecutionRatingsRecorder.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"

#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/Application.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include <map>

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
    getLogger().info("Crash number %d by Crash mod", getModConfig().CrashCounter.GetValue());
    if(getModConfig().OnCrashAction.GetValue() == 0) CRASH_UNLESS(false);
    else if (getModConfig().OnCrashAction.GetValue() == 1) Application::Quit();
    else if (getModConfig().OnCrashAction.GetValue() == 2) while (true) malloc(1024);
}

bool firstNote = true;
float bpm = 0.0f;

MAKE_HOOK_MATCH(ScoreController_HandleNoteWasMissed, &ScoreController::HandleNoteWasMissed, void, ScoreController* self, NoteController* note) {
    ScoreController_HandleNoteWasMissed(self, note);
    if(getModConfig().Active.GetValue() && (getModConfig().MissCrash.GetValue() || getModConfig().CrashOnFirstBlockMiss.GetValue() && firstNote)) Crash();
    firstNote = false;
}

MAKE_HOOK_MATCH(ScoreController_HandleNoteWasCut, &ScoreController::HandleNoteWasCut, void, ScoreController* self, NoteController* note, ByRef<GlobalNamespace::NoteCutInfo> info) {
    ScoreController_HandleNoteWasCut(self, note, info);
    if(getModConfig().Active.GetValue() && (info.heldRef.get_allIsOK() && getModConfig().CrashOnGoodCut.GetValue() || firstNote && !info.heldRef.get_allIsOK() && getModConfig().CrashOnFirstBlockMiss.GetValue())) Crash();
    firstNote = false;
}

MAKE_HOOK_MATCH(BombNoteController_HandleWasCutBySaber, &BombNoteController::HandleWasCutBySaber, void, BombNoteController* self, Saber* saber, Vector3 cutPoint, Quaternion orientation, Vector3 cutDirVec) {
    BombNoteController_HandleWasCutBySaber(self, saber, cutPoint, orientation, cutDirVec);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnBomb.GetValue()) Crash();
}

MAKE_HOOK_MATCH(HandleSwingFinish, &GlobalNamespace::CutScoreBuffer::HandleSaberSwingRatingCounterDidFinish, void, GlobalNamespace::CutScoreBuffer* self, ISaberSwingRatingCounter* counter) {
    HandleSwingFinish(self, counter);
    if(!getModConfig().Active.GetValue() || !getModConfig().CrashOn115.GetValue()) return;
    int score = self->dyn__afterCutScore() + self->dyn__beforeCutScore() + self->dyn__centerDistanceCutScore();
    auto *noteScoreDefinition = self->dyn__noteScoreDefinition();
    int maxCutScore = noteScoreDefinition->dyn_maxAfterCutScore() + noteScoreDefinition->dyn_maxBeforeCutScore() + noteScoreDefinition->dyn_maxCenterDistanceCutScore();
    if(score == maxCutScore) Crash();
}

MAKE_HOOK_MATCH(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, &RelativeScoreAndImmediateRankCounter::UpdateRelativeScoreAndImmediateRank, void, RelativeScoreAndImmediateRankCounter* self, int score, int modifiedscore, int maxscore, int maxmodfifiedscore) {
    RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedscore, maxscore, maxmodfifiedscore);
    if(getModConfig().Active.GetValue() && getModConfig().PercentageActive.GetValue() && self->get_relativeScore() < getModConfig().Percentage.GetValue() / 100) Crash();
}

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* dbm, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool startPaused, bool useTestNoteCutSoundEffects) {
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, dbm, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, startPaused, useTestNoteCutSoundEffects);
    if(getModConfig().Active.GetValue()) {
        if(getModConfig().CrashOnPlay.GetValue() || getModConfig().CrashOnNoFailOn.GetValue() && gameplayModifiers->get_noFailOn0Energy()) Crash();
        if(getModConfig().CrashOnHighBPM.GetValue() && bpm > getModConfig().HighBPMValue.GetValue() || getModConfig().CrashOnLowBPM.GetValue() && bpm < getModConfig().LowBPMValue.GetValue()) Crash();
        firstNote = true;
    }
}

MAKE_HOOK_MATCH(PlayerTransforms_Update, &PlayerTransforms::Update, void, PlayerTransforms* self) {
    PlayerTransforms_Update(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnTurn.GetValue() && self->get_headWorldRot().get_eulerAngles().y > 165 && self->get_headWorldRot().get_eulerAngles().y < 195) Crash();
}

MAKE_HOOK_MATCH(PauseController_HandleMenuButtonTriggered, &PauseController::HandleMenuButtonTriggered, void, PauseController* self) {
    PauseController_HandleMenuButtonTriggered(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnPause.GetValue()) Crash();
}

MAKE_HOOK_MATCH(PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, &PauseController::HandlePauseMenuManagerDidFinishResumeAnimation, void, PauseController* self) {
    PauseController_HandlePauseMenuManagerDidFinishResumeAnimation(self);
    if(getModConfig().Active.GetValue() && getModConfig().CrashOnUnpause.GetValue()) Crash();
}

MAKE_HOOK_MATCH(SceneManager_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene previousActiveScene, UnityEngine::SceneManagement::Scene nextActiveScene) {
    SceneManager_ActiveSceneChanged(previousActiveScene, nextActiveScene);
    if(getModConfig().Active.GetValue()) {
        if(getModConfig().CrashOnOver5PerBattery.GetValue() && GlobalNamespace::OVRPlugin::OVRP_1_1_0::ovrp_GetSystemBatteryLevel() > getModConfig().BatteryThreshold.GetValue() / 100) Crash();
        if(getModConfig().CrashOnNE.GetValue() && Modloader::getMods().find("noodleextensions") == Modloader::getMods().end()) Crash();
    }
}

MAKE_HOOK_MATCH(ResultsViewController_Init, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* result, IReadonlyBeatmapData* beatmap, IDifficultyBeatmap* bm, bool practice, bool newHighScore) {
    ResultsViewController_Init(self, result, beatmap, bm, practice, newHighScore);
    if(getModConfig().Active.GetValue() && ((getModConfig().CrashOnNotFullCombo.GetValue() && !result->dyn_fullCombo()) || (getModConfig().CrashOnNewHighscore.GetValue() && self->dyn__newHighScore()))) Crash();
}

MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void, StandardLevelDetailView* self) {
    StandardLevelDetailView_RefreshContent(self);
    IBeatmapLevel* beatmapLevel = self->get_selectedDifficultyBeatmap()->get_level();
    IPreviewBeatmapLevel* previewBeatmapLevel = reinterpret_cast<IPreviewBeatmapLevel*>(beatmapLevel);
    bpm = previewBeatmapLevel->get_beatsPerMinute();
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
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);
    // Install our hooks
    INSTALL_HOOK(logger, StandardLevelDetailView_RefreshContent)
    INSTALL_HOOK(logger, PlayerTransforms_Update);
    INSTALL_HOOK(logger, ScoreController_HandleNoteWasMissed);
    INSTALL_HOOK(logger, ScoreController_HandleNoteWasCut);
    INSTALL_HOOK(logger, RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank);
    INSTALL_HOOK(logger, StandardLevelScenesTransitionSetupDataSO_Init);
    INSTALL_HOOK(logger, SceneManager_ActiveSceneChanged);
    INSTALL_HOOK(logger, ResultsViewController_Init);
    //INSTALL_HOOK(logger, InitFlyingScoreEffect);
    INSTALL_HOOK(logger, HandleSwingFinish);
    INSTALL_HOOK(logger, BombNoteController_HandleWasCutBySaber)
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandleMenuButtonTriggered, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandleMenuButtonTriggered", 0));
    //INSTALL_HOOK_OFFSETLESS(logger, PauseController_HandlePauseMenuManagerDidFinishResumeAnimation, il2cpp_utils::FindMethodUnsafe("", "PauseController", "HandlePauseMenuManagerDidFinishResumeAnimation", 0));
    getLogger().info("Installed all hooks!");
}