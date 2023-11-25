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

#include "STSTask.h"

#include <vector>

typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned long DWORD;

namespace sub::Spooner
{
	class SpoonerTaskSequence final // Dtor deallocates used memory blocks
	{
	private:
		bool bJustJumpedToNext;
		std::vector<STSTask*> tasks;
		int16_t progress;
		DWORD timer;

	public:
		SpoonerTaskSequence();

		void operator=(const SpoonerTaskSequence& right);

		template<typename R = STSTask>
		inline R* GetTask(uint16_t index)
		{
			return reinterpret_cast<R*>(this->tasks[index]); // Note: no outofbounds check
		}

		bool ContainsType(const STSTaskType& value);

		uint16_t TaskCount() const;
		bool empty() const;

		inline void AddTask(STSTask* tskPtr);
		STSTask* AddTask(const STSTaskType& ofType);

		static void DeallocTask(STSTask* tskPtr);
		void RemoveTask(uint16_t index);

		void SwapTasks(uint16_t index1, uint16_t index2);

		std::vector<STSTask*>& AllTasks();

		bool IsActive() const;
		void Start();
		void Reset(bool deleteTasks = false);
		void Tick(void* ev);
	};

	/// Spooner ped options sub goes to task sequence (taskList) submenu.
	// In taskList submenu, show name of each task in AllTasks() and have ADD NEW option at bottom. Should be a START[Start()] / STOP[Reset(false) and ClearPedTasksImm] button right below it.
	/// AddNew submenu should name all tasks and pressing should take back to taskList submenu with added task.
	// X/B to delete selected task. Show in button instructions on the bottom right.
	/// Press task for task settings. Settings should be a [one to many funcs] submenu with 'Duration' in the main submenu func. Custom appropriate settings in each sub.
	// Save to Spooner xml k

}
