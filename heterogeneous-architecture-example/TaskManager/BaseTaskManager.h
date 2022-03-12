/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include "AutoCall.hpp"

#include <concurrent_queue.h>
#include <thread>
#include <functional>

class BaseTaskManager
{
public:
	using TaskType = std::function< void( void ) >;
	
public:
	static constexpr int UPDATE_INTERVAL_MS = 100; // 0.1s

public:
	BaseTaskManager();
	virtual ~BaseTaskManager();

	[[nodiscard]] Base::AutoCall Run();

	void PushTask( const TaskType& task );

protected:
	virtual           void        _OnUpdate();
	virtual constexpr int         _GetUpdateIntervalMilliseconds() const noexcept = 0;
	virtual constexpr std::string _GetName()                       const noexcept = 0;

private:
	void _ConsumeTaskAll();

private:
	concurrency::concurrent_queue< TaskType > m_taskQueue;
	std::jthread                              m_taskThread;
};