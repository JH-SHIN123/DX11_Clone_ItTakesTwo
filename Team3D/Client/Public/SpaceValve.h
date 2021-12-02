#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
class CTriggerActor;
END

BEGIN(Client)

class CSpaceValve final : public CGameObject
{
private:
	explicit CSpaceValve(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpaceValve(const CSpaceValve& rhs);
	virtual ~CSpaceValve() = default;

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

private:
	void Rotate_SpaceValve(_double dTimeDelta);

private:
	_uint		m_iTargetPlayer = 0;
	_bool		m_IsCollide = false;
	_bool		m_bEnterValve = false;
	_bool		m_bRotate = false;
	_float		m_fRotateDelay = 0.f;
	_uint		m_iRotateCount = 0;

private:
	class CSpace_Valve_Star* m_pSpaceValve_Star = nullptr; //베이스, 유리창 포함

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

public:
	static CSpaceValve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END