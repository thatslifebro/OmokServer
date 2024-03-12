# OmokServer


#### 서버

Poco 라이브러리에 parellel reactor 이용해서 네트워크 처리

protocol buffer 이용해서 데이터 주고 받아 보았고

패킷 body를 protobuf로 만들고 패킷 헤더(size, packet id) 를 달아 테스트 클라이언트와 통신 구현

기능 구현

- 로그인
- 방 입장, 나가기, 채팅
- 매칭
- 게임 준비
- 오목 두기
- 연결 끊김 처리 및 게임 종료 처리
- 게임 룰 서버에 구현

### 파일

db_processor : 데이터베이스에 접근하는 클래스 (실제 db사용은 안하고 있고, 로그인 확인만 처리중)

game_room_manager, game_room : 게임 방 및 게임 방 관리

omok_server : 서버 init, start 정의

packet_id : 패킷 ID 정의

packet_info : 패킷 및 헤더 정의

packet_processor : packet_queue에서 패킷 가져와 처리하는 클래스

packet_queue : 각 세션에서 온 패킷을 저장하는 곳.

packet_sender : 서버에서 보낼 패킷을 만들고 보내는 곳.

room, room_manager : 방과 방 관리

session.h, session_manager.h : 세션 정의와 세션 관리 클래스

user_info : db 대신 임시 사용중