#pragma once

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"

Logger& getLogger();

struct swingRatingCounter_context {
    GlobalNamespace::NoteCutInfo noteCutInfo;
    GlobalNamespace::FlyingScoreEffect* flyingScoreEffect;
};