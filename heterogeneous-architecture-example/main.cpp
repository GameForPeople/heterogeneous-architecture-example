/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

/*
	heterogeneous architecture에서, 
	TaskQueue와 lambda를 활용하여, 여러 스레드 간의 비동기 Task전달에 관한 간단한 예제입니다.
*/

#include "Framework.h"

int main( int /* argc */, char* /*argv[]*/ )
{
	// 기본적으로, 학과 교육과정에서는 homogeneous 방식으로 게임 서버를 작성해보셨을거기 때문에,
	// heterogeneous 방식의 정말 간단한 예제를 준비했습니다.
	// 과제 진행 및, heterogeneous 구조에 대한 이해에 도움이 되셨으면 좋겠습니다.
	Framework::GetInstance().Run();
	
	std::system( "pause" );
}