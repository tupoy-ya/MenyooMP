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

#include "Natives/types.h" //RGBA/RgbS
#include "SpoonerSettings.h"

#include <string>

typedef unsigned __int8 uint8_t;
typedef unsigned int UINT;
enum class EntityType : uint8_t;
class Vector3;
class GTAentity;
namespace GTAmodel
{
	class Model;
}

namespace sub::Spooner
{
	class SpoonerEntity;

	namespace EntityManagement
	{
		UINT GetNumberOfEntitiesSpawned(const EntityType& type);

		int GetEntityIndexInDb(const GTAentity& entity);
		int GetEntityIndexInDb(const SpoonerEntity& ent);
		void AddEntityToDb(SpoonerEntity ent, bool missionEnt = Settings::bAddToDbAsMissionEntities);
		void RemoveEntityFromDb(const SpoonerEntity& ent);

		void ClearDb();
		void DeleteEntity(SpoonerEntity& ent);
		void DeleteAllEntitiesInDb();
		void DeleteAllEntitiesOfTypeInDb(const EntityType& targetType);
		void DeleteAllPropsInDb();
		void DeleteAllPedsInDb();
		void DeleteAllVehiclesInDb();
		void DeleteInvalidEntitiesInDb();

		void DeleteAllEntitiesInWorld();
		void DeleteAllPropsInWorld();
		void DeleteAllPedsInWorld();
		void DeleteAllVehiclesInWorld();

		SpoonerEntity AddProp(const GTAmodel::Model& model, const std::string& name, bool unloadModel = true);
		SpoonerEntity AddPed(const GTAmodel::Model& model, const std::string& name, bool unloadModel = true);
		SpoonerEntity AddVehicle(const GTAmodel::Model& model, const std::string& name, bool unloadModel = true);
		SpoonerEntity AddEntityOfType(const EntityType& type, const GTAmodel::Model& model, const std::string& name);

		SpoonerEntity CopyEntity(SpoonerEntity orig, bool isInDb, bool addToDb = true, uint8_t copyAttachments = 0, bool unloadModel = true, uint8_t currAtir = 0);

		void DetachEntity(SpoonerEntity& ent);
		bool GetEntityThisEntityIsAttachedTo(GTAentity& from, GTAentity& to);
		void AttachEntity(SpoonerEntity& ent, GTAentity to, int boneIndex, const Vector3& offset, const Vector3& rotation);
		void AttachEntityInit(SpoonerEntity& ent, GTAentity to, bool bAttachWithRelativePosRot = false);

		// Keyboard input
		SpoonerEntity InputEntityIntoDb(const EntityType& type);

		void ShowBoxAroundEntity(const GTAentity& ent, bool showPoly = true, RgbS colour = {0, 255, 255});
		void ShowArrowAboveEntity(const GTAentity& ent, RGBA colour = {0, 255, 255, 200});
		void DrawRadiusDisplayingMarker(const Vector3& pos, float radius, RGBA colour = {0, 255, 0, 130});
	}

}
