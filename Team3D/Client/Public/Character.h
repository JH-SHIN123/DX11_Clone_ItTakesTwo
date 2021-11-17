#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCamera;
class CControllableActor;
END

BEGIN(Client)
class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

protected:
	_float Compute_Distance(CTransform* pPlayerTransform, CTransform* pDstTransform);
	_float Compute_Degree(CTransform* pPlayerTransform, CTransform* pDstTransform);
	_float Compute_Degree_On_Land(CTransform* pPlayerTransform, CTransform* pDstTransform);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;
	virtual HRESULT Render_ShadowDepth() override;


protected: /* For.Component */
	CRenderer*	m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	CControllableActor* m_pActorCom = nullptr;

protected:
	_float m_fClockWise = 1.f; // 1이면 시계방향, -1이면 반시계방향.
	//_uint  m_iViewPortNum = 0;

public:
	virtual void Free() override;
};
END