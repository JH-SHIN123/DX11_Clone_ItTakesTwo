#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
class CModel_Instance;
class CTriggerActor;
END

BEGIN(Client)
class CDeadLine final : public CGameObject
{
public:
	typedef struct tagArgumentDesc
	{
		_float3		vPosition;
		_float3		vRotation;
		_float3		vScale;
	}ARG_DESC;

private:
	explicit CDeadLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDeadLine(const CDeadLine& rhs);
	virtual ~CDeadLine() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);

public:
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	CTransform*		m_pTransformCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

public:
	static CDeadLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};
END
