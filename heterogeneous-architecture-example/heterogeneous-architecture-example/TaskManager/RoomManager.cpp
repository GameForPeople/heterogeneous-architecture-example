/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "RoomManager.h"
#include "RoomUnit.h"

RoomManager::RoomManager()
	: BaseTaskManager()
	, m_roomCont     ()
{
}

RoomManager::~RoomManager()
{
	for ( const auto& [ notuse, roomPtr ] : m_roomCont )
	{
		delete roomPtr;
	}

	m_roomCont.clear();
}