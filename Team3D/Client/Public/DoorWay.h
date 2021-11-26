#pragma once

#ifndef __DOORWAY_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
END

BEGIN(Client)
class CDoorWay final : public CGameObject
{
protected:
	explicit CDoorWay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDoorWay(const CDoorWay& rhs);
	virtual ~CDoorWay() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:


protected:
	/* For.Component */
	CRenderer*			m_pRendererCom	= nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom		= nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

public:
	static CDoorWay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __DOORWAY_H__
#endif // !__DOORWAY_H__
