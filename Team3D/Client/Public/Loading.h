#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

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
	HRESULT LoadingForWT(_uint ilThreadIndex); /* 테스트용 */

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