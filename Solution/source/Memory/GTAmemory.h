/*
* Copyright (C) 2015 crosire
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/
/*
* ALTERED SOURCE
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*/
#pragma once

#include "main.h" //getGameVersion getGlobalPtr

//#include <cstddef>
#include <array>
#include <psapi.h>
#include <string>
#include <vector>
#include <windows.h>

typedef unsigned char uint8_t, BYTE;
typedef signed int INT32;
typedef unsigned long DWORD;
typedef unsigned long long size_t;
typedef unsigned __int64 DWORD64;
typedef void* LPVOID;
typedef const char* LPCSTR;
typedef int Entity;

////psapi.h
//typedef struct _MODULEINFO {
//	LPVOID lpBaseOfDll;
//	DWORD SizeOfImage;
//	LPVOID EntryPoint;
//} MODULEINFO, *LPMODULEINFO;

enum eGameVersion : int;
class Vector3;

extern HMODULE g_MainModule;
extern MODULEINFO g_MainModuleInfo;

static inline bool IsBitSet(int& i, int bitNum)
{
	return ((i & (1 << bitNum)) != 0);
}
static inline void SetBit(int& i, int bitNum)
{
	i |= (1 << bitNum);
}
static inline void ClearBit(int& i, int bitNum)
{
	i &= ~(1 << bitNum);
}
static void SetClearBit(int& i, int bitNum, bool setAction)
{
	if (setAction)
		SetBit(i, bitNum);
	else
		ClearBit(i, bitNum);
}

template<typename R>
R GetMultilayerPointer(void* base, std::vector<DWORD>& offsets);

namespace MemryScan
{
	struct PatternByte
	{
		PatternByte();

		PatternByte(std::string byteString, bool ignoreThis = false);

		bool ignore;
		uint8_t data;

	private:
		uint8_t StringToUint8(std::string str);
	};

	class Pattern final
	{
	public:
		static DWORD64 Scan(DWORD64 dwStart, DWORD64 dwLength, const std::string& s);
		static DWORD64 Scan(MODULEINFO mi, const std::string& s);
	};

	class PatternScanner final
	{
	public:
		static bool CompareMemory(const BYTE* data, const BYTE* pattern, const char* mask);

		static DWORD64 FindPattern(std::vector<unsigned short> pattern);
		static DWORD64 FindPattern(const char* bMaskc, const char* sMaskc);
		static DWORD64 FindPattern(const char* pattern, const char* mask, DWORD64 startAddress, size_t size);
		static DWORD64 FindPattern(const std::string& patternString);
		static DWORD64 FindPattern(MODULEINFO moduleInfo, const std::string& patternString);
	};
}

// citizenfx
class Blipx
{
public:
	__int32 iID; //0x0000
	__int8 iID2; //0x0004
	char _0x0005[3];
	BYTE N000010FB; //0x0008 (80 = moves with player, some values will turn icon into map cursor and break it)
	char _0x0009[7];
	float x;
	float y;
	float z;
	char _0x001C[6];
	BYTE bFocused; //0x0022   (Focused? 0100 0000)
	char _0x0023[5];
	char* szMessage; //0x0028 If not null, contains the string of whatever the blip says when selected.
	char _0x0030[16];
	int iIcon; //0x0040
	char _0x0044[4];
	DWORD dwColor; //0x0048 (Sometimes works?)
	char _0x004C[4];
	float fScale;      //0x0050
	__int16 iRotation; //0x0054 Heading
	BYTE bInfoIDType;  //0x0056 GET_BLIP_INFO_ID_TYPE
	BYTE bZIndex;      //0x0057
	BYTE bDisplay;     //0x0058  Also Visibility 0010
	BYTE bAlpha;       //0x0059
};                     //Size=0x005A
class BlipList
{
public:
	Blipx* m_Blips[1500]; //0x0000
	char _0x2EE0[56];

}; //Size=0x2F18

// ScriptHookVDotNet
class GTAmemory final
{
public:
	static int (*_addEntityToPoolFunc)(uint64_t address);
	static uint64_t (*_entityPositionFunc)(uint64_t address, float* position);
	static uint64_t (*_entityModel1Func)(uint64_t address), (*_entityModel2Func)(uint64_t address);
	static uint64_t *_entityPoolAddress, *_vehiclePoolAddress, *_pedPoolAddress, *_objectPoolAddress, *_pickupObjectPoolAddress;
	static uint64_t (*CheckpointBaseAddr)();
	static uint64_t (*CheckpointHandleAddr)(uint64_t baseAddr, int Handle);
	static uint64_t* checkpointPoolAddress;
	static float *_readWorldGravityAddress, *_writeWorldGravityAddress;

	static BlipList* _blipList;
	static BlipList* GetBlipList();

	// Unknown_Modder
	struct GXT2Entry
	{
		DWORD labelHash;
		DWORD strOffset;
	};
	static uint64_t _globalTextBlockAddr;
	static std::vector<GXT2Entry> _vecGXT2Entries;
	static void LoadGlobalGXTEntries();
	static void EditGXTLabel(DWORD labelHash, LPCSTR string);
	static uint64_t _gxtLabelFromHashAddr1;
	// Zorg
	static char*(__fastcall* _gxtLabelFromHashFuncAddr)(uint64_t address, unsigned int hash);
	static inline char* GetGXTEntry(unsigned int labelHash)
	{
		return _gxtLabelFromHashFuncAddr(_gxtLabelFromHashAddr1, labelHash);
	}

