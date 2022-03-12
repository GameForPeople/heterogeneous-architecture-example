/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "UserUnit.h"

#include "RoomUnit.h"

UserUnit::UserUnit( const std::string& id )
	: m_id     ( id )
	, m_roomPtr( nullptr )
{
}
