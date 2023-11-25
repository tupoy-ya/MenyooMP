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

typedef signed char int8_t;
typedef int INT, Ped, Vehicle, Entity, ScrHandle;
typedef unsigned long DWORD, Hash;
typedef float FLOAT;

class GTAped;
class GTAvehicle;
class GTAentity;

class GTAplayer
{
private:
	int8_t index;

public:
	GTAplayer();
	GTAplayer(const GTAplayer& obj);
	GTAplayer(int8_t handle);

	struct NetHandle13
	{
		int _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12;
	};

	void SetIndex(int8_t); /// set new pIndex for player

	int8_t& Handle();                     /// return index reference
	int8_t GetHandle() const;             /// return index
	GTAped GetPed() const;                /// return player's ped handle
	std::string GetName() const;          /// return player's name
	bool IsActive() const;                /// return if exists
	bool IsAlive() const;                 /// return true if alive
	NetHandle13 GetNetHandle() const;     /// return NETWORK_HANDLE
	ScrHandle GetClanDescription() const; /// return clan desc/handle
	bool IsTalking() const;               /// return whether talking
	bool IsPressingHorn() const;          /// return whether pressing horn
	bool IsFreeAiming() const;            /// return whether free aiming
	bool IsFreeAimingAt(Entity) const;    /// return whether free aiming at entity
	bool IsTargetingAnything() const;     /// return whether targetting anything
	bool IsTargetingAt(Entity) const;     /// return whether targetting entity
	GTAentity AimedEntity() const;        /// returns entity handle for the entity the player is aiming at
	INT MaxArmour_get() const;            /// return player max armour
	INT WantedLevel_get() const;          /// return player wanted level

	void GiveRagdollControl(bool value);

	bool IsControlOn() const;               /// return whether self control is on
	void SetControl(bool toggle, int flag); /// set control flags


	bool operator==(GTAplayer const right);
};
