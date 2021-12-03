#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CSavePoint final : public CGameObject
{
public:/* Enum */
	enum SAVEPOINTSHAPE { BOX, SPHERE, SHAPE_END };

public:/* Struct */
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

public:
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	CTransform*		m_pTransformCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

	_float3			m_vSavePosition = {};

private:
	HRESULT Ready_Component(void* pArg);

public:
	static	CSavePoint*	  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void		  Free() override;
};
END
