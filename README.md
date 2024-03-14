# OmokServer


#### 서버

Poco 라이브러리에 parellel reactor 이용해서 네트워크 처리

protocol buffer 이용해서 데이터 주고 받아 보았고

패킷 body를 protobuf로 만들고 패킷 헤더(size, packet id) 를 달아 테스트 클라이언트와 통신

기능 구현

- 로그인
- 방 입장, 나가기, 채팅
- 매칭
- 게임 준비
- 오목 두기
- 연결 끊김 처리 및 게임 종료 처리
- 게임 룰 서버에 구현
- 타이머
- 관전 기능

방에서 게임은 하나만 이루어짐.

방장이 방에 있는 사람중 한명에게 매칭 요청을 보내 상대방이 수락하면 매칭.(15초 timeout)

준비완료 버튼을 둘다 누르면 게임 시작. (20초 timeout)

바둑돌을 두지 않으면 상대에게 턴이 넘어감 (30초 timeout)

게임을 하지 않는 인원은 관전자가 됨.

### 파일

db_processor : 데이터베이스에 접근하는 클래스 (실제 db사용은 안하고 있고, 로그인 확인만 처리중)

game : 게임 진행

omok_server : 서버 init, start 정의

packet_id : 패킷 ID 정의

packet_info : 패킷 및 헤더 정의

packet_processor : packet_queue에서 패킷 가져와 처리하는 클래스

packet_queue : 각 세션에서 온 패킷을 저장하는 곳. / db 처리를 위한 패킷 큐도 따로 만들어서 처리중

packet_sender : 서버에서 보낼 패킷을 만들고 보내는 곳.

room, room_manager : 방과 방 관리

session.h, session_manager.h : 세션 정의와 세션 관리 클래스

timer : 방마다 있는 타이머

timeout_processor : 모든 방의 타이머를 보고 시간이 되면 callback을 처리해줌.