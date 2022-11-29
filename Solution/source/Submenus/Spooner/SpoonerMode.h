/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include "Scripting/GTAentity.h"
#include "Scripting/Model.h"

#include <utility>
#include <set>

typedef unsigned char uint8_t, BYTE;
typedef unsigned short uint16_t;

class Camera;

namespace sub::Spooner
{
	class SpoonerEntity;

	namespace SpoonerMode
	{
		extern BYTE bindsKeyboard;
		extern std::pair<uint16_t, uint16_t> bindsGamepad;

		extern bool bEnabled;
		extern bool bIsSomethingHeld;
		extern bool bHeldEntityHasCollision;
		extern Camera spoonerModeCamera;
		extern float spoonerModeCameraCamDistance;

		bool IsHotkeyPressed();

		struct ModelPreviewInfoStructure
		{
			EntityType entityType;
			Model previousModel,
				model;
			GTAentity entity;
			std::set<GTAentity> previousEntities;
		};
		extern ModelPreviewInfoStructure ModelPreviewInfo;
		void SpawnModelPreview();

		void ResetSelectedEntity();
		bool GetEntityPtr(GTAentity& inEntity, SpoonerEntity*& outEntity);
		SpoonerEntity GetEntityPtrValue(GTAentity& entity);
		inline void SetAsSelectedEntity(GTAentity& entity);

		inline void CamTick();
		void Tick();

		void TurnOn();
		void TurnOff();
		void Toggle();
	}

}



