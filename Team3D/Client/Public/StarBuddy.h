#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
END

BEGIN(Client)

class CStarBuddy : public CGameObject
{
protected:
	explicit CStarBuddy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStarBuddy(const CStarBuddy& rhs);
	virtual ~CStarBuddy() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render() override;
	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	void Set_Launch(_bool bLaunch) { m_bLaunch = bLaunch; }
	void Launch_StarBuddy(_double dTimeDelta);

private:
	_float		m_fUpAcceleration = 0.f;
	_bool		m_bLaunch = false;
	_float		m_fLifeTime = 0.f;
	_bool		m_IsCollide = false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*	m_pTriggerCom = nullptr;

private:
	HRESULT InterActive_UI();

public:
	static CStarBuddy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END