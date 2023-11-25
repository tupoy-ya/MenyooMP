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

#include <map>
#include <string>

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

namespace sub
{
	void MiscOps();

	void Timecycles_();

	void ClearAreaSub();

	void DrawToolSub_();

	void RadioSub_();

	namespace WaterHack_catind
	{
		void Tick();

		void Sub_WaterHack();
	}

	namespace TVChannelStuff_catind
	{
		extern bool loop_basictv;

		extern std::string currentTvChannelLabel;
		extern std::map<std::string, std::string> vTvPlaylists;

		void DrawTvWhereItsSupposedToBe();

		void Sub_TV();
	}

	namespace HudOptions_catind
	{
		extern bool loop_revealMinimap;

		void Sub_HudOptions();
	}

	namespace GameCamOptions_catind
	{
		extern float _shakeAmplitude;
		extern int8_t _shakeId;

		void Sub_GameCamOptions();
	}

}
