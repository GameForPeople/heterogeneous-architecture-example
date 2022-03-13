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
	Lobby,   // �κ� ä��
	Room,    // �� ä��
};

void Framework::Run()
{
	/*
		0. ���� ���α׷��� 1���� ������ ä�� ������ �������ֽø� �� �� �����ϴ�.

		1. �� ���������� �� 3���� �����尡 �����մϴ�.
		 - ���� ������        : Framework::Run()�� ȣ���� main������ ( IOCP�϶��� �����ϸ�, Recv�� �̷������ I/O �������� �����ص� ���� �� �����ϴ�! )
		 - UserManager ������ : UserManager�� Task�� ������ ������, ������ �κ�� ���õ� ������ ����մϴ�.
		 - RoomManager ������ : RoomManager�� Task�� ������ ������, �� ������ ���õ� ������ ����մϴ�.

		2. �� ������ �ٽ��� TaskManager Ŭ��������, PushTask �Լ��� ����, Task�� �񵿱������� �ٸ� Context�� �ѱ�� ���Դϴ�.
			- ������ Context���� Target Context�� ���� PushTask�� Task�� �ѱ� ��,
				- Target��ü�� MS-Safe Queue(PPL)�� Push�ؼ�, MP�� �����ϰ� ó���ϰ� ����ȭ�ϰ�,
				- ����, Target��ü�� �ڽ��� ��ȿ�� Context���� Pop�����μ�, SC�� �����ϰ� ó���մϴ�.
			- �̷��� TaskManager�� �������� ��ġ�Ͽ� ���� ��Ű��ó�� ���������μ�, ���� Context�� ó���� �� �ֽ��ϴ�.
			- �� ���������� lambda�� ���� �ٸ� Context�� �ѱ�� �ֽ��ϴ�.
				- �� TaskUnit Class�� ���� �����, Command ����ó�� ó���� ���� �ְ�, �ٸ� ���� ��ĵ鵵 �����մϴٸ� �� ���������� lambda�� ����ϰ� �ֽ��ϴ�.
			- �۷θ����δ� ��Ȯ�ϰ� ���ذ� ����� �� �ֽ��ϴ�. �Ʒ� ������ �� ���� Ȯ���Ͻô� ���� ��õ�帳�ϴ�.

		3. �� ���� ���������� �ڵ忡 �ͼ��� �ϼż�, �� �ڵ��� ������ �޼��ϱ� ����, ���Ƿ� �������������� �ۼ��Ͽ����ϴ�.
			�Ʒ��� �׸��� �и��� ���� �ڵ尡 �ƴմϴ�. �����Ź�帳�ϴ�.
			- �ϳ��� �Լ��� ������������ ������� ��.
			- �� ��ü�� ����� public���� �����ϴ� ��.
			- �ߺ� ������ �Լ�ȭ ���� �ʴ� ��.

		4. �� 3���� ����� ������ �����Ͽ����ϴ�.
			0. �������� Ȥ�� �α��� ����
			1. ����� ����
			2. ä�� ����
	*/

	Log::AddThreadIdAndName( "MainThread" );
	PRINT_LOG_WITH_THREADID( "Run Main Thread!" );
	const auto userThreadStopAutoCall = UserManager::GetInstance().Run();
	const auto roomThreadStopAutoCall = RoomManager::GetInstance().Run();

	std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );

	// ������
	{
		��������_Ȥ��_�α���_����();
		��_����_����();
		ä��_����();
	}

