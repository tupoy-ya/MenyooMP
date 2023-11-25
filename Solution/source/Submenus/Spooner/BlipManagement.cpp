/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "BlipManagement.h"

#include "Databases.h"
#include "Scripting/GTAblip.h"
#include "Scripting/World.h"
#include "Util/GTAmath.h"

#include <string>
#include <utility>
#include <vector>

namespace sub::Spooner
{
	namespace BlipManagement
	{
		void ClearDb()
		{
			Databases::RefBlipDb.clear();
		}
		void ClearAllRefCoordBlips()
		{
			for (auto& b : Databases::RefBlipDb)
			{
				b.first.Remove();
			}
			Databases::RefBlipDb.clear();
		}

		void AddBlipToRefCoordDb(const std::pair<GTAblip, std::string>& pB)
		{
			for (auto& b : Databases::RefBlipDb)
			{
				if (b.second.compare(pB.second) == 0)
				{
					b = pB;
					return;
				}
			}
			Databases::RefBlipDb.push_back(pB);
		}
		std::pair<GTAblip, std::string> CreateRefCoordBlip(const Vector3& position, const std::string& name, bool addToDb)
		{
			auto blip = World::CreateBlip(position);
			blip.SetFlashing(false);
			blip.SetScale(0.8f);
			blip.SetFriendly(true);
			blip.SetIcon(BlipIcon::Castle);
			blip.SetColour(BlipColour::Green);
			blip.SetBlipName(name);
			const auto& blipAndNamePair = std::make_pair(blip, name);
			if (addToDb)
				AddBlipToRefCoordDb(blipAndNamePair);
			return blipAndNamePair;
		}
	}

}
