#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "LaserTennis_Manager.h"

BEGIN(Client)
class CLaser_LaserTennis final : public CGameObject
{
public:
	typedef struct tagArgument
	{
		_float3						 vPosition;
		_float3						 vDirection;
		CLaserTennis_Manager::TARGET eTarget;
	}ARG_DESC;

private:
	explicit CLaser_LaserTennis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaser_LaserTennis(const CLaser_LaserTennis& rhs);
	virtual ~CLaser_LaserTennis() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup);

public:
	virtual HRESULT Render_ShadowDepth();
	virtual void Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_float			m_fDistance = 20.f;
	_float			m_fCheckDistance = 0.f;
	_float			m_fScale = 1.f;
	_float			m_fMaxScale = 23.f;

	_float3						 m_vDirection = {};
	CLaserTennis_Manager::TARGET m_eTarget;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaser_LaserTennis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END