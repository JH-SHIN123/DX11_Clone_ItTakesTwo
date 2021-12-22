#pragma once
#include "GameObject.h"

BEGIN(Client)
class CRock final : public CGameObject
{
private:
	explicit CRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRock(const CRock& rhs);
	virtual ~CRock() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	virtual HRESULT Render_ShadowDepth() override;
	virtual void Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CDynamicActor*		m_pDynamicActorCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;

	CTransform*			m_pCodyTransformCom = nullptr;

	_float				m_fScale = 0.3f;
	_float				m_fMaxScale = 1.2f;
	_bool				m_bCollision = false;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END