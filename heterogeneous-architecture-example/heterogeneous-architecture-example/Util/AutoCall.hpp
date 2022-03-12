/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <functional>

namespace Base
{
	class AutoCall
	{
		const std::function< void() > func;

	public:
		AutoCall( const std::function< void() >& func ) noexcept
			: func( func )
		{
		}

		~AutoCall()
		{
			if ( func ) { func(); }
		}
	};
}