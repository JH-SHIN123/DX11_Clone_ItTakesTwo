#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CEarth final : public CGameObject
{
private:
	explicit CEarth(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEarth(const CEarth& rhs);
	virtual	~CEarth() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CTransform*	m_pTransformCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	CTextures*	m_pTexturesCom = nullptr;

private:
	_double m_dTimeUV = 0.0;

public:
	static CEarth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END