/*
	Copyright 2022, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "Framework.h"

#include "LogUtil.h"

#include "UserManager.h"
#include "RoomManager.h"

#include "UserUnit.h"
#include "RoomUnit.h"

enum class ChatType
{
	Lobby,   // 로비 채팅
	Room,    // 룸 채팅
};

void Framework::Run()
{
	/*
		0. 예제 프로그램은 1주차 과제인 채팅 서버를 생각해주시면 될 것 같습니다.

		1. 이 예제에서는 총 3개의 스레드가 존재합니다.
		 - 메인 스레드        : Framework::Run()을 호출한 main스레드 ( IOCP일때를 생각하면, Recv가 이루어지는 I/O 스레드라고 생각해도 좋을 것 같습니다! )
		 - UserManager 스레드 : UserManager의 Task를 돌리는 스레드, 유저와 로비와 관련된 업무를 담당합니다.
		 - RoomManager 스레드 : RoomManager의 Task를 돌리는 스레드, 방 생성과 관련된 업무를 담당합니다.

		2. 이 예제의 핵심은 TaskManager 클래스에서, PushTask 함수를 통해, Task를 비동기적으로 다른 Context로 넘기는 것입니다.
			- 임의의 Context에서 Target Context를 향해 PushTask로 Task를 넘길 때,
				- Target객체의 MS-Safe Queue(PPL)에 Push해서, MP에 안전하게 처리하고 직렬화하고,
				- 이후, Target객체가 자신이 유효한 Context에서 Pop함으로서, SC에 안전하게 처리합니다.
			- 이러한 TaskManager를 여러개를 배치하여 서버 아키텍처를 구성함으로서, 여러 Context를 처리할 수 있습니다.
			- 이 예제에서는 lambda를 통해 다른 Context로 넘기고 있습니다.
				- 각 TaskUnit Class를 따로 만들어, Command 패턴처럼 처리할 수도 있고, 다른 여러 방식들도 존재합니다만 이 예제에서는 lambda를 사용하고 있습니다.
			- 글로만으로는 명확하게 이해가 어려울 수 있습니다. 아래 예제를 쭉 따라서 확인하시는 것을 추천드립니다.

		3. 두 분이 절차지향적 코드에 익숙해 하셔서, 본 코드의 목적을 달성하기 위해, 임의로 절차지향적으로 작성하였습니다.
			아래의 항목은 분명히 좋은 코드가 아닙니다. 참고부탁드립니다.
			- 하나의 함수가 비정상적으로 길어지는 것.
			- 각 객체의 멤버를 public으로 노출하는 것.
			- 중복 로직을 함수화 하지 않는 것.

		4. 총 3가지 기능을 예제로 구현하였습니다.
			0. 유저생성 혹은 로그인 예제
			1. 방생성 예제
			2. 채팅 예제
	*/

	Log::AddThreadIdAndName( "MainThread" );
	PRINT_LOG_WITH_THREADID( "Run Main Thread!" );
	const auto userThreadStopAutoCall = UserManager::GetInstance().Run();
	const auto roomThreadStopAutoCall = RoomManager::GetInstance().Run();

	std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );

	// 예제들
	{
		유저생성_혹은_로그인_예제();
		방_생성_예제();
		채팅_예제();
	}

#pragma region 예제를_마치며
	/*
		이렇게 김민주님과 성기홍님이 1주차 과제때 해주신 채팅 서버의 몇몇 기능들을!
		단일스레드 방식에서 벗어나, 간단하게나마 heterogeneous 아키텍처로 구현해보았습니다.

		저도 학교 다닐때는 정내훈 교수님께 homogeneous방식만 배웠다가, 
		이후 heterogeneous방식을 공부하면서, 여러 코드를 작성하는 과정에서 많이 낯설었고 고통스러웠던 기억이 납니다.

		사실 위 예제로 구현된 heterogeneous 방식은 위에서 말씀드린 문제 외에도, 여러 문제를 갖고 있습니다.
			- 각 Manager 별로 Thread를 소유하고 있어, Context별로 부하 분산이 되지 않는 이슈
			- 여러개의 Room들이 하나의 RoomManager Thread에 의해 돌아가는 문제
			- 하나의 Task를 처리하는 과정에서 Atomic 연산이 빈번하게 사용( concurrent_queue의 push-try_pop)되는 문제들
			- 기타 여러 문제들.

		heterogeneous로 진행해보시면서, 위와 같은 문제들을 해결하기위해 많은 고민을 하게 되실거고, 해결이 쉽지 않을 거고, 사실 해결하시지 않으셔도 됩니다.
		다만 그러한 과정을 통해, 더 다양하고 깊은 시각을 갖게 되실거고, 서버 프로그래머로서 역량적으로 성장할 수 있는 좋은 기회가 되실 것이라고 생각합니다.

		진행하시면서 모르시는 것, 궁금한 것, 상의가 필요한 것, 여러가지 답중에서 가장 좋은 답을 골라낼때의 조언 등, 
		필요하시면 언제든지 연락부탁드립니다.
		
		마지막 2주의 넷마블네오 공통 교육 기간, 많이 힘드시겠지만 화이팅 하셨으면 좋겠습니다....ㅎ
		저 원래 꼰대인건 알았는데 이번에 멘토링하면서 느끼는데 너무 꼰대네요....젊꼰은 답도 없다는데....진심으로 죄송합니다...ㅠ

		화이팅!! :)
	*/
