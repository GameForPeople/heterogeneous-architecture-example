/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <string>

class RoomUnit;

class UserUnit
{
public:
	UserUnit( const std::string& id );

public:
	const std::string m_id;
	      RoomUnit*   m_roomPtr;
};