/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "RoomUnit.h"

RoomUnit::RoomUnit( const std::string& name )
	: m_roomName    ( name )
	, m_roomUserCont()
{
}

void RoomUnit::ForEach( const std::function< void( UserUnit* ) >& func )
{
	if ( !func )
		return;

	for ( const auto& [ id, user ] : m_roomUserCont )
	{
		func( user );
	}
}
