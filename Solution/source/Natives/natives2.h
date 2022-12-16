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

#include "natives.h"

#include "main.h" // scriptWait

#include <string>

using namespace PLAYER;
using namespace ENTITY;
using namespace PED;
using namespace VEHICLE;
using namespace OBJECT;
using namespace TASK;
using namespace MISC;
using namespace AUDIO;
using namespace CUTSCENE;
using namespace INTERIOR;
using namespace CAM;
using namespace WEAPON;
using namespace ITEMSET;
using namespace STREAMING;
using namespace SCRIPT;
using namespace HUD;
using namespace GRAPHICS;
using namespace STATS;
using namespace BRAIN;
using namespace MOBILE;
using namespace APP;
using namespace CLOCK;
using namespace PATHFIND;
using namespace PAD;
using namespace DATAFILE;
using namespace FIRE;
using namespace EVENT;
using namespace ZONE;
using namespace PHYSICS;
using namespace WATER;
using namespace SHAPETEST;
using namespace NETWORK;
using namespace MONEY;
using namespace FILES;
using namespace DLC;
using namespace SYSTEM;
using namespace DECORATOR;
using namespace SOCIALCLUB;
using namespace LOADINGSCREEN;
using namespace RECORDING;
using namespace REPLAY;
using namespace NETSHOPPING;

static inline void WAIT(DWORD time)
{
	scriptWait(time);
}
static inline void TERMINATE()
{
	WAIT(MAXDWORD);
}

Hash GET_HASH_KEY(char* value);
Hash GET_HASH_KEY(const std::wstring& value);
Hash GET_HASH_KEY(const std::string& value);

void add_text_component_long_string(const std::string& text);

bool IS_PED_SHOOTING(Ped ped);
bool IS_PLAYER_FREE_AIMING(Ped _);

Entity IS_PLAYER_FREE_AIMING_AT_ENTITY(Player _, Entity ent);
