/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "SpoonerEntity.h"

namespace sub::Spooner
{
	std::string SpoonerEntity::TypeName()
	{
		switch (this->Type)
		{
		case EntityType::PED: return "PED"; break;
		case EntityType::VEHICLE: return "VEHICLE"; break;
		case EntityType::PROP: return "OBJECT"; break;
		default: return "UNK"; break;
		}
	}

	SpoonerEntity::SpoonerEntity()
	{
		this->Dynamic                   = false;
		this->AttachmentArgs.isAttached = false;
		this->AttachmentArgs.boneIndex  = 0;
		this->TextureVariation          = 0;
		this->IsStill                   = false;
	}
	SpoonerEntity::SpoonerEntity(const SpoonerEntity& right)
	{
		this->Handle                    = right.Handle;
		this->Type                      = (EntityType)right.Type;
		this->HashName                  = right.HashName;
		this->Dynamic                   = right.Dynamic;
		this->LastAnimation.dict        = right.LastAnimation.dict;
		this->LastAnimation.name        = right.LastAnimation.name;
		this->AttachmentArgs.isAttached = right.AttachmentArgs.isAttached;
		this->AttachmentArgs.boneIndex  = right.AttachmentArgs.boneIndex;
		this->AttachmentArgs.offset     = right.AttachmentArgs.offset;
		this->AttachmentArgs.rotation   = right.AttachmentArgs.rotation;
		this->TextureVariation          = right.TextureVariation;
		this->IsStill                   = right.IsStill;
		this->TaskSequence              = right.TaskSequence;
	}

	bool operator==(const SpoonerEntity& left, const SpoonerEntity& right)
	{
		return left.Handle == right.Handle;
	}
	bool operator!=(const SpoonerEntity& left, const SpoonerEntity& right)
	{
		return left.Handle != right.Handle;
	}
	bool operator==(const SpoonerEntity& left, const GTAentity& right)
	{
		return left.Handle == right;
	}
	bool operator!=(const SpoonerEntity& left, const GTAentity& right)
	{
		return left.Handle != right;
	}


	SpoonerEntity SelectedEntity;
	std::vector<SpoonerEntity> SelectedSpoonGroup;

}