#pragma region ������_��ġ��
	/*
		�̷��� ����ִ԰� ����ȫ���� 1���� ������ ���ֽ� ä�� ������ ��� ��ɵ���!
		���Ͻ����� ��Ŀ��� ���, �����ϰԳ��� heterogeneous ��Ű��ó�� �����غ��ҽ��ϴ�.

		���� �б� �ٴҶ��� ������ �����Բ� homogeneous��ĸ� ����ٰ�, 
		���� heterogeneous����� �����ϸ鼭, ���� �ڵ带 �ۼ��ϴ� �������� ���� �������� ���뽺������ ����� ���ϴ�.

		��� �� ������ ������ heterogeneous ����� ������ �����帰 ���� �ܿ���, ���� ������ ���� �ֽ��ϴ�.
			- �� Manager ���� Thread�� �����ϰ� �־�, Context���� ���� �л��� ���� �ʴ� �̽�
			- �������� Room���� �ϳ��� RoomManager Thread�� ���� ���ư��� ����
			- �ϳ��� Task�� ó���ϴ� �������� Atomic ������ ����ϰ� ���( concurrent_queue�� push-try_pop)�Ǵ� ������
			- ��Ÿ ���� ������.

		heterogeneous�� �����غ��ø鼭, ���� ���� �������� �ذ��ϱ����� ���� ����� �ϰ� �ǽǰŰ�, �ذ��� ���� ���� �Ű�, ��� �ذ��Ͻ��� �����ŵ� �˴ϴ�.
		�ٸ� �׷��� ������ ����, �� �پ��ϰ� ���� �ð��� ���� �ǽǰŰ�, ���� ���α׷��ӷμ� ���������� ������ �� �ִ� ���� ��ȸ�� �ǽ� ���̶�� �����մϴ�.

		�����Ͻø鼭 �𸣽ô� ��, �ñ��� ��, ���ǰ� �ʿ��� ��, �������� ���߿��� ���� ���� ���� ��󳾶��� ���� ��, 
		�ʿ��Ͻø� �������� ������Ź�帳�ϴ�.
		
		������ 2���� �ݸ���׿� ���� ���� �Ⱓ, ���� ����ð����� ȭ���� �ϼ����� ���ڽ��ϴ�....��
		�� ���� �����ΰ� �˾Ҵµ� �̹��� ���丵�ϸ鼭 �����µ� �ʹ� ����׿�....������ �䵵 ���ٴµ�....�������� �˼��մϴ�...��

		ȭ����!! :)
	*/
#pragma endregion
}

