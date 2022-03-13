/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include "Singleton.hpp"

class Framework
	: public Base::TSingleton< Framework >
{
public:
	void Run();

private:
	void 유저생성_혹은_로그인_예제();
	void 방_생성_예제();
	void 채팅_예제();
};