#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPressurePlateLock : public CGameObject
{
protected:
	explicit CPressurePlateLock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPressurePlateLock(const CPressurePlateLock& rhs);
	virtual ~CPressurePlateLock() = default;

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
	void Set_LockActive(_bool IsCheck);

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool				m_IsLockActive = false;
	_bool				m_IsLockDeActive = false;
	_uint				m_iOption = 0;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;

private:
	void  LockActive(_double TimeDelta);

public:
	static CPressurePlateLock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END