#pragma endregion
}

void Framework::유저생성_혹은_로그인_예제()
{
	//	먼저 유저 생성 혹은, 로그인과 관련된 예제입니다.
	//	 - 간단한 예제이다보니, 따로 DB를 붙이지 않았습니다. 유저 생성과 로그인이 동시에 이루어진다고 가정합니다.

	//	 - 유저 생성 및 로그인의 경우는 다음의 로직이 필요합니다.
	//		0. 최대 동접수를 넘었는지 여부를 확인합니다. 
	//		1. 요청받은 Id가 기존에 있는 아이디인지 확인합니다.
	//		2. 유저 객체를 생성하여, 유저 컨테이너에 등록합니다.

	//	- 이 과정에서 확인해야할 변수는 다음과 같습니다.
	//		- 현재 로그인 한 전체 유저 컨테이너
	//			- 우리는 이 변수를, Id를 키로 가지고, UserUnit을 Data로 가지는 Map으로 처리할 겁니다.
				UserManager::GetInstance().m_userCont; // <- 바로 이 친구입니다!
	//	- 다만, 여기서 문제가 있습니다. Map은 Multithread에 Safe하지 않은 데이터형이기 때문에, 여러 쓰레드에서나 접근할 경우에는 미정의 동작입니다.
	//	- 그렇기 때문에, 우리는 아무 스레드에서 처리하지 않고, User관련된 업무는! UserManager에게 처리하도록 약속합시다. 

	//	- UserManager에서 유저 생성 혹은 로그인과 관련된 코드는 다음과 같습니다.
	{
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );

		PRINT_LOG( "시작 ---------- A. 유저 생성 혹은 로그인 예제 " );
		PRINT_LOG_WITH_THREADID( "A-0" );

		const std::string id = "NetmarbleNeo";
		UserManager::GetInstance().PushTask(
			// 테스크는 람다로 넘깁니다! 윤정현 교수님의 C++ 혹은 STL때 아마 람다에 대하여 배우셨을 겁니다!
			[ id ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "A-1" );

				auto& userCont = UserManager::GetInstance().m_userCont;

				// 예외 처리
				{
					if ( 
						const int currentUserCount = userCont.size()
						; currentUserCount >= UserManager::MAX_USER_NUM )
					{
						// 로그인 실패! 이미 최대 동접 수 입니다.
						PRINT_LOG_WITH_THREADID( "로그인 실패! 이미 최대 동접! // 현재 동접 수 : " + std::to_string( currentUserCount ) );
						return;
					}

					if (
						const auto idCheckResult = userCont.find( id )
						; idCheckResult != userCont.end() )
					{
						// 이미 접속해있는 Id입니다!
						PRINT_LOG_WITH_THREADID( "로그인 실패! 이미 접속한 ID // id : " + id );
						return;
					}
				}

				// 예외를 모두 통과하였습니다! 유저를 등록합니다.
				userCont[ id ] = new UserUnit( id );
				PRINT_LOG_WITH_THREADID( "로그인 성공! // id : " + id );

				UserManager::GetInstance().ForEach( 
					[]( UserUnit* user )
					{
						// 이 부분도 람다로 작성되어 있어 착각할수 있는데, ForEach는 PushTask와 다르게 동기함수입니다.
						PRINT_LOG_WITH_THREADID( "A-2" );

						// 유저 여러분들~~ 애 로그인 했어요~~
						// user->Send( UserLogin );
					} );
			} );

		PRINT_LOG_WITH_THREADID( "A-3" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "종료 ---------- A. 유저 생성 혹은 로그인 예제 " );
	}

	// 위 예제에서 중요하게 봐야할 몇가지들이 있습니다.
	
	//	0. 먼저, A-0, A-1, A-2 로그를 확인해보세요!
	//		- 로그가 찍힌 순서는 아마, A-0 -> A-3 -> A-1 -> A-2일 것입니다.
	//		- 여기서 우리는, 아! A-0, A-3과 A-1, A-2의 비동기적으로 이루어지는구나 라는 사실을 알 수 있습니다.
	//		- ContextName - ThreadID를 확인해보면 더 명확하게 인지할 수 있습니다.

	//	1. 다음으로는, 어떤 변수 혹은 함수가 어떤 스레드에서 접근해야하는 지를 제한하는 장치가 따로 없기 때문에 항상 생각해야한다는 점입니다.
	//		- 예를 들면 아래의 코드는 미정의 동작입니다.
			
			// RoomManager Task를 실행합니다.
			RoomManager::GetInstance().PushTask( []() 
				{ 
					// ? 뭐여 애는 유저매니저Task에서만 접근해야하는 변수인데?
					auto& userCont = UserManager::GetInstance().m_userCont;

					// ??? 뭐여?? 애도 마찬가진데?
					UserManager::GetInstance().ForEach( nullptr );
				} );

	//	- 다행히 여러분들이 프로젝트에 배정받으면, 이러한 접근을 불가능하게 하는 기법들에 대하여 확인할 수 있으니, 크게 걱정하지 않으셔도 됩니다!
	//	- 미니게임하시면서, 여러분들만의 해결책을 고안해서 적용해도 재밌을거 같습니다!!
}

