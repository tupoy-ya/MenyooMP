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

typedef unsigned __int8 uint8_t;
typedef int INT, Vehicle, Entity, Ped;
typedef unsigned long DWORD, Hash;
typedef char *PCHAR;

class GTAentity;
class GTAvehicle;
class GTAped;
namespace GTAmodel {
	class Model;
}

namespace sub
{
	// Vehicle - spawn function
	int FuncSpawnVehicle_(GTAmodel::Model model, GTAped ped, bool deleteOld = false, bool warpIntoVehicle = true);

	// Vehicle spawner

	namespace SpawnVehicle_catind
	{
		extern uint8_t SpawnVehicle_index;

		enum Indices
		{
			SUPER, SPORT, SPORTSCLASSIC, COUPE, MUSCLE, OFFROAD, SUV, SEDAN, COMPACT,
			PICKUP, VAN, TRUCK, SERVICE, TRAILER, TRAIN,
			EMERGENCY, MOTORCYCLE, BICYCLE, PLANE, HELICOPTER, BOAT, OTHER
		};

		void PopulateVehicleBmps();

		void AddvcatOption_(const std::string& text, uint8_t index, bool *extra_option_code = nullptr);

	}

	void SpawnVehicle_();

	void SpawnVehicle_Options();

	bool SpawnVehicle_IsVehicleModelAFavourite(GTAmodel::Model vehModel);
	bool SpawnVehicle_AddVehicleModelToFavourites(GTAmodel::Model vehModel, const std::string& customName);
	bool SpawnVehicle_RemoveVehicleModelFromFavourites(GTAmodel::Model vehModel);

	void SpawnVehicle_AllCatsSub();
	void SpawnVehicle_Favourites();

	// Vehicle saver

	namespace VehicleSaver_catind
	{
		extern uint8_t _persistentAttachmentsTexterIndex;
		extern uint8_t _driverVisibilityTexterIndex;

		void VehSaver_SaveToFile(std::string filePath, GTAvehicle ev);
		void VehSaver_ReadFromFile(std::string filePath, GTAentity ped);

		void Sub_VehSaver();
		void Sub_VehSaver_InItem();

	}

}