	static std::array<std::vector<unsigned int>, 0x20> const& VehicleModels()
	{
		return vehicleModels;
	}
	static bool IsModelAPed(unsigned int modelHash);

	static void Init();

	static inline eGameVersion GetGameVersion()
	{
		static eGameVersion __getGameVersion_value = getGameVersion();
		return __getGameVersion_value;
	}

	template<typename T>
	static inline T* GetGlobalPtr(int globalId)
	{
		return reinterpret_cast<T*>(getGlobalPtr(globalId));
	}

	static inline char ReadSByte(uint64_t address)
	{
		return *(char*)address;
	}
	static inline unsigned char ReadByte(uint64_t address)
	{
		return *(unsigned char*)address;
	}
	static inline short ReadShort(uint64_t address)
	{
		return *(short*)address;
	}
	static inline unsigned short ReadUShort(uint64_t address)
	{
		return *(unsigned short*)address;
	}
	static inline int ReadInt(uint64_t address)
	{
		return *(int*)address;
	}
	static inline unsigned int ReadUInt(uint64_t address)
	{
		return *(unsigned int*)address;
	}
	static inline float ReadFloat(uint64_t address)
	{
		return *(float*)address;
	}
	static inline std::string ReadString(uint64_t address)
	{
		return (char*)address;
	}
	static Vector3 ReadVector3(uint64_t address);
	/*static inline Matrix GTAmemory::ReadMatrix(uint64_t address)
	{
	return *(Matrix*)address;
	}*/
	static inline void WriteSByte(uint64_t address, char value)
	{
		*(char*)address = value;
	}
	static inline void WriteByte(uint64_t address, unsigned char value)
	{
		*(unsigned char*)address = value;
	}
	static inline void WriteShort(uint64_t address, short value)
	{
		*(short*)address = value;
	}
	static inline void WriteUShort(uint64_t address, unsigned short value)
	{
		*(unsigned short*)address = value;
	}
	static inline void WriteInt(uint64_t address, int value)
	{
		*(int*)address = value;
	}
	static inline void WriteUInt(uint64_t address, unsigned int value)
	{
		*(unsigned int*)address = value;
	}
	static inline void WriteFloat(uint64_t address, float value)
	{
		*(float*)address = value;
	}
	static void WriteVector3(uint64_t address, const Vector3& value);

	static uint64_t GetCheckpointAddress(int handle);

	static uint64_t GetEntityBoneMatrixAddress(int handle, int boneIndex);

	static void GetVehicleHandles(std::vector<Entity>& result);
	static void GetVehicleHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes);
	static void GetVehicleHandles(std::vector<Entity>& result, const Vector3& position, float radius);
	static void GetVehicleHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes);

	static void GetPedHandles(std::vector<Entity>& result);
	static void GetPedHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes);
	static void GetPedHandles(std::vector<Entity>& result, const Vector3& position, float radius);
	static void GetPedHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes);

	static void GetPropHandles(std::vector<Entity>& result);
	static void GetPropHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes);
	static void GetPropHandles(std::vector<Entity>& result, const Vector3& position, float radius);
	static void GetPropHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes);

	static void GetEntityHandles(std::vector<Entity>& result);
	static void GetEntityHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes);
	static void GetEntityHandles(std::vector<Entity>& result, const Vector3& position, float radius);
	static void GetEntityHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes);

	static void GetCheckpointHandles(std::vector<int>& result);

	static void GetPickupObjectHandles(std::vector<int>& result);
	static void GetPickupObjectHandles(std::vector<int>& result, const Vector3& position, float radius);

	static int GetNumberOfVehicles();

	static float WorldGravity_get();
	static void WorldGravity_set(float value);

	static inline bool bittest(int data, unsigned char index)
	{
		return (data & (1 << index)) != 0;
	}

	static void GenerateVehicleModelList();

private:
	static uint64_t modelHashTable, modelNum2, modelNum3, modelNum4;
	static int modelNum1;
	static unsigned short modelHashEntries;
	static std::array<std::vector<unsigned int>, 0x20> vehicleModels;

	static const uint16_t poolCount_vehicles;
	static const uint16_t poolCount_peds;
	static const uint16_t poolCount_objects;
};

class GeneralGlobalHax final
{
public:
	static void DisableAnnoyingRecordingUI(bool uSure);
	static void EnableBlockedMpVehiclesInSp(bool uSure);

	static void** WorldPtrPtr();
	static float GetPlayerHeight();
	static void SetPlayerHeight(float value);
	static float GetPlayerSwimSpeed();
	static void SetPlayerSwimSpeed(float value);
	static float GetPlayerMovementSpeed();
	static void SetPlayerMovementSpeed(float value);

	static int GetVehicleBoostState();
	static void SetVehicleBoostState(int value);
	static float* GetVehicleBoostChargePtr();
};
