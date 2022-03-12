/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "UserManager.h"
#include "UserUnit.h"

UserManager::UserManager()
	: BaseTaskManager()
	, m_userCont     ()
{
}

UserManager::~UserManager()
{
	ForEach( []( UserUnit* user ){ delete user; } );
	m_userCont.clear();
}

void UserManager::ForEach( const std::function< void( UserUnit* ) >& func )
{
	if ( !func )
		return;

	for ( const auto& [ id, user ] : m_userCont )
	{
		func( user );
	}
}