void Framework::��������_Ȥ��_�α���_����()
{
	//	���� ���� ���� Ȥ��, �α��ΰ� ���õ� �����Դϴ�.
	//	 - ������ �����̴ٺ���, ���� DB�� ������ �ʾҽ��ϴ�. ���� ������ �α����� ���ÿ� �̷�����ٰ� �����մϴ�.

	//	 - ���� ���� �� �α����� ���� ������ ������ �ʿ��մϴ�.
	//		0. �ִ� �������� �Ѿ����� ���θ� Ȯ���մϴ�. 
	//		1. ��û���� Id�� ������ �ִ� ���̵����� Ȯ���մϴ�.
	//		2. ���� ��ü�� �����Ͽ�, ���� �����̳ʿ� ����մϴ�.

	//	- �� �������� Ȯ���ؾ��� ������ ������ �����ϴ�.
	//		- ���� �α��� �� ��ü ���� �����̳�
	//			- �츮�� �� ������, Id�� Ű�� ������, UserUnit�� Data�� ������ Map���� ó���� �̴ϴ�.
				UserManager::GetInstance().m_userCont; // <- �ٷ� �� ģ���Դϴ�!
	//	- �ٸ�, ���⼭ ������ �ֽ��ϴ�. Map�� Multithread�� Safe���� ���� ���������̱� ������, ���� �����忡���� ������ ��쿡�� ������ �����Դϴ�.
	//	- �׷��� ������, �츮�� �ƹ� �����忡�� ó������ �ʰ�, User���õ� ������! UserManager���� ó���ϵ��� ����սô�. 

	//	- UserManager���� ���� ���� Ȥ�� �α��ΰ� ���õ� �ڵ�� ������ �����ϴ�.
	{
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );

		PRINT_LOG( "���� ---------- A. ���� ���� Ȥ�� �α��� ���� " );
		PRINT_LOG_WITH_THREADID( "A-0" );

		const std::string id = "NetmarbleNeo";
		UserManager::GetInstance().PushTask(
			// �׽�ũ�� ���ٷ� �ѱ�ϴ�! ������ �������� C++ Ȥ�� STL�� �Ƹ� ���ٿ� ���Ͽ� ������ �̴ϴ�!
			[ id ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "A-1" );

				auto& userCont = UserManager::GetInstance().m_userCont;

				// ���� ó��
				{
					if ( 
						const int currentUserCount = userCont.size()
						; currentUserCount >= UserManager::MAX_USER_NUM )
					{
						// �α��� ����! �̹� �ִ� ���� �� �Դϴ�.
						PRINT_LOG_WITH_THREADID( "�α��� ����! �̹� �ִ� ����! // ���� ���� �� : " + std::to_string( currentUserCount ) );
						return;
					}

					if (
						const auto idCheckResult = userCont.find( id )
						; idCheckResult != userCont.end() )
					{
						// �̹� �������ִ� Id�Դϴ�!
						PRINT_LOG_WITH_THREADID( "�α��� ����! �̹� ������ ID // id : " + id );
						return;
					}
				}

				// ���ܸ� ��� ����Ͽ����ϴ�! ������ ����մϴ�.
				userCont[ id ] = new UserUnit( id );
				PRINT_LOG_WITH_THREADID( "�α��� ����! // id : " + id );

				UserManager::GetInstance().ForEach( 
					[]( UserUnit* user )
					{
						// �� �κе� ���ٷ� �ۼ��Ǿ� �־� �����Ҽ� �ִµ�, ForEach�� PushTask�� �ٸ��� �����Լ��Դϴ�.
						PRINT_LOG_WITH_THREADID( "A-2" );

						// ���� �����е�~~ �� �α��� �߾��~~
						// user->Send( UserLogin );
					} );
			} );

		PRINT_LOG_WITH_THREADID( "A-3" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "���� ---------- A. ���� ���� Ȥ�� �α��� ���� " );
	}

	// �� �������� �߿��ϰ� ������ ������� �ֽ��ϴ�.
	
	//	0. ����, A-0, A-1, A-2 �α׸� Ȯ���غ�����!
	//		- �αװ� ���� ������ �Ƹ�, A-0 -> A-3 -> A-1 -> A-2�� ���Դϴ�.
	//		- ���⼭ �츮��, ��! A-0, A-3�� A-1, A-2�� �񵿱������� �̷�����±��� ��� ����� �� �� �ֽ��ϴ�.
	//		- ContextName - ThreadID�� Ȯ���غ��� �� ��Ȯ�ϰ� ������ �� �ֽ��ϴ�.

	//	1. �������δ�, � ���� Ȥ�� �Լ��� � �����忡�� �����ؾ��ϴ� ���� �����ϴ� ��ġ�� ���� ���� ������ �׻� �����ؾ��Ѵٴ� ���Դϴ�.
	//		- ���� ��� �Ʒ��� �ڵ�� ������ �����Դϴ�.
			
			// RoomManager Task�� �����մϴ�.
			RoomManager::GetInstance().PushTask( []() 
				{ 
					// ? ���� �ִ� �����Ŵ���Task������ �����ؾ��ϴ� �����ε�?
					auto& userCont = UserManager::GetInstance().m_userCont;

					// ??? ����?? �ֵ� ����������?
					UserManager::GetInstance().ForEach( nullptr );
				} );

	//	- ������ �����е��� ������Ʈ�� ����������, �̷��� ������ �Ұ����ϰ� �ϴ� ����鿡 ���Ͽ� Ȯ���� �� ������, ũ�� �������� �����ŵ� �˴ϴ�!
	//	- �̴ϰ����Ͻø鼭, �����е鸸�� �ذ�å�� ����ؼ� �����ص� ������� �����ϴ�!!
}

