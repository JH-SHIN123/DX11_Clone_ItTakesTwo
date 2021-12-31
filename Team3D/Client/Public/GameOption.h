#pragma once
 
/************************************************************************/

//#define __FULLSCREEN
//#define __START_LEVEL_LOGO
#define __PLAY_CUTSCENE
#define __FIRST_CUTSCENE_OFF

/************************************************************************/

/**
* 보스방 무적
*/
#define __PLAYER_INVINCIBLE_BOSSROOM
 
/************************************************************************/
  
/**
* 스레드 종료 시간 체크용
* 스레드마다 종료되는 시간 체크하려면 주석 해제.
*/
//#define __THREADCHECK
 
#ifdef __THREADCHECK
#define __threadbreak __debugbreak();
#else
#define __threadbreak
#endif^
  
/************************************************************************/

/**
* 사용할 스레드 
*/
#ifdef _DEBUG
#define __THREAD_NUM 8
#else
#define __THREAD_NUM 4
#endif
  
/************************************************************************/

/**
* 맵 로딩 하려면 주석 처리.
*/
//#define __MAPLOADING_OFF
 
/**
* 우주선 내부 맵만 제거
* 프레임 살리기용
*/
//#define __SPACESHIP_OFF

/************************************************************************/

/**
* 메이 키보드로 컨트롤하려면 주석 해제.
*/
#define __CONTROL_MAY_KEYBOARD

/************************************************************************/

/**
* 각자 테스트용 디파인
*/
//#define __TEST_SE
//#define __TEST_JUNG
//#define __TEST_HYE
//#define __TEST_TAEK
//#define __TEST_YOON
//#define __TEST_JIN
//#define __TEST_JUN

/************************************************************************/

/**
* 디버깅창 생성 여부
* 디폴트 디버깅창 = 오브젝트 정보창
*/
//#define __GAME_DEBUGGER

/**
* 패드 입력 디버깅
*/
//#define __DEBUGGING_PAD

/************************************************************************/

/**
* 뷰 행렬 생성방법
* 주석 해제 > 카메라 DESC 기준
* 주석 설정 > 카메라 Transform 기준
*/
//#define MAKE_VIEWMATRIX_BY_CAMERADESC
