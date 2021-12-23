#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "LaserTennis_Manager.h"

BEGIN(Client)
class CLaserTrigger final : public CGameObject
{
public:
	typedef struct tagArgument
	{
		_float3							vPosition;
		_float3							vTargetPosition;
		CLaserTennis_Manager::TARGET	eTarget;
		_uint							iActivationIndex;
	}ARG_DESC;

private:
	explicit CLaserTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaserTrigger(const CLaserTrigger& rhs);
	virtual ~CLaserTrigger() = default;

public:
	CLaserTennis_Manager::TARGET Get_Target() { return m_eTarget; }

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup);

public:
	virtual HRESULT Render_ShadowDepth();

private:
	CTransform*		 m_pTransformCom = nullptr;
	CRenderer*		 m_pRendererCom = nullptr;
	CModel*			 m_pModelCom = nullptr;

	_float			 m_fEndY = 0.f;
	_float			 m_fJumpY = 0.f;
	_float			 m_fJumpTime = 0.f;
	_float			 m_fJumpPower = 12.f;
	_float			 m_fJumpSpeed = 3.f;
	_float			 m_fMaxY = 0.f;

	_bool			 m_bJump = false;
	_bool			 m_bJumpEnd = false;
	_bool			 m_bBounce = false;

	_uint			 m_iActivationIndex = 0;

	_float3			 m_vTargetPos = {};

	CLaserTennis_Manager::TARGET m_eTarget = CLaserTennis_Manager::TARGET::TARGET_END;

	class CEffect_StarBuddy_Move* m_pMoveEffect = nullptr;

private:
	void Movement(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CLaserTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END