void Framework::��_����_����()
{
	//	�� ������ ���õ� �����Դϴ�.
	//	 - ���� ���� �� �α��ΰ� ū ���̰� ���� �ʳ�..��� �����Ͻ� �� �ֽ��ϴ�!
	//	 - ���� �ڵ带 �ٷ� �ۼ��ϰڽ��ϴ�. �Ʒ��� �ڵ带 �ѹ� ���ּ���!
	{
		// User�� �������� �κ��� ��� �������� �����ּ���. IOCP���������� Session���� ������ ���� ������ �Ǵµ�, ����� ��Ƴ׿�;
		UserUnit* userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // ��������_Ȥ��_�α���_�������� ���� �����Դϴ�.
		
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "���� ---------- B. �� ���� ���� " );
		PRINT_LOG_WITH_THREADID( "B-0" );

		const std::string roomName = "Room1";

		// �� ���� �׽�ũ�� RoomManager���� ��û�մϴ�.
		RoomManager::GetInstance().PushTask( 
			[ roomName, userUnit ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 2 ) );
				PRINT_LOG_WITH_THREADID( "B-1" );

				auto& roomCont = RoomManager::GetInstance().m_roomCont;

				// ���� ó��
				{
					if ( 
						const int currentRoomCount = roomCont.size()
						; currentRoomCount >= RoomManager::MAX_ROOM_NUM )
					{
						// �� ���� ����! �̹� �ִ� �� �� �Դϴ�.
						PRINT_LOG_WITH_THREADID( "�� ���� ����! �̹� �ִ� �� ����! // userId : " + userUnit->m_id + ",���� �� �� : " + std::to_string( currentRoomCount ) );
						return;
					}

					if (
						const auto nameCheckResult = roomCont.find( roomName )
						; nameCheckResult != roomCont.end() )
					{
						// �̹� �����Ǿ��ִ� �̸��Դϴ�!
						PRINT_LOG_WITH_THREADID( "�� ���� ����! �̹� ��������ִ� �̸��Դϴ�. // userId : " + userUnit->m_id + ", name : " + roomName );
						return;
					}
				}

				// ���ܸ� ��� ����Ͽ����ϴ�! ���� ����մϴ�.
				RoomUnit* newRoom = new RoomUnit( roomName );
				roomCont[ roomName ] = newRoom;
				
				// ��ϵ� �濡 ������ �־��ְ�, �������Ե� ���� �˷��ݴϴ�.
				newRoom->m_roomUserCont.insert( { userUnit->m_id, userUnit } );
				userUnit->m_roomPtr = newRoom;

				PRINT_LOG_WITH_THREADID( "�� ���� ����! // userId : " + userUnit->m_id + ", name : " + roomName );
				PRINT_LOG_WITH_THREADID( "B-2" );
			} );

		// �κ� �ִ� �����鿡�� ���� �����Ǿ����� �˷���� �ϱ� ������, ������ �� ������ �����ϴ�.
		UserManager::GetInstance().PushTask(
			[ roomName ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "B-3" );

				UserManager::GetInstance().ForEach(
					[ roomName ]( UserUnit* user )
					{
						// �κ� �ִ� �����鿡�Ը� ������ �����ش�.
						if ( user->m_roomPtr )
							return;

						// ���� �����е�~~ �� ���� ������~~~
						// user->Send( RoomCreate );
					} );
			} );

		PRINT_LOG_WITH_THREADID( "B-4" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "���� ---------- B. �� ���� ���� " );
	}
	
	//	 - �� ���� �ڵ带 �� ���̳���? ��Ű���? ����!!
	// 
	// 
	//	 - ������...��    �� �ڵ�� �������� ������ �Ϻ��ϰ� �������Դϴ�. �� �׷����?
	//		0. ���� �α��� ������ �� Ȯ���غ��ô�. Task ������ �̻��մϴ�!
	//			- RoomManager�� ����� Task�� �����ϸ�, ���� ������� �����ϴµ�, UserManager Task�� RoomManager Task�� ���п��ο� ������� ����˴ϴ�.
	//			- �񵿱� ������ ���� ���� ���� �� ����Ͽ� �ۼ��ؾ��մϴ�!
	//		1. RoomManager������ user->m_roomPtr�� �����ϰ� �ְ�, UserManager������ user->m_roomPtr�� �����ϰ� �ֽ��ϴ�! ���� ����Ʈ �����ε� ������!
	//			- m_roomPtr�� RoomManager������ Ȯ���ϰų� �Ǵ� UserManager������ �����ؾ� �մϴ�!
	//		2. �� �ֳ���...?
	
	// �׷� ���� �̽����� ������ ����ε� ������ Ȯ���غ��Ǳ��?
	{
		// �� �̰� ���� �����ϰ� �Ű澲�� �����ּ���! �����Դϴ�.
		UserUnit* const userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // ������ ���� �����Դϴ�.
		{
			// �Ϳ�; ���� ���������� ���� �ʱ�ȭ�մϴ�.
			userUnit->m_roomPtr = nullptr;
			RoomUnit* roomUnit = RoomManager::GetInstance().m_roomCont[ "Room1" ]; // ������ ���� ���Դϴ�.
			delete roomUnit;
			RoomManager::GetInstance().m_roomCont.clear();
		}

		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "���� ---------- C. ��¥!! �� ���� ���� " );
		PRINT_LOG_WITH_THREADID( "C-0");

		const std::string roomName = "Room2";

		// �� ���� �׽�ũ�� RoomManager���� ��û�մϴ�.
		RoomManager::GetInstance().PushTask( 
			[ roomName, userUnit ]()
			{
				std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
				PRINT_LOG_WITH_THREADID( "C-1" );

				auto& roomCont = RoomManager::GetInstance().m_roomCont;

				// ����
				{
					if ( 
						const int currentRoomCount = roomCont.size()
						; currentRoomCount >= RoomManager::MAX_ROOM_NUM )
					{
						// �� ���� ����! �̹� �ִ� �� �� �Դϴ�.
						PRINT_LOG_WITH_THREADID( "�� ���� ����! �̹� �ִ� �� ����! // userId : " + userUnit->m_id + ",���� �� �� : " + std::to_string( currentRoomCount ) );
						return;
					}

					if (
						const auto nameCheckResult = roomCont.find( roomName )
						; nameCheckResult != roomCont.end() )
					{
						// �̹� �����Ǿ��ִ� �̸��Դϴ�!
						PRINT_LOG_WITH_THREADID( "�� ���� ����! �̹� ��������ִ� �̸��Դϴ�. // userId : " + userUnit->m_id + ", name : " + roomName );

						return;
					}
				}

				// ���ܸ� ��� ����Ͽ����ϴ�! ���� ����մϴ�.
				RoomUnit* newRoom = new RoomUnit( roomName );
				roomCont[ roomName ] = newRoom;

				// ��ϵ� �濡�� ������ �־��ݴϴ�.
				newRoom->m_roomUserCont.insert( { userUnit->m_id, userUnit } );

				// �̰� ���⼭ �ϸ� �ȵǿ�! m_roomPtr�� UserManager�� Thread Context���� �����ϵ��� ����սô�.
				// userUnit->m_roomPtr = newRoom;

				PRINT_LOG_WITH_THREADID( "�� ���� ����! // userId : " + userUnit->m_id + ", name : " + roomName );
				PRINT_LOG_WITH_THREADID( "C-2" );

				// ���������� �� ������ �����ߴٸ�, ���� �Ŵ��� �׽�ũ�� �¿�ϴ�!
				UserManager::GetInstance().PushTask(
					[ roomName, newRoom, userUnit ]()
					{
						std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 1 ) );
						PRINT_LOG_WITH_THREADID( "C-3" );

						// userUnit�� m_roomPtr�� UserManager������ �����սô�!
						userUnit->m_roomPtr = newRoom;

						UserManager::GetInstance().ForEach(
							[ roomName ]( UserUnit* user )
							{
								// �κ� �ִ� �����鿡�Ը� ������ �����ش�.
								// ������ m_roomPtr�� �����ص� �˴ϴ�!
								if ( user->m_roomPtr )
									return;
		
								// ���� �����е�~~ �� ���� ������~~~
								// user->Send( RoomCreate );
							} );
					} );

				PRINT_LOG_WITH_THREADID( "C-4" );
			} );

		PRINT_LOG_WITH_THREADID( "C-5" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 5 ) );

		PRINT_LOG( "���� ---------- C. ��¥!! �� ���� ���� " );
	}

	// �̷��� �� ���� �������� ������ �Ǿ����ϴ�!

	// ���� 2��° ���� �ڵ嵵 �Ϻ����� �ʽ��ϴ�.
	// ���� ū �����δ�, UserPtr�� RoomPtr�� �����ϴ� Thread�� �ƴ� �ٸ� Thread�� �Ѿ� ���� ��, �ش� ������ �������� �������� ���Ѵٴ� �̽��� �ֽ��ϴ�.
	//	0. ���� �������̰� ������ ����� C++ ǥ���� shared_ptr�� ����ϴ� ������, ���� ĸ�� ��, RefCount�� �����ϸ鼭 MultiThread������ Safe�ϰ� ó���� �� �ֽ��ϴ�.
	//	1. �ٸ� ����� �� �������??

	// ���⿡�� �� ������ ��ġ�ڽ��ϴ�.
}

