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

class UserUnit;

class UserManager final
	: public BaseTaskManager
	, public Base::TSingleton< UserManager >
{
public:
	static constexpr int MAX_USER_NUM = 100;

public:
	UserManager();
	virtual ~UserManager();

	void ForEach( const std::function< void( UserUnit* ) >& func );

protected:
	virtual constexpr int         _GetUpdateIntervalMilliseconds() const noexcept final { return BaseTaskManager::UPDATE_INTERVAL_MS; }
	virtual constexpr std::string _GetName()                       const noexcept final { return "UserManager";                       }
/*
	������������ �ڵ� �ۼ��� ����, public���� ��� ����� �����մϴ�.
*/
public:
	std::map< std::string /* key */, UserUnit* > m_userCont;

};