/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef GRIM_LUA_V2
#define GRIM_LUA_V2

#include "engines/grim/lua_v1.h"

namespace Grim {

class Lua_V2 : public Lua_V1 {
public:
	typedef Lua_V2 LuaClass;
	void registerOpcodes() override;

protected:
	virtual bool findCostume(lua_Object costumeObj, Actor *actor, Costume **costume) override;
	void setChoreAndCostume(lua_Object choreObj, lua_Object costumeObj, Actor *actor, Costume *&costume, int &chore);

	DECLARE_LUA_OPCODE(UndimAll);
	DECLARE_LUA_OPCODE(SetActorLocalAlpha);
	DECLARE_LUA_OPCODE(UndimRegion);
	DECLARE_LUA_OPCODE(DimScreen);
	DECLARE_LUA_OPCODE(MakeCurrentSetup);
	DECLARE_LUA_OPCODE(SetActorGlobalAlpha);
	DECLARE_LUA_OPCODE(ImGetMillisecondPosition);
	DECLARE_LUA_OPCODE(RemoveActorFromOverworld);
	DECLARE_LUA_OPCODE(UnloadActor);
	DECLARE_LUA_OPCODE(SetActorWalkRate);
	DECLARE_LUA_OPCODE(GetActorWalkRate);
	DECLARE_LUA_OPCODE(SetActorTurnRate);
	DECLARE_LUA_OPCODE(SetReverb);
	DECLARE_LUA_OPCODE(LockBackground);
	DECLARE_LUA_OPCODE(UnLockBackground);
	DECLARE_LUA_OPCODE(LockChore);
	DECLARE_LUA_OPCODE(IsActorChoring);
	DECLARE_LUA_OPCODE(IsChoreValid);
	DECLARE_LUA_OPCODE(IsChorePlaying);
	DECLARE_LUA_OPCODE(StopChore);
	DECLARE_LUA_OPCODE(AdvanceChore);
	DECLARE_LUA_OPCODE(SetActorSortOrder);
	DECLARE_LUA_OPCODE(ActorActivateShadow);
	DECLARE_LUA_OPCODE(ActorStopMoving);
	DECLARE_LUA_OPCODE(ActorLookAt);
	DECLARE_LUA_OPCODE(PutActorInOverworld);
	DECLARE_LUA_OPCODE(GetActorWorldPos);
	DECLARE_LUA_OPCODE(MakeScreenTextures);
	DECLARE_LUA_OPCODE(PutActorInSet);
	DECLARE_LUA_OPCODE(LoadBundle);
	DECLARE_LUA_OPCODE(AreWeInternational);
	DECLARE_LUA_OPCODE(ImSetState);
	DECLARE_LUA_OPCODE(EnableVoiceFX);
	DECLARE_LUA_OPCODE(SetGroupVolume);
	DECLARE_LUA_OPCODE(EnableAudioGroup);
	DECLARE_LUA_OPCODE(ImSelectSet);
	DECLARE_LUA_OPCODE(GetActorChores);
	DECLARE_LUA_OPCODE(PlayActorChore);
	DECLARE_LUA_OPCODE(StopActorChores);
	DECLARE_LUA_OPCODE(SetActorLighting);
	DECLARE_LUA_OPCODE(SetActorCollisionMode);
	DECLARE_LUA_OPCODE(SetActorCollisionScale);
	DECLARE_LUA_OPCODE(GetActorPuckVector);
	DECLARE_LUA_OPCODE(SetActorHeadLimits);
	DECLARE_LUA_OPCODE(SetActorHead);
	DECLARE_LUA_OPCODE(SetActorFOV);
	DECLARE_LUA_OPCODE(AttachActor);
	DECLARE_LUA_OPCODE(DetachActor);
	DECLARE_LUA_OPCODE(GetCPUSpeed);
	DECLARE_LUA_OPCODE(StartMovie);
	DECLARE_LUA_OPCODE(IsMoviePlaying);
	DECLARE_LUA_OPCODE(SetActiveCD);
	DECLARE_LUA_OPCODE(GetActiveCD);
	DECLARE_LUA_OPCODE(PurgeText);
	DECLARE_LUA_OPCODE(ImFlushStack);
	DECLARE_LUA_OPCODE(LoadSound);
	DECLARE_LUA_OPCODE(ImSetMusicVol);
	DECLARE_LUA_OPCODE(ImSetSfxVol);
	DECLARE_LUA_OPCODE(ImSetVoiceVol);
	DECLARE_LUA_OPCODE(ImSetVoiceEffect);
	DECLARE_LUA_OPCODE(ToggleOverworld);
	DECLARE_LUA_OPCODE(ScreenshotForSavegame);
	DECLARE_LUA_OPCODE(EngineDisplay);
	DECLARE_LUA_OPCODE(SetAmbientLight);
	DECLARE_LUA_OPCODE(Display);
	DECLARE_LUA_OPCODE(ThumbnailFromFile);
	DECLARE_LUA_OPCODE(ClearSpecialtyTexture);
	DECLARE_LUA_OPCODE(ClearOverworld);
	DECLARE_LUA_OPCODE(EnableActorPuck);
	DECLARE_LUA_OPCODE(GetActorSortOrder);
	DECLARE_LUA_OPCODE(IsChoreLooping);
	DECLARE_LUA_OPCODE(SetChoreLooping);
	DECLARE_LUA_OPCODE(PlayChore);
	DECLARE_LUA_OPCODE(PauseChore);
	DECLARE_LUA_OPCODE(CompleteChore);
	DECLARE_LUA_OPCODE(UnlockChore);
	DECLARE_LUA_OPCODE(LockChoreSet);
	DECLARE_LUA_OPCODE(UnlockChoreSet);
	DECLARE_LUA_OPCODE(EscapeMovie);
	DECLARE_LUA_OPCODE(StopAllSounds);
	DECLARE_LUA_OPCODE(FreeSound);
	DECLARE_LUA_OPCODE(PlayLoadedSound);
	DECLARE_LUA_OPCODE(StopSound);
	DECLARE_LUA_OPCODE(PlaySound);
	DECLARE_LUA_OPCODE(IsSoundPlaying);
	DECLARE_LUA_OPCODE(GetSoundVolume);
	DECLARE_LUA_OPCODE(SetSoundVolume);
	DECLARE_LUA_OPCODE(PlaySoundFrom);
	DECLARE_LUA_OPCODE(PlayLoadedSoundFrom);
	DECLARE_LUA_OPCODE(UpdateSoundPosition);
	DECLARE_LUA_OPCODE(ImStateHasLooped);
	DECLARE_LUA_OPCODE(ImStateHasEnded);
	DECLARE_LUA_OPCODE(ImPushState);
	DECLARE_LUA_OPCODE(ImPopState);
	DECLARE_LUA_OPCODE(GetSectorName);
	DECLARE_LUA_OPCODE(GetCameraPosition);
	DECLARE_LUA_OPCODE(GetCameraYaw);
	DECLARE_LUA_OPCODE(YawCamera);
	DECLARE_LUA_OPCODE(GetCameraPitch);
	DECLARE_LUA_OPCODE(GetCameraRoll);
	DECLARE_LUA_OPCODE(PitchCamera);
	DECLARE_LUA_OPCODE(RollCamera);
	DECLARE_LUA_OPCODE(NewLayer);
	DECLARE_LUA_OPCODE(FreeLayer);
	DECLARE_LUA_OPCODE(SetLayerSortOrder);
	DECLARE_LUA_OPCODE(SetLayerFrame);
	DECLARE_LUA_OPCODE(AdvanceLayerFrame);
	DECLARE_LUA_OPCODE(PushText);
	DECLARE_LUA_OPCODE(PopText);
	DECLARE_LUA_OPCODE(NukeAllScriptLocks);
	DECLARE_LUA_OPCODE(ToggleDebugDraw);
	DECLARE_LUA_OPCODE(ToggleDrawCameras);
	DECLARE_LUA_OPCODE(ToggleDrawLights);
	DECLARE_LUA_OPCODE(ToggleDrawSectors);
	DECLARE_LUA_OPCODE(ToggleDrawBBoxes);
	DECLARE_LUA_OPCODE(ToggleDrawFPS);
	DECLARE_LUA_OPCODE(ToggleDrawPerformance);
	DECLARE_LUA_OPCODE(ToggleDrawActorStats);
	DECLARE_LUA_OPCODE(SectEditSelect);
	DECLARE_LUA_OPCODE(SectEditPlace);
	DECLARE_LUA_OPCODE(SectEditDelete);
	DECLARE_LUA_OPCODE(SectEditInsert);
	DECLARE_LUA_OPCODE(SectEditSortAdd);
	DECLARE_LUA_OPCODE(SectEditForgetIt);
	DECLARE_LUA_OPCODE(FRUTEY_Begin);
	DECLARE_LUA_OPCODE(FRUTEY_End);
	DECLARE_LUA_OPCODE(GetFontDimensions);
	DECLARE_LUA_OPCODE(GetTextObjectDimensions);
	DECLARE_LUA_OPCODE(GetTextCharPosition);
	DECLARE_LUA_OPCODE(SetActorRestChore);
	DECLARE_LUA_OPCODE(SetActorWalkChore);
	DECLARE_LUA_OPCODE(SetActorTurnChores);
	DECLARE_LUA_OPCODE(SetActorTalkChore);
	DECLARE_LUA_OPCODE(SetActorMumblechore);
	DECLARE_LUA_OPCODE(GammaEnabled);
	DECLARE_LUA_OPCODE(FileFindFirst);
	DECLARE_LUA_OPCODE(WalkActorToAvoiding);
	DECLARE_LUA_OPCODE(LocalizeString);
	DECLARE_LUA_OPCODE(WorldToScreen);
	//PS2:
	DECLARE_LUA_OPCODE(GetMemoryCardId);
	DECLARE_LUA_OPCODE(OverWorldToScreen);
	//ResidualVM-hacks:
	DECLARE_LUA_OPCODE(GetResidualVMPreference);
	DECLARE_LUA_OPCODE(SetResidualVMPreference);
};

} // end of namespace Grim

#endif
