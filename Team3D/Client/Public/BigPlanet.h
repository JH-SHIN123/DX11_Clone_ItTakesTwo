#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
class CStaticActor;
END

BEGIN(Client)

class CBigPlanet : public CGameObject
{
protected:
	explicit CBigPlanet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBigPlanet(const CBigPlanet& rhs);
	virtual ~CBigPlanet() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;


	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }


public:
	void Set_Launch(_bool bLaunch) { m_bLaunch = bLaunch; }
	void Pendulum(_double dTimeDelta);


private:
	_bool		m_bLaunch = false;
	_bool		m_IsCollide = false;
	_float		m_fDelay = 0.f;

	_float3		m_vRotateDir = {};
	_float		m_fRotateTime = 0.f;
	_float		m_fSpeed = 5.f;



protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

	CTransform*			m_pPhysxTransformCom = nullptr;

private:
	HRESULT InterActive_UI();

public:
	static CBigPlanet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END