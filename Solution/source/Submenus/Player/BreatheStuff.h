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

#include <string>
#include <utility>
#include <vector>

typedef unsigned __int8 uint8_t;

namespace PTFX
{
	class LoopedPTFX;
}

namespace sub
{
	namespace BreatheStuff_catind
	{
		enum class BreathePtfxType : uint8_t
		{
			None,
			Blood,
			Fire,
		};
		extern const std::vector<std::pair<std::string, BreathePtfxType>> vCaptions_breatheStuffs;

		extern PTFX::LoopedPTFX _globalBreatheStuff_ptfx;
		extern BreathePtfxType loop_player_breatheStuff;

		void set_self_breathe_ptfx(const BreathePtfxType& type);


		void __AddOption(const std::string& text, const BreathePtfxType& type);

		void BreatheStuffSub_();

	}

}
