#include "main.hpp"
#include "Hooks.hpp"
#include "ModConfig.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
using namespace GlobalNamespace;

#include "sombrero/shared/FastColor.hpp"
using namespace Sombrero;

#include "chroma/shared/SaberAPI.hpp"
#include "chroma/shared/BombAPI.hpp"
#include "chroma/shared/NoteAPI.hpp"
#include "chroma/shared/ObstacleAPI.hpp"
#include "chroma/shared/LightAPI.hpp"

#include <array>
#include <unordered_map>

float i;
float ii;

const int STEPS = 256;

/*const int NUM_STEPS = 256;
std::array<FastColor, NUM_STEPS> WarmColours;
std::array<FastColor, NUM_STEPS> CoolColours;
std::unordered_map<int, FastColor> WarmColourCache;
std::unordered_map<int, FastColor> CoolColourCache;

FastColor getWarmColour(int index)
{
  auto cachedResult = WarmColourCache.find(index);
  if (cachedResult != WarmColourCache.end())
  {
    return cachedResult->second;
  }

  float t = (float)index / NUM_STEPS;
  FastColor colour = FastColor(1.0f, 1.0f - t * 0.5f, 0.0f, t);
  WarmColourCache[index] = colour;
  return colour;
}

FastColor getCoolColour(int index)
{
  auto cachedResult = CoolColourCache.find(index);
  if (cachedResult != CoolColourCache.end())
  {
    return cachedResult->second;
  }

  float t = (float)index / NUM_STEPS;
  FastColor colour = FastColor(0.0f, 1.0f - t, t * 0.5f, 1.0f);
  CoolColourCache[index] = colour;
  return colour;
}

void generateColours()
{
  for (int i = 0; i < NUM_STEPS; i++)
  {
    WarmColours[i] = getWarmColour(i);
    CoolColours[i] = getCoolColour(i);
  }
}
*/

std::vector<FastColor> warmColours, coolColours;

void PrecomputeWarmColours()
{
  warmColours.resize(STEPS + 1);
  for (int i = 0; i < STEPS; i++)
  {
    FastColor colour;

    colour = FastColor::Lerp(FastColor(1.0f, 1.0f, 0.0f), FastColor(0.89f, 0.04f, 0.36f), (float)i / STEPS);

    warmColours[i] = colour;
  }
}

void PrecomputeCoolColours()
{
  coolColours.resize(STEPS + 1);
  for (int i = 0; i < STEPS; i++)
  {
    FastColor colour;

    colour = FastColor::Lerp(FastColor(0.0f, 1.0f, 0.0f), FastColor(1.0f, 0.0f, 1.0f), (float)i / STEPS);

    coolColours[i] = colour;
  }
}

FastColor WarmGen(int index)
{
  return warmColours[index];
}
FastColor CoolGen(int index)
{
  return coolColours[index];
}



bool WarmToggle = true, CoolToggle = true;

MAKE_AUTO_HOOK_MATCH(WC_GameplayCoreInstaller_InstallBindings, &GameplayCoreInstaller::InstallBindings, void, GameplayCoreInstaller *self)
{
  WC_GameplayCoreInstaller_InstallBindings(self);

  //generateColours();
  PrecomputeWarmColours();
  PrecomputeCoolColours();

  i = getModConfig().LeftWarmOffset.GetValue();
  ii = getModConfig().RightCoolOffset.GetValue();
}

MAKE_AUTO_HOOK_MATCH(WC_AudioTimeSyncController_Update, &GlobalNamespace::AudioTimeSyncController::Update, void, GlobalNamespace::AudioTimeSyncController *self)
{
  WC_AudioTimeSyncController_Update(self);

  if (getModConfig().ModToggle.GetValue() && getModConfig().TechniNotes.GetValue() == "Warm/Cold")
  {
    /*FastColor LeftColour = WarmGen(i);
    FastColor RightColour = CoolGen(ii);*/

    FastColor LeftColour = WarmGen(i);
    FastColor RightColour = CoolGen(ii);

    Chroma::NoteAPI::setGlobalNoteColorSafe(LeftColour, RightColour);
    Chroma::SaberAPI::setGlobalSaberColorSafe(SaberType::SaberA, LeftColour);
    Chroma::SaberAPI::setGlobalSaberColorSafe(SaberType::SaberB, RightColour);

    if (WarmToggle)
      i++;
    else if (!WarmToggle)
      i--;

    if (CoolToggle)
      ii++;
    else if (!CoolToggle)
      ii--;

    if (i >= 254)
      WarmToggle = false;
    else if (i <= 1)
      WarmToggle = true;

    if (ii >= 254)
      CoolToggle = false;
    else if (i <= 1)
      CoolToggle = true;
  }
}