void Framework::방_생성_예제()
{
	//	방 생성과 관련된 예제입니다.
	//	 - 유저 생성 및 로그인과 큰 차이가 없지 않나..라고 생각하실 수 있습니다!
	//	 - 따라서 코드를 바로 작성하겠습니다. 아래의 코드를 한번 봐주세요!
	{
		// User를 가져오는 부분은 깊게 생각하지 말아주세요. IOCP구조에서는 Session에서 유저를 갖고 있으면 되는데, 여기는 어렵네요;
		UserUnit* userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // 유저생성_혹은_로그인_예제에서 만든 유저입니다.
		
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "시작 ---------- B. 방 생성 예제 " );
		PRINT_LOG_WITH_THREADID( "B-0" );

		const std::string roomName = "Room1";

		// 방 생성 테스크를 RoomManager에게 요청합니다.
		RoomManager::GetInstance().PushTask( 
			[ roomName, userUnit ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 2 ) );
				PRINT_LOG_WITH_THREADID( "B-1" );

				auto& roomCont = RoomManager::GetInstance().m_roomCont;

				// 예외 처리
				{
					if ( 
						const int currentRoomCount = roomCont.size()
						; currentRoomCount >= RoomManager::MAX_ROOM_NUM )
					{
						// 방 생성 실패! 이미 최대 방 수 입니다.
						PRINT_LOG_WITH_THREADID( "방 생성 실패! 이미 최대 방 개수! // userId : " + userUnit->m_id + ",현재 방 수 : " + std::to_string( currentRoomCount ) );
						return;
					}

					if (
						const auto nameCheckResult = roomCont.find( roomName )
						; nameCheckResult != roomCont.end() )
					{
						// 이미 생성되어있는 이름입니다!
						PRINT_LOG_WITH_THREADID( "방 생성 실패! 이미 만들어져있는 이름입니다. // userId : " + userUnit->m_id + ", name : " + roomName );
						return;
					}
				}

				// 예외를 모두 통과하였습니다! 방을 등록합니다.
				RoomUnit* newRoom = new RoomUnit( roomName );
				roomCont[ roomName ] = newRoom;
				
				// 등록된 방에 유저를 넣어주고, 유저에게도 방을 알려줍니다.
				newRoom->m_roomUserCont.insert( { userUnit->m_id, userUnit } );
				userUnit->m_roomPtr = newRoom;

				PRINT_LOG_WITH_THREADID( "방 생성 성공! // userId : " + userUnit->m_id + ", name : " + roomName );
				PRINT_LOG_WITH_THREADID( "B-2" );
			} );

		// 로비에 있는 유저들에게 방이 생성되었는지 알려줘야 하기 떄문에, 생성된 방 정보를 보냅니다.
		UserManager::GetInstance().PushTask(
			[ roomName ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "B-3" );

				UserManager::GetInstance().ForEach(
					[ roomName ]( UserUnit* user )
					{
						// 로비에 있는 유저들에게만 정보를 보내준다.
						if ( user->m_roomPtr )
							return;

						// 유저 여러분들~~ 방 새로 생겼어요~~~
						// user->Send( RoomCreate );
					} );
			} );

		PRINT_LOG_WITH_THREADID( "B-4" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "종료 ---------- B. 방 생성 예제 " );
	}
	
	//	 - 방 생성 코드를 잘 보셨나요? 어떠신가요? 쉽죠!!
	// 
	// 
	//	 - 하지만...ㅎ    위 코드는 여러가지 이유로 완벽하게 쓰레기입니다. 왜 그럴까요?
	//		0. 찍힌 로그의 순서를 잘 확인해봅시다. Task 순서가 이상합니다!
	//			- RoomManager의 방생성 Task가 실패하면, 실제 방생성이 실패하는데, UserManager Task는 RoomManager Task의 실패여부와 상관없이 실행됩니다.
	//			- 비동기 로직은 선후 로직 등을 잘 고려하여 작성해야합니다!
	//		1. RoomManager에서도 user->m_roomPtr를 참고하고 있고, UserManager에서도 user->m_roomPtr를 참고하고 있습니다! 무려 포인트 변수인데 말이죠!
	//			- m_roomPtr를 RoomManager에서만 확인하거나 또는 UserManager에서만 참고해야 합니다!
	//		2. 또 있나요...?
	
	// 그럼 위의 이슈들을 수정한 제대로된 예제를 확인해보실까요?
	{
		// 음 이건 위와 동일하게 신경쓰지 말아주세요! 예제입니다.
		UserUnit* const userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // 위에서 만든 유저입니다.
		{
			// 와우; 위의 예제내용을 먼저 초기화합니다.
			userUnit->m_roomPtr = nullptr;
			RoomUnit* roomUnit = RoomManager::GetInstance().m_roomCont[ "Room1" ]; // 위에서 만든 방입니다.
			delete roomUnit;
			RoomManager::GetInstance().m_roomCont.clear();
		}

		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "시작 ---------- C. 진짜!! 방 생성 예제 " );
		PRINT_LOG_WITH_THREADID( "C-0");

		const std::string roomName = "Room2";

		// 방 생성 테스크를 RoomManager에게 요청합니다.
		RoomManager::GetInstance().PushTask( 
			[ roomName, userUnit ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "C-1" );

				auto& roomCont = RoomManager::GetInstance().m_roomCont;

				// 예외
				{
					if ( 
						const int currentRoomCount = roomCont.size()
						; currentRoomCount >= RoomManager::MAX_ROOM_NUM )
					{
						// 방 생성 실패! 이미 최대 방 수 입니다.
						PRINT_LOG_WITH_THREADID( "방 생성 실패! 이미 최대 방 개수! // userId : " + userUnit->m_id + ",현재 방 수 : " + std::to_string( currentRoomCount ) );
						return;
					}

					if (
						const auto nameCheckResult = roomCont.find( roomName )
						; nameCheckResult != roomCont.end() )
					{
						// 이미 생성되어있는 이름입니다!
						PRINT_LOG_WITH_THREADID( "방 생성 실패! 이미 만들어져있는 이름입니다. // userId : " + userUnit->m_id + ", name : " + roomName );

						return;
					}
				}

				// 예외를 모두 통과하였습니다! 방을 등록합니다.
				RoomUnit* newRoom = new RoomUnit( roomName );
				roomCont[ roomName ] = newRoom;

				// 등록된 방에만 유저를 넣어줍니다.
				newRoom->m_roomUserCont.insert( { userUnit->m_id, userUnit } );

				// 이건 여기서 하면 안되요! m_roomPtr는 UserManager의 Thread Context에서 접근하도록 약속합시다.
				// userUnit->m_roomPtr = newRoom;

				PRINT_LOG_WITH_THREADID( "방 생성 성공! // userId : " + userUnit->m_id + ", name : " + roomName );
				PRINT_LOG_WITH_THREADID( "C-2" );

				// 정상적으로 방 생성에 성공했다면, 유저 매니저 테스크를 태웁니다!
				UserManager::GetInstance().PushTask(
					[ roomName, newRoom, userUnit ]()
					{
						std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
						PRINT_LOG_WITH_THREADID( "C-3" );

						// userUnit의 m_roomPtr는 UserManager에서만 접근합시다!
						userUnit->m_roomPtr = newRoom;

						UserManager::GetInstance().ForEach(
							[ roomName ]( UserUnit* user )
							{
								// 로비에 있는 유저들에게만 정보를 보내준다.
								// 이제는 m_roomPtr를 접근해도 됩니다!
								if ( user->m_roomPtr )
									return;
		
								// 유저 여러분들~~ 방 새로 생겼어요~~~
								// user->Send( RoomCreate );
							} );
					} );

				PRINT_LOG_WITH_THREADID( "C-4" );
			} );

		PRINT_LOG_WITH_THREADID( "C-5" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "종료 ---------- C. 진짜!! 방 생성 예제 " );
	}

	// 이렇게 방 생성 예제까지 마무리 되었습니다!

	// 물론 2번째 예제 코드도 완벽하지 않습니다.
	// 가장 큰 문제로는, UserPtr와 RoomPtr를 관리하는 Thread가 아닌 다른 Thread로 넘어 갔을 때, 해당 변수의 라이프를 보장하지 못한다는 이슈가 있습니다.
	//	0. 가장 대중적이고 안전한 방법은 C++ 표준의 shared_ptr를 사용하는 것으로, 람다 캡쳐 시, RefCount가 증가하면서 MultiThread에서도 Safe하게 처리할 수 있습니다.
	//	1. 다른 방법은 또 없을까요??

	// 여기에서 방 생성은 마치겠습니다.
}

