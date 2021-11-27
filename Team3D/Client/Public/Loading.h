#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

/* 스레드 종료 시간 체크용 */
//#define __THREADCHECK
/** 
* 각자 CPU 상태보다 높은 상태는 주석처리
* 로딩에 코드 추가할 때 16/8/6 스레드별 함수 모두 세팅할 것.
*/
#define __6THREADS
// #define __8THREADS
// #define __16THREADS

#ifdef __THREADCHECK
#define __threadbreak __debugbreak();
#else
#define __threadbreak
#endif

class CLoading final : public CBase
{
public:
	explicit CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoading() = default;

public: /* Struct */
	typedef struct tagThreadArg
	{
		CLoading*	pLoading;
		_uint		iThreadIndex;
	}THREAD_ARG;

public: /* Getter */
	CRITICAL_SECTION*	Get_CriticalSectionPtr(_uint iThreadIndex);
	const Level::ID		Get_PreLevelID() const { return m_ePreLevelID; }
	const Level::ID		Get_NextLevelID() const { return m_eNextLevelID; }
	const _bool			Is_FinishedToLoading() const;

public:
	HRESULT	NativeConstruct(Level::ID ePreLevelID, Level::ID eNextLevelID);
	HRESULT	Loading(Level::ID ePreLevelID, Level::ID eNextLevelID, _uint iThreadIndex);

private:
	CGameInstance*			m_pGameInstance		= nullptr;
	ID3D11Device*			m_pDevice			= nullptr;
	ID3D11DeviceContext*	m_pDeviceContext	= nullptr;
	Level::ID				m_ePreLevelID		= Level::LEVEL_END;
	Level::ID				m_eNextLevelID		= Level::LEVEL_END;
	/* For.Thread */
	_uint					m_iThreadCount		= 0;
	HANDLE*					m_arrThreads;
	CRITICAL_SECTION*		m_arrCriticalSections;
	THREAD_ARG*				m_arrThreadArgs;
	_bool*					m_arrFinished;
private:
	HRESULT LoadingForStage(_uint iThreadIndex); /* 예시, 테스트용 Level */

private:
	/* 각자 GameObject 프로토타입 생성 함수 */
	HRESULT Create_GameObjects_SpaceStage_Se();
	HRESULT Create_GameObjects_SpaceStage_Jung();
	HRESULT Create_GameObjects_SpaceStage_Hye();
	HRESULT Create_GameObjects_SpaceStage_Won();
	HRESULT Create_GameObjects_SpaceStage_Yoon();
	HRESULT Create_GameObjects_SpaceStage_Jin();
	HRESULT Create_GameObjects_SpaceStage_Jun();

public:
	static CLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID);
	virtual void Free() override;
};

END

/*
	// 추가 방법
	"LoadingFor_레벨이름"으로 함수 정의.
	인자로 스레드 인덱스를 받고, 인덱스에 따라 처리할 내용들을 정의한다.
	인덱스 범위는 0 ~ m_iThreadCount - 1

	특정 스레드가 먼저 종료되어야 하는 경우에는 WaitForSingleObject(m_arrThreads[iIndex], INFINITE); 추가
*/