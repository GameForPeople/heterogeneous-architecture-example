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
	public:
		[[nodiscard]] static std::shared_ptr< AutoCall > Make( const std::function< void() >& func )
		{
			return std::make_shared< AutoCall >( func );
		}

	public:
		AutoCall( const std::function< void() >& func ) noexcept
			: func( func )
		{
		}

		~AutoCall()
		{
			if ( func ) { func(); }
		}

	private:
		const std::function< void() > func;
	};

	using AutoCallPtr = std::shared_ptr< AutoCall >;
}
