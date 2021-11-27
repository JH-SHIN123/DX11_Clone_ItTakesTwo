#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPressurePlateFrame : public CGameObject
{
protected:
	explicit CPressurePlateFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPressurePlateFrame(const CPressurePlateFrame& rhs);
	virtual ~CPressurePlateFrame() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	void Set_Position(_vector vPosition);

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	void Set_BatteryCharged(_bool _bBatteryCharged) { m_bBatteryCharged = _bBatteryCharged; }

private:
	_bool		m_bRotate = false;
	_bool		m_IsCollide = false;
	_bool		m_bBatteryCharged = false;
	_bool		m_bUpdate = true;
	_float		m_fStopDelay = 0.f;

	_bool		m_bNoBatteryHit = false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

public:
	static CPressurePlateFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END