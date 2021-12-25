#pragma once
#include "GameObject.h"

BEGIN(Client)
class C2DMesh final : public CGameObject
{
private:
	explicit C2DMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit C2DMesh(const C2DMesh& rhs);
	virtual ~C2DMesh() = default;

public:
	void Start();

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup);

	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	CModel*				m_pModelCom_UFO = nullptr;
	CModel*				m_pModelCom_Ailen = nullptr;
	CModel*				m_pModelCom_Robot = nullptr;
	CModel*				m_pModelCom_Star = nullptr;
	CModel*				m_pModelCom_Umbrella = nullptr;

	CDynamicActor*		m_pDynamicActorCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;

	CTransform*			m_pCodyTransformCom = nullptr;

	/* 초기 생성스케일, 최대 스케일 */
	_float				m_fScale = 0.5f;
	_float				m_fMaxScale = 1.f;

	_float				m_fRandomAngle = 0.f;
	_double				m_dCoolTime = 0.0;
	_uint				m_iColorIndex = 0;
	_bool				m_bCollision = false;
	_bool				m_bStart = false;

	_uint				m_iRandomModel = 0;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static C2DMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END