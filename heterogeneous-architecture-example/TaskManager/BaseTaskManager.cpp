/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "BaseTaskManager.h"
#include "LogUtil.h"

#include <chrono>

BaseTaskManager::BaseTaskManager()
	: m_taskQueue ()
	, m_taskThread()
{
}

BaseTaskManager::~BaseTaskManager() = default;

Base::AutoCall BaseTaskManager::Run()
{
	m_taskThread = static_cast< std::jthread >(
			[ this ]( std::stop_token stoken )
			{
				PRINT_LOG( "Run Task Manager! // Name : " + _GetName() + " ThreadId : " + Log::GetThreadIdString() );
				
				while ( !stoken.stop_requested() )
				{
					_OnUpdate();
					_ConsumeTaskAll();

					std::this_thread::sleep_for( 
						static_cast< std::chrono::milliseconds >( _GetUpdateIntervalMilliseconds() ) );
				}
			} );

	return Base::AutoCall( 
		[ & ]()
		{ 
			m_taskThread.request_stop();
		
			if ( m_taskThread.joinable() )
				m_taskThread.join();
		} );
}

void BaseTaskManager::PushTask( const TaskType& task )
{
	m_taskQueue.push( task );
}

void BaseTaskManager::_OnUpdate()
{
	/* Default - void */
}

void BaseTaskManager::_ConsumeTaskAll()
{
	TaskType retTask = nullptr;

	while ( m_taskQueue.try_pop( retTask ) )
	{
		retTask();
	}
}