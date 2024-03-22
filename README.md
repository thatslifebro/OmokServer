# Omok Server

## 서버 구성

### 1. 네트워크 라이브러리 (외부 라이브러리)
Poco - ParallelSocketAcceptor 사용

클라이언트 접속 처리는 싱글 스레드. 그 이후의 이벤트들을 멀티스레드로 처리. 

### 2. 데이터 직렬화 (외부 라이브러리)
Google Protocol Buffer (protobuf)

### 3. 패킷 구성
packet_size(2byte) + packet id(2byte) + body(protobuf)

### 4. 쓰레드 구성
- main thread (1) : Init, 스레드 생성, Run
- Poco ParallellSocketAcceptor (n) : 멀티스레드로 네트워크 이벤트 처리 및 PacketQueue에 패킷 저장
- PacketProcessor (1) : PacketQueue에 저장되는 패킷을 처리, 타이머 처리, DBPacketQueue에 패킷 저장, 응답 Packet 전송
- DBProcessor (1) : PacketProcessor가 저장한 패킷을 처리, 응답 Packet 전송


### 5. 기능 구현
- 로그인
- 방 입장, 나가기, 채팅
- 방장의 매칭 요청, 수락 및 거절
- 게임 준비
- 오목 두기(룰 체크)
- 관전
- 타임아웃 (매칭 요청 응답(15초), 준비완료(20초), 돌 두기(30초))
- 결과 전송

-------

## 주요 클래스

### Server
서버 init, start

### Session 
- client의 접속시 생성되는 객체
- connect & disconnect 시 PacketQueue에 내부 패킷 저장 (PacketProcessor에서 처리)
- client가 보낸 데이터를 읽어 PacketQueue에 통신 패킷 저장
- clinet에 데이터 전송.

### PacketProcessor
- PacketQueue에 저장된 패킷을 순서대로 처리.
- 로직 처리 후 응답 패킷 전송.
- 1초마다 증가되는 time_count_를 통해 타임아웃 처리.
- DB를 거쳐야 한다면 DBPacketQueue에 저장.

### DBProcessor
- DBPacketQueue에 저장된 패킷을 순서대로 처리.
- DB 처리 후 응답 패킷 전송.
- 현재는 DB연결 없음. 로그인 확인만 처리중.

### SessionManager
- Session 객체를 관리.

### RoomManager
- Room 객체를 관리.
- 서버 시작시 Room의 개수를 받아 그 개수만큼 Room 객체 미리 생성.

### Room
- unordered_set(session_id 보관), Timer 객체, Game 객체 포함.
- 유저 관리, 타임아웃 처리, 게임 진행.

### Timer
- Room 객체에 포함되어 있음
- PacketProcessor에서 모든 방의 타이머를 체크해 callback 함수를 실행.
- 1초마다 증가되는 time_count_를 통해 Timer에 callback 함수와 실행될 time_count_를 저장. 

### Game
- 오목 게임 진행
- 현재 게임 상태 및 룰 체크

### Packet
- 헤더 정보와 protobuf 데이터를 포함하는 클래스
- 데이터 직렬화 및 역직렬화 (헤더 : big-endian, protobuf : little-endian)

----

## 클라이언트

C# winform으로 만들어진 클라이언트

https://github.com/jacking75/omok_clients

CSharp_OmokClient_2를 기반.

protobuf 이용해서 패킷 직렬화 및 역직렬화.