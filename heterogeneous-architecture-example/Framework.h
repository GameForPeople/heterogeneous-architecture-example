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
	void ��������_Ȥ��_�α���_����();
	void ��_����_����();
	void ä��_����();
};