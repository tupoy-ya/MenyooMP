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

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned long DWORD, Hash;
typedef int INT, Vehicle, Ped, Object, Entity, ScrHandle;

namespace sub
{
	void PopulateAllPaintIDs();

	// vehicle - upgrades
	void set_vehicle_max_upgrades(Vehicle vehicle, bool upgradeIt = true, bool invincible = false, int8_t plateType = 5, std::string plateText = std::string(), bool neonIt = false, uint8_t NeonR = 0, uint8_t NeonG = 0, uint8_t NeonB = 0, int16_t prim_col_index = -3, int16_t sec_col_index = -3);


	// ModShop

	void ModShop_();

	// Bennys

	namespace Bennys_catind
	{

		void Sub_BennysMain();
	}

	// Selected vehicle mod submenu (for selection of mod value)

	void MSCatall_();

	// Emblem

	void MSEmblem_();

	// Wheels

	void MSWheels_();
	void MSWheels2_();
	void MSWheels3_();
	void MSTyresBurst_();

	// Windows

	namespace MSWindows_catind
	{
		void MSWindows_();
	}

	// Doors

	void MSDoors_();

	// Paints

	INT getpaintCarUsing_index(Vehicle veh, INT partIndex_CustomK);
	void paintCarUsing_index(Vehicle veh, INT partIndex_CustomK, int16_t colour_index, int16_t pearl_index);

	void MSPaints_();
	void MSPaints2_();

	namespace MSPaints_catind
	{
		void Sub_Shared();
		void Sub_Chrome();
		void Sub_Normal();
		void Sub_Matte();
		void Sub_Metallic();
		void Sub_Metal();
		void Sub_Chameleon();
		void Sub_Pearl();
		void Sub_Util();
		void Sub_Worn();
	}

	void rgb_mode_set_carcol(Vehicle veh, int16_t R, int16_t G, int16_t B, int16_t A);
	void MSPaints_RGB();

	// Extras

	void MSExtra_();

	// Neons

	void MSNeons_();

	// Engine sound

	void MSEngineSound_();

	void MSLights_();

}
