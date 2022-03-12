/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

/*
	heterogeneous architecture����, 
	TaskQueue�� lambda�� Ȱ���Ͽ�, ���� ������ ���� �񵿱� Task���޿� ���� ������ �����Դϴ�.
*/

#include "Framework.h"

int main( int /* argc */, char* /*argv[]*/ )
{
	// �⺻������, �а� �������������� homogeneous ������� ���� ������ �ۼ��غ������ű� ������,
	// heterogeneous ����� ���� ������ ������ �غ��߽��ϴ�.
	// ���� ���� ��, heterogeneous ������ ���� ���ؿ� ������ �Ǽ����� ���ڽ��ϴ�.
	Framework::GetInstance().Run();
	
	std::system( "pause" );
}