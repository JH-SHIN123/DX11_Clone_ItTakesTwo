#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CMoonBaboonCore final : public CGameObject
{
public:
	enum WHOISON { ON_CODY, ON_MAY, ON_NONE };

public:
	typedef struct tagMoonBaboonCoreDesc
	{
		_float4x4	WorldMatrix = MH_XMFloat4x4Identity();
		_uint		iIndex = 0;
	}MOONBABOONCORE_DESC;

private:
	explicit CMoonBaboonCore(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore(const CMoonBaboonCore& rhs);
	virtual ~CMoonBaboonCore() = default;

public:
	_bool Get_IsGoUp() { return m_IsGoUp; }

public:
	void Set_Broken();
	void Set_ActiveCore(_int iActive, WHOISON eOn);
	void Set_MoonBaboonCoreUp(_float fMaxDistance, _float fSpeed);

public:
	CTransform* Get_Transform() const { return m_pTransformCom; }
	_int		Get_ActiveCore() const { return m_iActiveCore; }
	_bool		Get_bArrived() const { return m_bArrived; }
	_bool		Get_Broken() const { return m_bBroken; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;

private:
	//void Reset(); // Dont Use
	void GoUp(_double dTimeDelta);
	void Active_Pillar(_double TimeDelta);
	void OnPatternCheck();

private:
	MOONBABOONCORE_DESC m_tDesc;

private:
	CTransform*		m_pTransformCom = nullptr;

private:
	static	_uint	m_iBrokenCheck;
	_bool			m_bPatternOn = false;
	WHOISON			m_eWhoIsOn = WHOISON::ON_NONE;

private:
	_bool	m_b2Floor = false;
	_bool	m_bResetOnce = false;
	_bool	m_bMove = false;
	_bool	m_bBroken = false;
	_bool	m_bArrived = false;
	_bool	m_IsGoUp = false;
	_int	m_iActiveCore = 0;

private:
	_float3 m_vMaxPos;
	_float	m_fMaxY = 0.f;
	_float	m_fUpSpeed = 0.f;
	_float	m_fDistance = 0.f;
	_float  m_fCoreSoundVolume = 1.f;

private:
	_float	m_fMoveTime = 1.4f;
	_float	m_fMoveDelta = 0.f;

private:
	_bool	m_bBrokenStart = false;
	_double	m_dBrokenWaitingDeltaT = 0.f;

private:
	class CMoonBaboonCore_Pillar*	m_pCorePillar = nullptr;	// 장식
	class CMoonBaboonCore_Button*	m_pCoreButton = nullptr;	// 밟으면 올라감
	class CMoonBaboonCore_Shield*	m_pCoreShield = nullptr;	// shield / pillar shield : 코어필러가 다올라오면 실드가 내려감
	class CMoonBaboonCore_Glass*	m_pCoreGlass = nullptr;		// 데미지 달면, 부서지고, 코어 비활성화 (다내려가고, 버튼쪽 문닫힘)
	class CEffect_Boss_Core*		m_pEffectBossCore = nullptr;

public:
	static CMoonBaboonCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END