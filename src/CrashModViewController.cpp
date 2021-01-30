#include "CrashModViewController.hpp"

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Toggle_ToggleEvent.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/ModalView.hpp"
#include "HMUI/Touchable.hpp"
#include "main.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include <stdlib.h>

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace HMUI;
using namespace CrashMod;

DEFINE_CLASS(CrashModViewController);

void onActiveSChange(CrashModViewController* self, bool newValue) {
    getConfig().config["Active"].SetBool(newValue);
}

void onPercentageActiveChange(CrashModViewController* self, bool newValue) {
    getConfig().config["PercentageActive"].SetBool(newValue);
}

void onCrashOnPlayChange(CrashModViewController* self, bool newValue) {
    getConfig().config["CrashOnPlay"].SetBool(newValue);
}

void onCrashOnPauseChange(CrashModViewController* self, bool newValue) {
    getConfig().config["CrashOnPause"].SetBool(newValue);
}

void onCrashOnUnpauseChange(CrashModViewController* self, bool newValue) {
    getConfig().config["CrashOnUnpause"].SetBool(newValue);
}

void onCrashOnTurnChange(CrashModViewController* self, bool newValue) {
    getConfig().config["CrashOnTurn"].SetBool(newValue);
}

void onMissCrashChange(CrashModViewController* self, bool newValue) {
    getConfig().config["MissCrash"].SetBool(newValue);
}

void onPercentageChange(CrashModViewController* self, float newValue)   {
    getConfig().config["Percentage"].SetFloat(newValue / 100.0f);
}

void onOnCrashActionChange(CrashModViewController* self, bool newValue)   {
    getConfig().config["OnCrashAction"].SetInt(newValue ? 1 : 0);
}

void onCrashNow(CrashModViewController* self)  {
    getConfig().config["CrashCounter"].SetInt(getConfig().config["CrashCounter"].GetInt() + 1);
    getConfig().Write();
    CRASH_UNLESS(false);
}


void CrashModViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation) {
        get_gameObject()->AddComponent<Touchable*>();
        GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(get_transform());

        // Active
        auto ActiveChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onActiveSChange);
        UnityEngine::UI::Toggle* Active = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Active", getConfig().config["Active"].GetBool(), ActiveChange);
        QuestUI::BeatSaberUI::AddHoverHint(Active->get_gameObject(), "Why would you want to disable it at all?");

        // PercentageActive
        auto PercentageActiveChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onPercentageActiveChange);
        UnityEngine::UI::Toggle* PercentageActive = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Percentage Crash Active", getConfig().config["PercentageActive"].GetBool(), PercentageActiveChange);
        QuestUI::BeatSaberUI::AddHoverHint(PercentageActive->get_gameObject(), "Enabled the Crash for percentage");

        // Percentage
        auto PercentageChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<float>*>(
                    classof(UnityEngine::Events::UnityAction_1<float>*), this, onPercentageChange);
        QuestUI::IncrementSetting* percentage = QuestUI::BeatSaberUI::CreateIncrementSetting(container->get_transform(), "Percentage", 0, 1.0, getConfig().config["Percentage"].GetFloat() * 100.0f, 0.0f, 100.0f, PercentageChange);
        QuestUI::BeatSaberUI::AddHoverHint(percentage->get_gameObject(), "Under this percentage your game will crash.");

        // MissCrash
        auto MissCrashChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onMissCrashChange);
        UnityEngine::UI::Toggle* MissCrash = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Crash on Miss", getConfig().config["MissCrash"].GetBool(), MissCrashChange);
        QuestUI::BeatSaberUI::AddHoverHint(MissCrash->get_gameObject(), "Crashes the game on a miss");

        // CrashOnPlay
        auto CrashOnPlayChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onCrashOnPlayChange);
        UnityEngine::UI::Toggle* CrashOnPlay = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Crash on Play", getConfig().config["CrashOnPlay"].GetBool(), CrashOnPlayChange);
        QuestUI::BeatSaberUI::AddHoverHint(CrashOnPlay->get_gameObject(), "Crashes the game when you start a level");

        // CrashOnTurn
        auto CrashOnTurnChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onCrashOnTurnChange);
        UnityEngine::UI::Toggle* CrashOnTurn = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Crash on Turn", getConfig().config["CrashOnTurn"].GetBool(), CrashOnTurnChange);
        QuestUI::BeatSaberUI::AddHoverHint(CrashOnTurn->get_gameObject(), "Crashes the game when you turn around in a song");

        // CrashOnPause
        /*
        auto CrashOnPauseChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onCrashOnPauseChange);
        UnityEngine::UI::Toggle* CrashOnPause = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Crash on Pause", getConfig().config["CrashOnPause"].GetBool(), CrashOnPauseChange);
        QuestUI::BeatSaberUI::AddHoverHint(CrashOnPause->get_gameObject(), "Crashes the game when you pause it");

        // CrashOnUnpause
        auto CrashOnUnpauseChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onCrashOnUnpauseChange);
        UnityEngine::UI::Toggle* CrashOnUnpause = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Crash on Resume", getConfig().config["CrashOnUnpause"].GetBool(), CrashOnUnpauseChange);
        QuestUI::BeatSaberUI::AddHoverHint(CrashOnUnpause->get_gameObject(), "Crashes the game when you resume.");
        */
       
        // OnCrashAction
        auto OnCrashActionChange = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction_1<bool>*>(
                   classof(UnityEngine::Events::UnityAction_1<bool>*), this, onOnCrashActionChange);
        UnityEngine::UI::Toggle* OnCrashAction = QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Quit instead of Crash", getConfig().config["OnCrashAction"].GetInt() == 1 ? true : false, OnCrashActionChange);
        QuestUI::BeatSaberUI::AddHoverHint(OnCrashAction->get_gameObject(), "Why would you even want to do that?");

        // Crash Now
        auto CrashNow = il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction*>(
                    classof(UnityEngine::Events::UnityAction*), this, onCrashNow);
        this->modeSelectButton = QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), "Crash now", CrashNow);

        // Crash Counter
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Crash Count: " + std::to_string(getConfig().config["CrashCounter"].GetInt()));
    }
}

void CrashModViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
    getConfig().Write();
}