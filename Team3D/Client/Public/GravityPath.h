#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CGravityPath final : public CGameObject
{
private:
	explicit CGravityPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGravityPath(const CGravityPath& rhs);
	virtual	~CGravityPath() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CStaticActor*	m_pStaticActorCom = nullptr;

public:
	virtual void Free() override;
};

END