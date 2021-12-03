#pragma once

#include "Client_Defines.h"
#include "GameOption.h"

BEGIN(Client)

#ifdef __THREADCHECK
#define __threadbreak __debugbreak();
#else
#define __threadbreak
#endif

/**
* 프로토타입 추가할 때는 자기 이름 함수에 추가해주세요.
* 다른 위치에 만들어야하는 경우에는 따로 알려주세요.
* 자기 이름 함수 이외의 부분은 건드리지 마세요.
*/
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
	HRESULT LoadingForLogo(_uint iThreadIndex);
	HRESULT LoadingForStage(_uint iThreadIndex);
private:
	/* 각자 GameObject 프로토타입 생성 함수 */
	HRESULT Create_GameObjects_SpaceStage_Se();
	HRESULT Create_GameObjects_SpaceStage_Jung();
	HRESULT Create_GameObjects_SpaceStage_Hye();
	HRESULT Create_GameObjects_SpaceStage_Taek();
	HRESULT Create_GameObjects_SpaceStage_Yoon();
	HRESULT Create_GameObjects_SpaceStage_Jin();
	HRESULT Create_GameObjects_SpaceStage_Jun();

public:
	static CLoading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Level::ID ePreLevelID, Level::ID eNextLevelID);
	virtual void Free() override;
};

END