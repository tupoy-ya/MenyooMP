/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "JumpAroundMode.h"

#include "Menu/Routine.h"
#include "Natives/natives2.h"
#include "Natives/types.h" // RgbS
#include "Scripting/GTAped.h"
#include "Scripting/Raycast.h"
#include "Scripting/World.h"
#include "Util/ExePath.h"
#include "Util/FileLogger.h"
#include "Util/GTAmath.h"

#include <string>
#include <vector>
#include <windows.h>

namespace _JumpAroundMode_
{
	bool bEnabled = false;

	struct DiscoLight
	{
		float angle;
		RgbS colour;
	};
	std::vector<DiscoLight> discoLights{{0, RgbS(0, 255, 255)}, {180, RgbS(255, 50, 0)}};
	inline void DrawDiscoLights()
	{
		GTAped myPed  = PLAYER_PED_ID();
		Vector3 myPos = myPed.Position_get();
		int i;

		DiscoLight* dlight;
		RaycastResult ray;
		for (i = 0; i < discoLights.size(); i++)
		{
			dlight = &discoLights[i];
			dlight->angle += 0.0001f;
			if (dlight->angle > 360.0f)
				dlight->angle = 0.0f;

			Vector3 lightPos = myPos.PointOnCircle(240.0f, dlight->angle);

			ray = RaycastResult::Raycast(myPos + Vector3(0, 0, 2.0f), lightPos, IntersectOptions::Everything);
			if (ray.DidHitAnything())
			{
				lightPos = ray.HitCoords();
				World::DrawLightWithRange(lightPos, dlight->colour, 6.2f, 3.0f);
			}
		}
	}

	void Tick()
	{
		float jumpForce = GET_RANDOM_FLOAT_IN_RANGE(0.6f, 1.14f);
		for (auto& ped : _nearbyPeds)
		{
			if (IS_ENTITY_ATTACHED(ped) || !IS_PED_ON_FOOT(ped) || IS_ENTITY_IN_AIR(ped))
				continue;
			NETWORK_REQUEST_CONTROL_OF_ENTITY(ped);
			TASK_JUMP(ped, true, false, false);
			APPLY_FORCE_TO_ENTITY(ped, 1, 0, 0, jumpForce, 0, 0, 0, false, false, true, true, false, true);
			STOP_PED_SPEAKING(ped, true);
		}

		auto& neonRGB = g_fadedRGB;
		jumpForce     = GET_RANDOM_FLOAT_IN_RANGE(40, 90);
		for (auto& vehicle : _nearbyVehicles)
		{
			if (vehicle == g_myVeh || IS_ENTITY_ATTACHED(vehicle))
				continue;

			Vector3 Pos = GET_ENTITY_COORDS(vehicle, 1);
			SET_VEHICLE_NEON_ENABLED(vehicle, 0, true);
			SET_VEHICLE_NEON_ENABLED(vehicle, 1, true);
			SET_VEHICLE_NEON_ENABLED(vehicle, 2, true);
			SET_VEHICLE_NEON_ENABLED(vehicle, 3, true);
			SET_VEHICLE_NEON_COLOUR(vehicle, neonRGB.R, neonRGB.G, neonRGB.B);
			DRAW_LIGHT_WITH_RANGE(Pos.x, Pos.y, Pos.z, neonRGB.R, neonRGB.G, neonRGB.B, 3.2f, 1.2f);

			if (IS_VEHICLE_ON_ALL_WHEELS(vehicle))
			{
				NETWORK_REQUEST_CONTROL_OF_ENTITY(vehicle);
				SET_VEHICLE_NEON_ENABLED(vehicle, 0, 1);
				SET_VEHICLE_NEON_ENABLED(vehicle, 1, 1);
				SET_VEHICLE_NEON_ENABLED(vehicle, 2, 1);
				SET_VEHICLE_NEON_ENABLED(vehicle, 3, 1);
				APPLY_FORCE_TO_ENTITY(vehicle, 0, 0.0f, 0.0f, jumpForce, 0.0f, 0.0f, 0.0f, false, false, true, true, false, true); // 68.f
			}
		}

		DrawDiscoLights();
	}

	const std::string szAlias = "JumpAroundMode";
	void BgMusic(bool enable)
	{
		std::string szCommand;
		try
		{
			if (enable)
			{
				szCommand = "open \"" + GetPathffA(Pathff::Main, true) + szAlias + ".mp3" + "\" type MPEGVideo" + " alias " + szAlias;
				mciSendStringA(szCommand.c_str(), NULL, 0, 0);

				szCommand = "play " + szAlias + " from 0 repeat";
				mciSendStringA(szCommand.c_str(), NULL, 0, 0);

				szCommand = "setaudio " + szAlias + " volume to 400";
				mciSendStringA(szCommand.c_str(), NULL, 0, 0);
			}
			else
			{
				szCommand = "stop " + szAlias;
				mciSendStringA(szCommand.c_str(), NULL, 0, 0);

				szCommand = "close " + szAlias;
				mciSendStringA(szCommand.c_str(), NULL, 0, 0);
			}
		}
		catch (std::exception& e)
		{
			ige::myLog << ige::LogType::LOG_ERROR << "JumpAroundMode countered an exception -> " << e.what();
		}
	}

	void StartJumping(bool enable)
	{
		BgMusic(enable);

		int i;
		for (i = 0; i < discoLights.size(); i++)
		{
			discoLights[i].colour = RgbS(rand() % 256, rand() % 256, rand() % 256);
		}
	}

}
