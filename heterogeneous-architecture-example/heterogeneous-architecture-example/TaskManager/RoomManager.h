/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include "BaseTaskManager.h"
#include "Singleton.hpp"

#include <map>
#include <string>

class RoomUnit;

class RoomManager final
	: public BaseTaskManager
	, public Base::TSingleton< RoomManager >
{
public:
	static constexpr int MAX_ROOM_NUM = 10;

public:
	RoomManager();
	virtual ~RoomManager();

protected:
	virtual constexpr int         _GetUpdateIntervalMilliseconds() const noexcept final { return BaseTaskManager::UPDATE_INTERVAL_MS; }
	virtual constexpr std::string _GetName()                       const noexcept final { return "RoomManager";                       }

	/*
	������������ �ڵ� �ۼ��� ����, public���� ��� ����� �����մϴ�.
	*/
public:
	std::map< std::string /* key */, RoomUnit* > m_roomCont;
};