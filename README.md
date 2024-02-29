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