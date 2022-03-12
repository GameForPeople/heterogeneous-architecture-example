/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <string>

#define __FUNCTION_NAME__ __FUNCTION__
#define __FILENAME__      (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define SOURCE_LOCATION   {__LINE__, __FILENAME__, __FUNCTION_NAME__}

namespace Log
{
	struct SourceLocation
	{
		int         fileLine;
		const char* fileName;
		const char* functionName;

		SourceLocation( const int fileLine, const char* fileName, const char* functionName );
		~SourceLocation();

		std::string GetString() const;
	};

	void Print( const SourceLocation& sourceLocation, const std::string& msg );

	std::string GetThreadIdString();
};

#define PRINT_LOG( logMessage ) Log::Print( SOURCE_LOCATION, logMessage );