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
#include <vector>

class GTAentity;

namespace sub
{
	// All animations subs

	namespace AnimationSub_catind
	{
		struct NamedAnimation
		{
			std::string caption;
			std::string animDict, animName;
		};
		extern const std::vector<AnimationSub_catind::NamedAnimation> vPresetPedAnims;

		extern std::map<std::string, std::vector<std::string>> vAllPedAnims;
		extern std::pair<const std::string, std::vector<std::string>>* selectedAnimDictPtr;

		void PopulateAllPedAnimsList();

		void Sub_AllPedAnims();
		void Sub_AllPedAnims_InDict();
	}

	// Animation Favourites

	void GetFavouriteAnimations(std::vector<std::pair<std::string, std::string>>& result);
	bool IsAnimationAFavourite(const std::string animDict, const std::string& animName);
	void AddAnimationToFavourites(const std::string animDict, const std::string& animName);
	void RemoveAnimationFromFavourites(const std::string animDict, const std::string& animName);

	// Old animation subs

	void AnimationSub_StopAnimationCallback();

	void AnimationSub_();
	void AnimationSub_Settings();
	void AnimationSub_Favourites();
	void AnimationSub_Custom();
	void AnimationSub_Deer();
	void AnimationSub_Shark();
	void AnimationSub_MissRappel();
	void AnimationSub_GestSit();
	void AnimationSub_Swat();
	void AnimationSub_GuardReac();
	void AnimationSub_RandArrest();

	// Scenario animations

	namespace AnimationSub_TaskScenarios
	{
		extern std::vector<std::string> vValues_TaskScenarios;
		struct NamedScenario
		{
			std::string name;
			std::string label;
		};
		extern std::vector<NamedScenario> vNamedScenarios;


		void AnimationSub_TaskScenarios();
		void AnimationSub_TaskScenarios2();

	}

	// Movement clipsets

	std::string get_ped_movement_clipset(const GTAentity& ped);
	void set_ped_movement_clipset(GTAentity ped, const std::string& setName);
	std::string get_ped_weapon_movement_clipset(const GTAentity& ped);
	void set_ped_weapon_movement_clipset(GTAentity ped, const std::string& setName);

	void MovementGroup_();

	// Facial animations

	namespace FacialAnims_catind
	{
		struct NamedFacialAnim
		{
			std::string caption;
			std::string animName;
		};
		extern const std::vector<NamedFacialAnim> vFacialAnims;

		void Sub_FacialMood();
	}

}
