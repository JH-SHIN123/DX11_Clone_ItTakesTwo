#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CHangingPlanet final : public CDynamic_Env
{
public:
	typedef struct tagArumentDesc
	{
		CDynamic_Env::ARG_DESC	DynamicDesc;
		_float3					vJointPosition;
		_float3					vOffset;
	}ARG_DESC;

private:
	explicit CHangingPlanet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHangingPlanet(const CHangingPlanet& rhs);
	virtual ~CHangingPlanet() = default;

public:
	void Add_Force(_fvector vForce);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CDynamicActor*			m_pDynamicActorCom = nullptr;
	CTriggerActor*			m_pTriggerActorCom = nullptr;
	PxSphericalJoint*		m_pJoint = nullptr;

	_bool					m_bTrigger = false;
	_bool					m_bCollider = false;

public:
	static CHangingPlanet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
