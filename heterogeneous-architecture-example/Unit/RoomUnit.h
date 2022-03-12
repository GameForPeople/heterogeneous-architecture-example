/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <string>
#include <map>
#include <functional>

class UserUnit;

class RoomUnit
{
public:
	RoomUnit( const std::string& name );

public:
	void ForEach( const std::function< void( UserUnit* ) >& func );

public:
	const std::string                                 m_roomName;
	      std::map< std::string /* id */, UserUnit* > m_roomUserCont;
};