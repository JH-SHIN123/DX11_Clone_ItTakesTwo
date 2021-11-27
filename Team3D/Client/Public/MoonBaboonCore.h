#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CMoonBaboonCore final : public CGameObject
{
private:
	explicit CMoonBaboonCore(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore(const CMoonBaboonCore& rhs);
	virtual ~CMoonBaboonCore() = default;

public:
	void Set_Broken() { m_bBroken = true; }
	void Set_ActiveCore(_int iActive) { m_iActiveCore = iActive; }

public:
	CTransform* Get_Transform() const { return m_pTransformCom; }
	_int		Get_ActiveCore() const { return m_iActiveCore; }
	_bool		Get_bArrived() const { return m_bArrived; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;

private:
	void Active_Pillar(_double TimeDelta);

private:
	CTransform* m_pTransformCom = nullptr;

private:
	_bool	m_bBroken = false;
	_bool	m_bArrived = false;
	_int	m_iActiveCore = 0;

private:
	_float	m_fMoveDelta = 0.f;

private:
	_bool	m_bBrokenStart = false;
	_double	m_dBrokenWaitingDeltaT = 0.f;

private:
	class CMoonBaboonCore_Pillar*	m_pCorePillar = nullptr;	// 장식
	class CMoonBaboonCore_Button*	m_pCoreButton = nullptr;	// 밟으면 올라감
	class CMoonBaboonCore_Shield*	m_pCoreShield = nullptr;	// shield / pillar shield : 코어필러가 다올라오면 실드가 내려감
	class CMoonBaboonCore_Glass*	m_pCoreGlass = nullptr;		// 데미지 달면, 부서지고, 코어 비활성화 (다내려가고, 버튼쪽 문닫힘)
	// Core

public:
	static CMoonBaboonCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END