void Framework::ä��_����()
{
	//	ä�ð� ���õ� �����Դϴ�. ũ�� ����� �ʽ��ϴ�. �ڵ�� Ȯ���ϰڽ��ϴ�.
	{
		// ���ó� User�� ������� �����ɴϴ�.
		UserUnit* const userUnit = UserManager::GetInstance().m_userCont[ "NetmarbleNeo" ]; // ������ ���� �����Դϴ�.
		
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "" );
		PRINT_LOG( "���� ---------- D. ä�� ���� " );
		PRINT_LOG_WITH_THREADID( "D-0" );

		const ChatType    chatType    = ChatType::Room;
		const std::string chatMessage = "Hello World!";

		// �κ� ä���� �濡 ���� ����, �� ä���� �濡 ���� ���� �����մϴ�.
		// ���� ���� �츮��, �� ������ �濡 �����ߴ��� ���θ� Ȯ���մϴ�.
		// �Ʊ� �濡 �����ߴ��� ���θ� UserManager���� �����ϱ�� ����߽��ϴ�. ���� UserManager�� ���� ���ô�.
		UserManager::GetInstance().PushTask(
			[ userUnit, chatType, chatMessage ]()
			{
				PRINT_LOG_WITH_THREADID( "D-1" );

				const bool isInLobby = userUnit->m_roomPtr == nullptr;

				// ���� ó��
				{
					if ( 
						!isInLobby                 &&
						chatType == ChatType::Lobby )
					{
						PRINT_LOG_WITH_THREADID( "���� �κ� ���µ�, �κ� ä���� ��û�߽��ϴ�. // ���� �̸� : " + userUnit->m_id );
					}

					if ( 
						isInLobby                 &&
						chatType == ChatType::Room )
					{
						PRINT_LOG_WITH_THREADID( "���� �濡 ���µ�, �� ä���� ��û�߽��ϴ�. // ���� �̸� : " + userUnit->m_id );
					}
				}

				// �κ� ä���� ó���մϴ�.
				if ( chatType == ChatType::Lobby )
				{
					// ���� ����� UserManager Thread�̱� ������, �ٷ� User�鿡�� �����մϴ�.
					UserManager::GetInstance().ForEach(
						[ userUnit, chatMessage ]( UserUnit* user )
						{
							if ( user->m_roomPtr )
								return;

							// ä�� �޾ƶ�
							// user->Send( userUnit, chatMessage );
						} );

					PRINT_LOG_WITH_THREADID( "�κ� ä�� ����! // userId : " + userUnit->m_id + ", msg : " + chatMessage );
				}
				// Ȥ�� �� ä���� ó���մϴ�.
				else if ( chatType == ChatType::Room )
				{
					// ��� �濡 �ִ� ���� �����, RoomManager Thread���� ó���ϱ� ������ Task�� �Ѱ��ݴϴ�.
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
									// ä�� �޾ƶ�
									// user->Send( userUnit, chatMessage );
								} );

							PRINT_LOG_WITH_THREADID( "�� ä�� ����! // userId : " + userUnit->m_id + ", msg : " + chatMessage );
						} );
				}
			}
		);

		PRINT_LOG_WITH_THREADID( "D-2" );
		std::this_thread::sleep_for( static_cast< std::chrono::seconds >( 3 ) );

		PRINT_LOG( "���� ---------- D. ä�� ����" );
	}

	PRINT_LOG( "" );
	PRINT_LOG( "" );

	// �� ���� ä�� ����� �����մϴ�!...�� �°���...?
}