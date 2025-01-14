﻿/*
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
#include "GTAmemory.h"

#include "Natives/natives2.h"
#include "Util/FileLogger.h"
#include "Util/GTAmath.h"
#include "main.h"

#include <array>
#include <psapi.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <windows.h>


HMODULE g_MainModule        = 0;
MODULEINFO g_MainModuleInfo = {0};

template<typename R>
R GetMultilayerPointer(void* base, const std::vector<DWORD>& offsets)
{
	DWORD64 addr = (uint64_t)base;
	if (!addr)
		return NULL;
	auto numOffsets = offsets.size();

	for (auto i = 0; i < numOffsets - 1; i++)
	{
		addr = *(uint64_t*)(addr + offsets[i]);
		if (!addr)
			return NULL;
	}
	addr += offsets[numOffsets - 1];
	return (R)addr;
}

namespace MemryScan
{
	PatternByte::PatternByte() :
	    ignore(true)
	{
	}

	PatternByte::PatternByte(std::string byteString, bool ignoreThis)
	{
		data   = StringToUint8(byteString);
		ignore = ignoreThis;
	}

	uint8_t PatternByte::StringToUint8(std::string str)
	{
		std::istringstream iss(str);

		uint32_t ret;

		if (iss >> std::hex >> ret)
		{
			return (uint8_t)ret;
		}

		return 0;
	}

	DWORD64 Pattern::Scan(DWORD64 dwStart, DWORD64 dwLength, const std::string& s)
	{
		std::vector<PatternByte> p;
		std::istringstream iss(s);
		std::string w;

		while (iss >> w)
		{
			if (w.data()[0] == '?') // Wildcard
			{
				p.push_back(PatternByte());
			}
			else if (w.length() == 2 && isxdigit(w.data()[0]) && isxdigit(w.data()[1])) // Hex
			{
				p.push_back(PatternByte(w));
			}
			else
			{
				return NULL; // Bad
			}
		}

		for (DWORD64 i = 0; i < dwLength; i++)
		{
			uint8_t* lpCurrentByte = (uint8_t*)(dwStart + i);

			bool found = true;

			for (size_t ps = 0; ps < p.size(); ps++)
			{
				if (p[ps].ignore == false && lpCurrentByte[ps] != p[ps].data)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return (DWORD64)lpCurrentByte;
			}
		}

		return NULL;
	}
	DWORD64 Pattern::Scan(MODULEINFO mi, const std::string& s)
	{
		return Scan((DWORD64)mi.lpBaseOfDll, (DWORD64)mi.SizeOfImage, s);
	}

	bool PatternScanner::CompareMemory(const BYTE* data, const BYTE* pattern, const char* mask)
	{
		for (; *mask; ++mask, ++data, ++pattern)
		{
			if (*mask == 'x' && *data != *pattern)
			{
				return false;
			}
		}
		return (*mask) == NULL;
	}

	DWORD64 PatternScanner::FindPattern(std::vector<unsigned short> pattern)
	{
		DWORD64 i;
		DWORD64 size;
		DWORD64 address = 0;

		MODULEINFO info = g_MainModuleInfo;
		size            = (DWORD64)info.SizeOfImage;

		std::vector<unsigned char> search;
		std::vector<char> mask;

		for (auto elem : pattern)
		{
			if (elem != 0xFFFF)
			{
				search.push_back((unsigned char)elem);
				mask.push_back('x');
			}
			else
			{
				search.push_back(0);
				mask.push_back('?');
			}
		}

		for (i = 0; i < size; ++i)
		{
			if (CompareMemory((BYTE*)(address + i), (BYTE*)search.data(), mask.data()))
			{
				return (DWORD64)(address + i);
			}
		}
		return 0;
	}
	DWORD64 PatternScanner::FindPattern(const char* bMaskc, const char* sMaskc)
	{
		// Game Base & Size
		static DWORD64 pGameBase  = (DWORD64)g_MainModule;
		static uint32_t pGameSize = 0;
		if (!pGameSize)
		{
			MODULEINFO info = g_MainModuleInfo;
			pGameSize       = info.SizeOfImage;
		}

		// Scan
		for (uint32_t i = 0; i < pGameSize; i++)
		{
			if (CompareMemory((uint8_t*)(pGameBase + i), (uint8_t*)bMaskc, sMaskc))
			{
				return pGameBase + i;
			}
		}
		return 0;
	}
	DWORD64 PatternScanner::FindPattern(const char* pattern, const char* mask, DWORD64 startAddress, size_t size)
	{
		const char* currAddress    = (const char*)startAddress;
		const char* endAddress     = (const char*)(startAddress + size);
		const uint32_t mask_length = static_cast<uint32_t>(strlen(mask) - 1);

		for (uint32_t i = 0; currAddress < endAddress; currAddress++)
		{
			if (*currAddress == pattern[i] || mask[i] == '?')
			{
				if (mask[i + 1] == '\0')
				{
					return reinterpret_cast<DWORD64>(currAddress) - mask_length;
				}
				i++;
			}
			else
			{
				i = 0;
			}
		}
		return 0;
	}
	DWORD64 PatternScanner::FindPattern(const std::string& patternString)
	{
		return MemryScan::Pattern::Scan((DWORD64)g_MainModuleInfo.lpBaseOfDll, (DWORD64)g_MainModuleInfo.SizeOfImage, patternString);
	}
	DWORD64 PatternScanner::FindPattern(MODULEINFO moduleInfo, const std::string& patternString)
	{
		return MemryScan::Pattern::Scan((DWORD64)moduleInfo.lpBaseOfDll, (DWORD64)moduleInfo.SizeOfImage, patternString);
	}
}

//--------------------------------GTAmemory------------------------------------------------------

const uint16_t GTAmemory::poolCount_vehicles = 1024;
const uint16_t GTAmemory::poolCount_peds     = 1024;
const uint16_t GTAmemory::poolCount_objects  = 1024;

BlipList* GTAmemory::_blipList;
BlipList* GTAmemory::GetBlipList()
{
	return GTAmemory::_blipList;
}

uint64_t GTAmemory::modelHashTable, GTAmemory::modelNum2, GTAmemory::modelNum3, GTAmemory::modelNum4;
int GTAmemory::modelNum1;
unsigned short GTAmemory::modelHashEntries;
std::array<std::vector<unsigned int>, 0x20> GTAmemory::vehicleModels;

int (*GTAmemory::_addEntityToPoolFunc)(uint64_t address);
uint64_t (*GTAmemory::_entityPositionFunc)(uint64_t address, float* position);
uint64_t (*GTAmemory::_entityModel1Func)(uint64_t address), (*GTAmemory::_entityModel2Func)(uint64_t address);
uint64_t *GTAmemory::_entityPoolAddress, *GTAmemory::_vehiclePoolAddress, *GTAmemory::_pedPoolAddress, *GTAmemory::_objectPoolAddress, *GTAmemory::_pickupObjectPoolAddress;
uint64_t (*GTAmemory::CheckpointBaseAddr)();
uint64_t (*GTAmemory::CheckpointHandleAddr)(uint64_t baseAddr, int Handle);
uint64_t* GTAmemory::checkpointPoolAddress;
float *GTAmemory::_readWorldGravityAddress, *GTAmemory::_writeWorldGravityAddress;

uint64_t GTAmemory::_gxtLabelFromHashAddr1;
char*(__fastcall* GTAmemory::_gxtLabelFromHashFuncAddr)(uint64_t address, unsigned int hash);

class EntityPool
{
public:
	char _padding0[16];
	uint32_t num1;
	char _padding1[12];
	uint32_t num2;

	inline bool Full()
	{
		return num1 - (num2 & 0x3FFFFFFF) <= 256;
	}
};
class VehiclePool
{
public:
	uint64_t* poolAddress;
	uint32_t size;
	char _padding2[36];
	uint32_t* bitArray;
	char _padding3[40];
	uint32_t itemCount;

	inline bool isValid(uint32_t i)
	{
		return (bitArray[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline uint64_t getAddress(uint32_t i)
	{
		return poolAddress[i];
	}
};
class GenericPool
{
public:
	uint64_t poolStartAddress;
	uint8_t* byteArray;
	uint32_t size;
	uint32_t itemSize;


	inline bool isValid(uint32_t i)
	{
		return mask(i) != 0;
	}

	inline uint64_t getAddress(uint32_t i)
	{
		return mask(i) & (poolStartAddress + i * itemSize);
	}

private:
	inline long long mask(uint32_t i)
	{
		long long num1 = byteArray[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};
struct EntityPoolTask
{
	enum Type : uint16_t
	{
		Vehicle      = 1,
		Ped          = 2,
		Prop         = 4,
		PickupObject = 8
	};
	inline bool TypeHasFlag(uint16_t flag)
	{
		return (_type & flag) != 0;
	}

	EntityPoolTask(uint16_t type) :
	    _type(type),
	    _posCheck(false),
	    _modelCheck(false)
	{
	}

	inline bool CheckEntity(uintptr_t address)
	{
		if (_posCheck)
		{
			float position[3];
			GTAmemory::_entityPositionFunc(address, position);

			if (Vector3::Subtract(_position, Vector3(position[0], position[1], position[2])).LengthSquared() > _radiusSquared)
			{
				return false;
			}
		}

		if (_modelCheck)
		{
			uint32_t v0 = *reinterpret_cast<uint32_t*>(GTAmemory::_entityModel1Func(*reinterpret_cast<uint64_t*>(address + 32)));
			uint32_t v1        = v0 & 0xFFFF;
			uint32_t v2        = ((v1 ^ v0) & 0x0FFF0000 ^ v1) & 0xDFFFFFFF;
			uint32_t v3        = ((v2 ^ v0) & 0x10000000 ^ v2) & 0x3FFFFFFF;
			const uintptr_t v5 = GTAmemory::_entityModel2Func(reinterpret_cast<uintptr_t>(&v3));


			if (!v5)
			{
				return false;
			}
			for (int hash : _modelHashes)
			{
				if (*reinterpret_cast<int*>(v5 + 24) == hash)
				{
					return true;
				}
			}
			return false;
		}

		return true;
	}

	void Run(std::vector<int>& _handles)
	{
		if (*GTAmemory::_entityPoolAddress == 0)
		{
			return;
		}
		EntityPool* entityPool = reinterpret_cast<EntityPool*>(*GTAmemory::_entityPoolAddress);

		if (TypeHasFlag(Type::Vehicle))
		{
			if (*GTAmemory::_vehiclePoolAddress)
			{
				VehiclePool* vehiclePool = *reinterpret_cast<VehiclePool**>(*GTAmemory::_vehiclePoolAddress);

				for (uint32_t i = 0; i < vehiclePool->size; i++)
				{
					if (vehiclePool->isValid(i))
					{
						uint64_t address = vehiclePool->getAddress(i);
						if (address && CheckEntity(address))
						{
							_handles.push_back(GTAmemory::_addEntityToPoolFunc(address));
						}
					}
				}
			}
		}
		if (TypeHasFlag(Type::Ped))
		{
			if (*GTAmemory::_pedPoolAddress)
			{
				GenericPool* pedPool = reinterpret_cast<GenericPool*>(*GTAmemory::_pedPoolAddress);

				for (uint32_t i = 0; i < pedPool->size; i++)
				{
					if (pedPool->isValid(i))
					{
						uint64_t address = pedPool->getAddress(i);
						if (address && CheckEntity(address))
						{
							_handles.push_back(GTAmemory::_addEntityToPoolFunc(address));
						}
					}
				}
			}
		}
		if (TypeHasFlag(Type::Prop))
		{
			if (*GTAmemory::_objectPoolAddress)
			{
				GenericPool* propPool = reinterpret_cast<GenericPool*>(*GTAmemory::_objectPoolAddress);

				for (uint32_t i = 0; i < propPool->size; i++)
				{
					if (propPool->isValid(i))
					{
						uint64_t address = propPool->getAddress(i);
						if (address && CheckEntity(address))
						{
							_handles.push_back(GTAmemory::_addEntityToPoolFunc(address));
						}
					}
				}
			}
		}
		if (TypeHasFlag(Type::PickupObject))
		{
			if (*GTAmemory::_pickupObjectPoolAddress)
			{
				GenericPool* pickupPool = reinterpret_cast<GenericPool*>(*GTAmemory::_pickupObjectPoolAddress);

				for (uint32_t i = 0; i < pickupPool->size; i++)
				{
					if (pickupPool->isValid(i))
					{
						uint64_t address = pickupPool->getAddress(i);
						if (address)
						{
							if (_posCheck)
							{
								float* position = (float*)(address + 0x90);
								if (Vector3::Subtract(_position, Vector3(position[0], position[1], position[2])).LengthSquared() > _radiusSquared)
								{
									continue;
								}
							}
							_handles.push_back(GTAmemory::_addEntityToPoolFunc(address));
						}
					}
				}
			}
		}
	}

	uint16_t _type;
	bool _posCheck, _modelCheck;
	Vector3 _position;
	float _radiusSquared;
	std::vector<DWORD> _modelHashes;
};
struct GenericTask
{
public:
	typedef uint64_t (*func)(uint64_t);
	GenericTask(func pFunc, uint64_t Arg) :
	    _toRun(pFunc),
	    _arg(Arg)
	{
	}
	virtual void Run()
	{
		_res = _toRun(_arg);
	}

	uint64_t GetResult()
	{
		return _res;
	}

private:
	func _toRun;
	uint64_t _arg;
	uint64_t _res;
};

void GTAmemory::Init()
{
	uint64_t address;

	// Get relative address and add it to the instruction address.
	// 3 bytes equal the size of the opcode and its first argument. 7 bytes are the length of opcode and all its parameters.

	address = MemryScan::PatternScanner::FindPattern("4C 8D 05 ? ? ? ? 0F B7 C1");
	if (address)
		_blipList = reinterpret_cast<BlipList*>(*reinterpret_cast<int*>(address + 3) + address + 7);

	address = MemryScan::PatternScanner::FindPattern("48 89 5C 24 08 48 89 6C 24 18 89 54 24 10 56 57 41 56 48 83 EC 20");
	_gxtLabelFromHashFuncAddr = reinterpret_cast<char*(__fastcall*)(uint64_t, unsigned int)>(address);
	address                   = MemryScan::PatternScanner::FindPattern("84 C0 74 34 48 8D 0D ? ? ? ? 48 8B D3");
	_gxtLabelFromHashAddr1    = *reinterpret_cast<int*>(address + 7) + address + 11;

	address             = MemryScan::PatternScanner::FindPattern("48 8B DA E8 ? ? ? ? F3 0F 10 44 24");
	_entityPositionFunc = reinterpret_cast<uint64_t (*)(uint64_t, float*)>(address - 6);
	address             = MemryScan::PatternScanner::FindPattern("0F 85 ? ? ? ? 48 8B 4B 20 E8 ? ? ? ? 48 8B C8");
	_entityModel1Func = reinterpret_cast<uint64_t (*)(uint64_t)>(*reinterpret_cast<int*>(address + 11) + address + 15);
	address           = MemryScan::PatternScanner::FindPattern("45 33 C9 3B 05");
	_entityModel2Func = reinterpret_cast<uint64_t (*)(uint64_t)>(address - 0x46);

	address                  = MemryScan::PatternScanner::FindPattern("4C 8B 05 ? ? ? ? 40 8A F2 8B E9");
	_pickupObjectPoolAddress = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 3) + address + 7);

	address = MemryScan::PatternScanner::FindPattern("48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8");
	_addEntityToPoolFunc = reinterpret_cast<int (*)(uint64_t)>(address - 0x68);

	address             = MemryScan::PatternScanner::FindPattern("4C 8B 0D ? ? ? ? 44 8B C1 49 8B 41 08");
	_entityPoolAddress  = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 3) + address + 7);
	address             = MemryScan::PatternScanner::FindPattern("48 8B 05 ? ? ? ? F3 0F 59 F6 48 8B 08");
	_vehiclePoolAddress = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 3) + address + 7);
	address             = MemryScan::PatternScanner::FindPattern("48 8B 05 ? ? ? ? 41 0F BF C8 0F BF 40 10");
	_pedPoolAddress     = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 3) + address + 7);
	address             = MemryScan::PatternScanner::FindPattern("48 8B 05 ? ? ? ? 8B 78 10 85 FF");
	_objectPoolAddress  = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 3) + address + 7);

	address            = MemryScan::PatternScanner::FindPattern("8A 4C 24 60 8B 50 10 44 8A CE");
	CheckpointBaseAddr = reinterpret_cast<uint64_t (*)()>(*reinterpret_cast<int*>(address - 19) + address - 15);
	CheckpointHandleAddr = reinterpret_cast<uint64_t (*)(uint64_t, int)>(*reinterpret_cast<int*>(address - 9) + address - 5);
	checkpointPoolAddress = reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(address + 17) + address + 21);

	address = MemryScan::PatternScanner::FindPattern("48 63 C1 48 8D 0D ? ? ? ? F3 0F 10 04 81 F3 0F 11 05 ? ? ? ?");
	_writeWorldGravityAddress = reinterpret_cast<float*>(*reinterpret_cast<int*>(address + 6) + address + 10);
	_readWorldGravityAddress  = reinterpret_cast<float*>(*reinterpret_cast<int*>(address + 19) + address + 23);

	GenerateVehicleModelList();
}

Vector3 GTAmemory::ReadVector3(uint64_t address)
{
	const float* data = (float*)address;
	return Vector3(data[0], data[1], data[2]);
}
void GTAmemory::WriteVector3(uint64_t address, const Vector3& value)
{
	float* data = (float*)address;
	data[0]     = value.x;
	data[1]     = value.y;
	data[2]     = value.z;
}

struct HashNode
{
	int hash;
	uint16_t data;
	uint16_t padding;
	HashNode* next;
};
void GTAmemory::GenerateVehicleModelList()
{
	//Zorg
	uintptr_t address = MemryScan::PatternScanner::FindPattern("66 81 F9 ? ? 74 10 4D 85 C0");
	if (address)
	{
		address = address - 0x21;
		//uint64_t baseFuncAddr = *reinterpret_cast<int*>(address - 0x21) + address - 0x1D;
		uint64_t baseFuncAddr = address + *reinterpret_cast<int*>(address) + 0x4;
		//int classOffset = *reinterpret_cast<int*>(address + 0x10);
		int classOffset  = *reinterpret_cast<int*>(address + 0x31);
		modelHashEntries = *reinterpret_cast<uint16_t*>(baseFuncAddr + *reinterpret_cast<int*>(baseFuncAddr + 3) + 7);
		modelNum1 = *reinterpret_cast<int*>(*reinterpret_cast<int*>(baseFuncAddr + 0x52) + baseFuncAddr + 0x56); //cmp
		modelNum2 = *reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(baseFuncAddr + 0x63) + baseFuncAddr + 0x67); //mov
		modelNum3 = *reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(baseFuncAddr + 0x7A) + baseFuncAddr + 0x7E); //mul
		modelNum4 = *reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(baseFuncAddr + 0x81) + baseFuncAddr + 0x85); //add

		modelHashTable = *reinterpret_cast<uint64_t*>(*reinterpret_cast<int*>(baseFuncAddr + 0x24) + baseFuncAddr + 0x28);
		HashNode** HashMap = reinterpret_cast<HashNode**>(modelHashTable);
		//I know 0x20 items are defined but there are only 0x16 vehicle classes.
		//But keeping it at 0x20 is just being safe as the & 0x1F in theory supports up to 0x20
		auto& hashes = GTAmemory::vehicleModels;
		for (auto& vec : hashes)
			vec.clear();
		for (int i = 0; i < modelHashEntries; i++)
		{
			for (HashNode* cur = HashMap[i]; cur; cur = cur->next)
			{
				uint16_t data = cur->data;
				if ((int)data < modelNum1 && bittest(*reinterpret_cast<int*>(modelNum2 + (4 * data >> 5)), data & 0x1F))
				{
					uint64_t addr1 = modelNum4 + modelNum3 * data;
					if (addr1)
					{
						uint64_t addr2 = *reinterpret_cast<uint64_t*>(addr1);
						if (addr2)
						{
							if ((*reinterpret_cast<PBYTE>(addr2 + 157) & 0x1F) == 5)
							{
								hashes[*reinterpret_cast<PBYTE>(addr2 + classOffset) & 0x1F].push_back((unsigned int)cur->hash);
							}
						}
					}
				}
			}
		}
	}
}
bool GTAmemory::IsModelAPed(unsigned int modelHash)
{
	HashNode** HashMap = reinterpret_cast<HashNode**>(modelHashTable);
	for (HashNode* cur = HashMap[modelHash % modelHashEntries]; cur; cur = cur->next)
	{
		if ((unsigned int)cur->hash != modelHash)
		{
			continue;
		}
		uint16_t data = cur->data;
		if ((int)data < modelNum1 && bittest(*reinterpret_cast<int*>(modelNum2 + (4 * data >> 5)), data & 0x1F))
		{
			uint64_t addr1 = modelNum4 + modelNum3 * data;
			if (addr1)
			{
				uint64_t addr2 = *reinterpret_cast<uint64_t*>(addr1);
				if (addr2)
				{
					return (*reinterpret_cast<PBYTE>(addr2 + 157) & 0x1F) == 6;
				}
			}
		}
	}
	return false;
}

// Unknown_Modder
uint64_t GTAmemory::_globalTextBlockAddr;
std::vector<GTAmemory::GXT2Entry> GTAmemory::_vecGXT2Entries;
void GTAmemory::LoadGlobalGXTEntries()
{
	uint64_t address = MemryScan::PatternScanner::FindPattern("84 C0 74 34 48 8D 0D ? ? ? ? 48 8B D3");
	if (address)
	{
		address              = *reinterpret_cast<int*>(address + 7) + address + 11;
		DWORD offset         = 0xA0; // for text block "GLOBAL"
		_globalTextBlockAddr = *(uint64_t*)(address + offset);

		uint32_t numEntries = *(uint32_t*)(_globalTextBlockAddr + 4);

		for (uint32_t entry = 0; entry < numEntries; entry++)
		{
			DWORD labelHash = *(DWORD*)(_globalTextBlockAddr + 8 + entry * 8);

			DWORD strOffset = *(DWORD*)(_globalTextBlockAddr + 12 + entry * 8);

			_vecGXT2Entries.push_back({labelHash, strOffset});
		}
	}
}
void GTAmemory::EditGXTLabel(DWORD labelHash, LPCSTR string)
{
	static bool sbLoadGxtEntries = true;
	if (sbLoadGxtEntries)
	{
		sbLoadGxtEntries = false;
		LoadGlobalGXTEntries();
	}

	for (GXT2Entry& entry : _vecGXT2Entries)
	{
		if (entry.labelHash == labelHash)
		{
			strcpy_s((char*)(_globalTextBlockAddr + entry.strOffset), strlen(string), string);
		}
	}
}

uint64_t GTAmemory::GetCheckpointAddress(int handle)
{
	uint64_t addr = GTAmemory::CheckpointHandleAddr(GTAmemory::CheckpointBaseAddr(), handle);
	if (addr)
	{
		return (uint64_t)((uint64_t)(GTAmemory::checkpointPoolAddress) + 96 * *reinterpret_cast<int*>(addr + 16));
	}
	return 0;
}

uint64_t GTAmemory::GetEntityBoneMatrixAddress(int handle, int boneIndex)
{
	if ((boneIndex & 0x80000000) != 0) //boneIndex cant be negative
		return 0;

	uint64_t MemAddress = (uint64_t)getScriptHandleBaseAddress(handle);
	uint64_t Addr2      = (*(uint64_t(__fastcall**)(__int64))(*(uint64_t*)MemAddress + 88i64))(MemAddress);
	uint64_t Addr3;
	if (!Addr2)
	{
		Addr3 = *(uint64_t*)(MemAddress + 80);
		if (!Addr3)
		{
			return 0;
		}
		else
		{
			Addr3 = *(uint64_t*)(Addr3 + 40);
		}
	}
	else
	{
		Addr3 = *(uint64_t*)(Addr2 + 104);
		if (!Addr3 || !*(uint64_t*)(Addr2 + 120))
		{
			return 0;
		}
		else
		{
			Addr3 = *(uint64_t*)(Addr3 + 376);
		}
	}
	if (!Addr3)
	{
		return 0;
	}
	if (boneIndex < *(int*)(Addr3 + 32))
	{
		return uint64_t((*(uint64_t*)(Addr3 + 24) + (boneIndex << 6)));
	}
	return 0;
}

void GTAmemory::GetVehicleHandles(std::vector<Entity>& result)
{
	EntityPoolTask(EntityPoolTask::Type::Vehicle).Run(result);
}
void GTAmemory::GetVehicleHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes)
{
	auto pool         = EntityPoolTask(EntityPoolTask::Type::Vehicle);
	pool._modelHashes = modelHashes;
	pool._modelCheck  = !modelHashes.empty();
	pool.Run(result);
}
void GTAmemory::GetVehicleHandles(std::vector<Entity>& result, const Vector3& position, float radius)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Vehicle);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool.Run(result);
}
void GTAmemory::GetVehicleHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Vehicle);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool._modelHashes   = modelHashes;
	pool._modelCheck    = !modelHashes.empty();
	pool.Run(result);
}

void GTAmemory::GetPedHandles(std::vector<Entity>& result)
{
	EntityPoolTask(EntityPoolTask::Type::Ped).Run(result);
}
void GTAmemory::GetPedHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes)
{
	auto pool         = EntityPoolTask(EntityPoolTask::Type::Ped);
	pool._modelHashes = modelHashes;
	pool._modelCheck  = !modelHashes.empty();
	pool.Run(result);
}
void GTAmemory::GetPedHandles(std::vector<Entity>& result, const Vector3& position, float radius)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Ped);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool.Run(result);
}
void GTAmemory::GetPedHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Ped);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool._modelHashes   = modelHashes;
	pool._modelCheck    = !modelHashes.empty();
	pool.Run(result);
}

void GTAmemory::GetPropHandles(std::vector<Entity>& result)
{
	EntityPoolTask(EntityPoolTask::Type::Prop).Run(result);
}
void GTAmemory::GetPropHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes)
{
	auto pool         = EntityPoolTask(EntityPoolTask::Type::Prop);
	pool._modelHashes = modelHashes;
	pool._modelCheck  = !modelHashes.empty();
	pool.Run(result);
}
void GTAmemory::GetPropHandles(std::vector<Entity>& result, const Vector3& position, float radius)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Prop);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool.Run(result);
}
void GTAmemory::GetPropHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::Prop);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool._modelHashes   = modelHashes;
	pool._modelCheck    = !modelHashes.empty();
	pool.Run(result);
}

void GTAmemory::GetEntityHandles(std::vector<Entity>& result)
{
	EntityPoolTask(EntityPoolTask::Type::Ped | EntityPoolTask::Type::Vehicle | EntityPoolTask::Type::Prop).Run(result);
}
void GTAmemory::GetEntityHandles(std::vector<Entity>& result, std::vector<DWORD> modelHashes)
{
	auto pool = EntityPoolTask(EntityPoolTask::Type::Ped | EntityPoolTask::Type::Vehicle | EntityPoolTask::Type::Prop);
	pool._modelHashes = modelHashes;
	pool._modelCheck  = !modelHashes.empty();
	pool.Run(result);
}
void GTAmemory::GetEntityHandles(std::vector<Entity>& result, const Vector3& position, float radius)
{
	auto pool = EntityPoolTask(EntityPoolTask::Type::Ped | EntityPoolTask::Type::Vehicle | EntityPoolTask::Type::Prop);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool.Run(result);
}
void GTAmemory::GetEntityHandles(std::vector<Entity>& result, const Vector3& position, float radius, std::vector<DWORD> modelHashes)
{
	auto pool = EntityPoolTask(EntityPoolTask::Type::Ped | EntityPoolTask::Type::Vehicle | EntityPoolTask::Type::Prop);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool._modelHashes   = modelHashes;
	pool._modelCheck    = !modelHashes.empty();
	pool.Run(result);
}

void GTAmemory::GetPickupObjectHandles(std::vector<int>& result)
{
	auto pool = EntityPoolTask(EntityPoolTask::Type::PickupObject);
	pool.Run(result);
}
void GTAmemory::GetPickupObjectHandles(std::vector<int>& result, const Vector3& position, float radius)
{
	auto pool           = EntityPoolTask(EntityPoolTask::Type::PickupObject);
	pool._position      = position;
	pool._radiusSquared = radius * radius;
	pool._posCheck      = true;
	pool.Run(result);
}

void GTAmemory::GetCheckpointHandles(std::vector<int>& result)
{
	uint64_t addr = GTAmemory::CheckpointBaseAddr();
	uint8_t count = 0;
	for (uint64_t i = *(uint64_t*)(addr + 48); i && count < 64; i = *(uint64_t*)(i + 24))
	{
		result.push_back(*(int*)(i + 12));
	}
}

int GTAmemory::GetNumberOfVehicles()
{
	if (*_vehiclePoolAddress)
	{
		VehiclePool* pool = *reinterpret_cast<VehiclePool**>(*_vehiclePoolAddress);
		return pool->itemCount;
	}
	return 0;
}

float GTAmemory::WorldGravity_get()
{
	return *_readWorldGravityAddress;
}
void GTAmemory::WorldGravity_set(float value)
{
	*_writeWorldGravityAddress = value;
	MISC::SET_GRAVITY_LEVEL(0);
}

//--------------------------------GeneralGlobalHax-----------------------------------------------

void GeneralGlobalHax::DisableAnnoyingRecordingUI(bool uSure)
{
	// Has to be updated every patch.
	switch (GTAmemory::GetGameVersion())
	{
	case eGameVersion::VER_1_0_757_4_NOSTEAM:
	case eGameVersion::VER_1_0_757_4_STEAM:
	case eGameVersion::VER_1_0_791_2_NOSTEAM:
	case eGameVersion::VER_1_0_791_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x42DE + 0x9) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_877_1_NOSTEAM:
	case eGameVersion::VER_1_0_877_1_STEAM:
	case eGameVersion::VER_1_0_944_2_NOSTEAM:
	case eGameVersion::VER_1_0_944_2_STEAM:
	case eGameVersion::VER_1_0_1011_1_NOSTEAM:
	case eGameVersion::VER_1_0_1011_1_STEAM:
	case eGameVersion::VER_1_0_1032_1_NOSTEAM:
	case eGameVersion::VER_1_0_1032_1_STEAM:
	case eGameVersion::VER_1_0_1103_2_NOSTEAM:
	case eGameVersion::VER_1_0_1103_2_STEAM:
	case eGameVersion::VER_1_0_1180_2_NOSTEAM:
	case eGameVersion::VER_1_0_1180_2_STEAM:
		*GTAmemory::GetGlobalPtr<INT32>(0x42FF + 0x9)  = uSure ? 1 : 0;
		*GTAmemory::GetGlobalPtr<INT32>(0x42FF + 0x82) = uSure ? 1 : 0;
		break;
	case eGameVersion::VER_1_0_1290_1_NOSTEAM:
	case eGameVersion::VER_1_0_1290_1_STEAM:
	case eGameVersion::VER_1_0_1365_1_NOSTEAM:
	case eGameVersion::VER_1_0_1365_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x430A + 0x82) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1493_0_NOSTEAM:
	case eGameVersion::VER_1_0_1493_0_STEAM:
	case eGameVersion::VER_1_0_1493_1_NOSTEAM:
	case eGameVersion::VER_1_0_1493_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x4336 + 0x82) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1604_0_NOSTEAM:
	case eGameVersion::VER_1_0_1604_0_STEAM:
	case eGameVersion::VER_1_0_1604_1_NOSTEAM:
	case eGameVersion::VER_1_0_1604_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x434C + 0x82) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1737_0_NOSTEAM:
	case eGameVersion::VER_1_0_1737_0_STEAM:
	case eGameVersion::VER_1_0_1737_6_NOSTEAM:
	case eGameVersion::VER_1_0_1737_6_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x4378 + 0x82) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1868_0_NOSTEAM:
	case eGameVersion::VER_1_0_1868_0_STEAM:
	case eGameVersion::VER_1_0_1868_1_NOSTEAM:
	case eGameVersion::VER_1_0_1868_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x56C3 + 0x82) = uSure ? 1 : 0; break;
	}
}
void GeneralGlobalHax::EnableBlockedMpVehiclesInSp(bool uSure)
{
	// Has to be updated every patch.

	switch (GTAmemory::GetGameVersion())
	{
	case eGameVersion::VER_1_0_757_4_NOSTEAM:
	case eGameVersion::VER_1_0_757_4_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x271803) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_791_2_NOSTEAM:
	case eGameVersion::VER_1_0_791_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x272A34) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_877_1_NOSTEAM:
	case eGameVersion::VER_1_0_877_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x2750BD) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_944_2_NOSTEAM:
	case eGameVersion::VER_1_0_944_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(0x279476) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1011_1_NOSTEAM:
	case eGameVersion::VER_1_0_1011_1_STEAM:
	case eGameVersion::VER_1_0_1032_1_NOSTEAM:
	case eGameVersion::VER_1_0_1032_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(2593970) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1103_2_NOSTEAM:
	case eGameVersion::VER_1_0_1103_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(2599337) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1180_2_NOSTEAM:
	case eGameVersion::VER_1_0_1180_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(2606794) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1290_1_NOSTEAM:
	case eGameVersion::VER_1_0_1290_1_STEAM:
	case eGameVersion::VER_1_0_1365_1_NOSTEAM:
	case eGameVersion::VER_1_0_1365_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4265719) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1493_0_NOSTEAM:
	case eGameVersion::VER_1_0_1493_0_STEAM:
	case eGameVersion::VER_1_0_1493_1_NOSTEAM:
	case eGameVersion::VER_1_0_1493_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4266042) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1604_0_NOSTEAM:
	case eGameVersion::VER_1_0_1604_0_STEAM:
	case eGameVersion::VER_1_0_1604_1_NOSTEAM:
	case eGameVersion::VER_1_0_1604_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4266905) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1737_0_NOSTEAM:
	case eGameVersion::VER_1_0_1737_0_STEAM:
	case eGameVersion::VER_1_0_1737_6_NOSTEAM:
	case eGameVersion::VER_1_0_1737_6_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4267883) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_1868_0_NOSTEAM:
	case eGameVersion::VER_1_0_1868_0_STEAM:
	case eGameVersion::VER_1_0_1868_1_NOSTEAM:
	case eGameVersion::VER_1_0_1868_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4268190) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2060_0_NOSTEAM:
	case eGameVersion::VER_1_0_2060_0_STEAM:
	case eGameVersion::VER_1_0_2060_1_NOSTEAM:
	case eGameVersion::VER_1_0_2060_1_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4268340) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2189_0_NOSTEAM:
	case eGameVersion::VER_1_0_2189_0_STEAM:
	case eGameVersion::VER_1_0_2215_0_NOSTEAM:
	case eGameVersion::VER_1_0_2215_0_STEAM:
	case eGameVersion::VER_1_0_2245_0_NOSTEAM:
	case eGameVersion::VER_1_0_2245_0_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4269479) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2372_0_NOSTEAM:
	case eGameVersion::VER_1_0_2372_0_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4270934) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2545_0_NOSTEAM:
	case eGameVersion::VER_1_0_2545_0_STEAM:
	case eGameVersion::VER_1_0_2612_1_NOSTEAM:
	case eGameVersion::VER_1_0_2612_1_STEAM:
	case eGameVersion::VER_1_0_2628_2_NOSTEAM:
	case eGameVersion::VER_1_0_2628_2_STEAM: *GTAmemory::GetGlobalPtr<INT32>(4533757) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2699_0_NOSTEAM:
	case eGameVersion::VER_1_0_2699_0_STEAM:
	case eGameVersion::VER_1_0_2699_16: *GTAmemory::GetGlobalPtr<INT32>(4539659) = uSure ? 1 : 0; break;
	case eGameVersion::VER_1_0_2802_0:
	default: *GTAmemory::GetGlobalPtr<INT32>(4540726) = uSure ? 1 : 0; break;
	}
}

void** GeneralGlobalHax::WorldPtrPtr()
{
	static DWORD64 __dwWorldPtrAddr = 0x0U;
	if (!__dwWorldPtrAddr)
	{
		__dwWorldPtrAddr = MemryScan::PatternScanner::FindPattern("48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81");
		if (__dwWorldPtrAddr)
		{
			__dwWorldPtrAddr = __dwWorldPtrAddr + *reinterpret_cast<int*>(__dwWorldPtrAddr + 3) + 7;
		}
	}
	return reinterpret_cast<void**>(__dwWorldPtrAddr);
}
float GeneralGlobalHax::GetPlayerHeight()
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_2802_0)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x88}));
	}
	return 1.0f;
}
void GeneralGlobalHax::SetPlayerHeight(float value)
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_2802_0)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x88})) = value;
	}
}
float GeneralGlobalHax::GetPlayerSwimSpeed()
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_791_2_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x1088, 0xE4}));
		else if (gameVersion <= eGameVersion::VER_1_0_877_1_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10A8, 0xE4}));
		else if (gameVersion <= eGameVersion::VER_1_0_1103_2_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0xE4}));
		else if (gameVersion <= eGameVersion::VER_1_0_1868_1_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0x148}));
		else if (gameVersion <= eGameVersion::VER_1_0_2215_0_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0x150}));
		else if (gameVersion <= eGameVersion::VER_1_0_2245_0_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0x170}));
	}
	return 1.0f;
}
void GeneralGlobalHax::SetPlayerSwimSpeed(float value)
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_791_2_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x1088, 0xE4})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_877_1_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10A8, 0xE4})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_1103_2_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0xE4})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_1868_1_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0x148})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_2215_0_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0x150})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_2245_0_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0x170})) = value;
	}
}
float GeneralGlobalHax::GetPlayerMovementSpeed()
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_791_2_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x1088, 0xE8}));
		else if (gameVersion <= eGameVersion::VER_1_0_877_1_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10A8, 0xE8}));
		else if (gameVersion <= eGameVersion::VER_1_0_1103_2_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0xE8}));
		else if (gameVersion <= eGameVersion::VER_1_0_1868_1_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0x14C}));
		else if (gameVersion <= eGameVersion::VER_1_0_2215_0_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0xCD0}));
		else if (gameVersion <= eGameVersion::VER_1_0_2245_0_NOSTEAM)
			return *(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0xCF0}));
	}
	return 1.0f;
}
void GeneralGlobalHax::SetPlayerMovementSpeed(float value)
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_791_2_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x1088, 0xE8})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_877_1_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10A8, 0xE8})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_1103_2_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0xE8})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_1868_1_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10B8, 0x14C})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_2215_0_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0xCD0})) = value;
		else if (gameVersion <= eGameVersion::VER_1_0_2245_0_NOSTEAM)
			*(GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0x10C8, 0xCF0})) = value;
	}
}

int GeneralGlobalHax::GetVehicleBoostState()
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_2372_0_NOSTEAM)
			return *(GetMultilayerPointer<int*>(baddr, std::vector<DWORD>{0x8, 0xD28, 0x318}));
	}
	return 0;
}
void GeneralGlobalHax::SetVehicleBoostState(int value)
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_2372_0_NOSTEAM)
			*(GetMultilayerPointer<int*>(baddr, std::vector<DWORD>{0x8, 0xD28, 0x318})) = value;
	}
}
float* GeneralGlobalHax::GetVehicleBoostChargePtr()
{
	auto baddr = *GeneralGlobalHax::WorldPtrPtr();
	if (baddr)
	{
		auto gameVersion = GTAmemory::GetGameVersion();
		if (gameVersion <= eGameVersion::VER_1_0_1103_2_NOSTEAM)
			return (GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0xD28, 0x31C}));
		if (gameVersion <= eGameVersion::VER_1_0_2372_0_NOSTEAM) //VER_1_0_1868_1_NOSTEAM
			return (GetMultilayerPointer<float*>(baddr, std::vector<DWORD>{0x8, 0xD28, 0x320})); // Might be off
	}
	return nullptr;
}