void Framework::채팅_예제()
{
	//	채팅과 관련된 예제입니다. 크게 어렵지 않습니다. 코드로 확인하겠습니다.
	{
		// 역시나 User는 어거지로 가져옵니다.
		UserUnit* const userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // 위에서 만든 유저입니다.
		
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "시작 ---------- D. 채팅 예제 " );
		PRINT_LOG_WITH_THREADID( "D-0" );

		const ChatType    chatType    = ChatType::Room;
		const std::string chatMessage = "Hello World!";

		// 로비 채팅은 방에 없을 떄만, 방 채팅은 방에 있을 때만 가능합니다.
		// 따라서 먼저 우리는, 이 유저가 방에 접속했는지 여부를 확인합니다.
		// 아까 방에 접속했는지 여부를 UserManager에서 참조하기로 약속했습니다. 따라서 UserManager에 먼저 갑시다.
		UserManager::GetInstance().PushTask(
			[ userUnit, chatType, chatMessage ]()
			{
				PRINT_LOG_WITH_THREADID( "D-1" );

				const bool isInLobby = userUnit->m_roomPtr == nullptr;

				// 예외 처리
				{
					if ( 
						!isInLobby                 &&
						chatType == ChatType::Lobby )
					{
						PRINT_LOG_WITH_THREADID( "뭐여 로비에 없는데, 로비 채팅을 요청했습니다. // 유저 이름 : " + userUnit->m_id );
					}

					if ( 
						isInLobby                 &&
						chatType == ChatType::Room )
					{
						PRINT_LOG_WITH_THREADID( "뭐여 방에 없는데, 방 채팅을 요청했습니다. // 유저 이름 : " + userUnit->m_id );
					}
				}

				// 로비 채팅을 처리합니다.
				if ( chatType == ChatType::Lobby )
				{
					// 지금 여기는 UserManager Thread이기 때문에, 바로 User들에게 전송합니다.
					UserManager::GetInstance().ForEach(
						[ userUnit, chatMessage ]( UserUnit* user )
						{
							if ( user->m_roomPtr )
								return;

							// 채팅 받아랏
							// user->Send( userUnit, chatMessage );
						} );

					PRINT_LOG_WITH_THREADID( "로비 채팅 성공! // userId : " + userUnit->m_id + ", msg : " + chatMessage );
				}
				// 혹은 방 채팅을 처리합니다.
				else if ( chatType == ChatType::Room )
				{
					// 어떠한 방에 있는 유저 목록은, RoomManager Thread에서 처리하기 떄문에 Task를 넘겨줍니다.
					RoomManager::GetInstance().PushTask(
						[ 
							roomPtr = userUnit->m_roomPtr,
							userUnit, 
							chatMessage ]()
						{
							if ( !roomPtr )
								return;

							roomPtr->ForEach( 
								[ userUnit, chatMessage ]( UserUnit* user )
								{
									// 채팅 받아랏
									// user->Send( userUnit, chatMessage );
								} );

							PRINT_LOG_WITH_THREADID( "방 채팅 성공! // userId : " + userUnit->m_id + ", msg : " + chatMessage );
						} );
				}
			}
		);

		PRINT_LOG_WITH_THREADID( "D-2" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 3 ) );

		PRINT_LOG( "종료 ---------- D. 채팅 예제" );
	}

	PRINT_LOG( "" );
	PRINT_LOG( "" );

	// 위 예제 채팅 기능은 무난합니다!...ㅎ 맞겠죠...?
}