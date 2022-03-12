/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <type_traits>

namespace Base
{
	template< 
		typename T, 
		typename = std::void_t< decltype( std::declval< T >() ) > /* = need default ctor! */ >
	class TSingleton
	{
	public:
		static T& GetInstance()
		{
			static T instance;
			return instance;
		}
	
	protected:
		TSingleton()  = default;
		~TSingleton() = default;
	
	public:
		TSingleton( TSingleton const& )            = delete;
		TSingleton& operator=( TSingleton const& ) = delete;
	
		TSingleton( TSingleton&& )            = delete;
		TSingleton& operator=( TSingleton&& ) = delete;
	};
}
