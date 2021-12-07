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

class CHookUFO final : public CGameObject
{
private:
	explicit CHookUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHookUFO(const CHookUFO& rhs);
	virtual ~CHookUFO() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

public: /* Setter */
	void Set_Launch(_bool bLaunch) { m_bLaunch = bLaunch; }

private:
	HRESULT InterActive_UI();
	void Launch_HookUFO(_double dTimeDelta);

private:
	_float		m_fUpAcceleration = 0.f;
	_bool		m_bLaunch = false;
	_float		m_fLifeTime = 0.f;
	_bool		m_IsCollide = false;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CTransform*			m_pPhysxTransform = nullptr;

public:
	static CHookUFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END