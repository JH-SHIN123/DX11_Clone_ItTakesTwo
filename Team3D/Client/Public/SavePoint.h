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
class CSavePoint final : public CGameObject
{
public:
	enum SAVEPOINTSHAPE { BOX, SPHERE, SHAPE_END };

public:
	typedef struct tagArgumentDesc
	{
		SAVEPOINTSHAPE	eShape;
		_float3			vPosition;
		_float3			vRotation;
		_float3			vScale;
		_float3			vSavePosition;
	}ARG_DESC;

private:
	explicit CSavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSavePoint(const CSavePoint& rhs);
	virtual ~CSavePoint() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);

private:
	CTransform*		m_pTransformCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_float3			m_vSavePosition = _float3(0.f, 0.f, 0.f);

public:
	static CSavePoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};
END
