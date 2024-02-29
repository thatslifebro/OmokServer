# OmokServer


### 작업내용

클라이언트는 현재 테스트 클라이언트입니다.

#### 서버

Poco 라이브러리에 parellel reactor 이용해서 네트워크 처리

protocol buffer 이용해서 데이터 주고 받아 보았고

패킷 body를 protobuf로 만들고 패킷 헤더(size, packet id) 를 달아 테스트 클라이언트와 통신 구현

#### 작업 예정

실제 오목 클라이언트 통신부에 protobuf 및 헤더 적용해서 서버와 통신

이후 게임 로직 구현 ( 룸, 채팅, 게임로직 등)

--------------------------

### 파일

#### PacketBufferManager
Write 함수  : socket에서 받은 데이터를 버퍼에 저장한다. (_writePos 변수를 통해 쓸 위치 기억)

Read 함수 : 버퍼에서 헤더 부분만 읽어서 패킷 사이즈를 알아내고, 패킷 사이즈만큼 읽어서 패킷을 만든다. (_readPos 변수를 통해 읽을 위치 기억)

#### PacketManager
<패킷ID, 패킷핸들러> key-value로하는 map을 생성하고, 패킷 핸들러들을 저장하는 클래스이다.

OmokServer.cpp(main)에 있는 PacketManage함수에서 객체가 단 한번 생성된다.

PacketManager 함수는 하나의 쓰레드로 동작하며, PacketBufferManager의 Read를 반복적으로 수행하고, 패킷을 읽어 처리까지 한번에 한다.