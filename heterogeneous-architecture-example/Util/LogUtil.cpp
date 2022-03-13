/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "LogUtil.h"

#include <sstream>
#include <iostream>
#include <mutex>
#include <concurrent_unordered_map.h>

namespace Log
{
	static concurrency::concurrent_unordered_map< std::thread::id, std::string > g_threadIdAndNameMap;

SourceLocation::SourceLocation( const int fileLine, const char* fileName, const char* functionName )
	: fileLine    ( fileLine     )
	, fileName    ( fileName     )
	, functionName( functionName )
{
}

SourceLocation::~SourceLocation() = default;

std::string SourceLocation::GetString() const
{
	std::ostringstream os;

	os
		// << "[ "
		<< std::string( fileName )
		<< " ("
		<< std::to_string( fileLine )
		<< ")"
		// << std::string( functionName )
		<< " ]";

	return os.str();
}

void Print( const SourceLocation& sourceLocation, const std::string& message )
{
	static std::mutex coutLock{};

	{
		std::lock_guard< std::mutex > localLock( coutLock );

		//? 이게 맞아?
		if ( message.size() )
			std::cout << "[ LOG | " << sourceLocation.GetString() << " " << message << std::endl;
		else
			std::cout << "\n";
	}
}

void PrintWithThreadId( const SourceLocation& sourceLocation, const std::string& msg )
{
	const std::string name = g_threadIdAndNameMap[ std::this_thread::get_id() ];
	//Print( sourceLocation, "[Thread:" + name + ":" + GetThreadIdString() + "] " + msg );
	Print( sourceLocation, "[Thread:" + name + "] " + msg );
}

void AddThreadIdAndName( const std::string& name )
{
	g_threadIdAndNameMap[ std::this_thread::get_id() ] = name;
}

std::string GetThreadIdString()
{
	const std::thread::id myid = std::this_thread::get_id();
	
	std::stringstream ss;
	ss << myid;
	
	return ss.str();
}

};
