#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel_Instance;
END

BEGIN(Client)

class CTileBox final : public CGameObject
{
private:
	explicit CTileBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTileBox(const CTileBox& rhs);
	virtual ~CTileBox() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CModel_Instance*	m_pModelCom = nullptr;

public:
	static CTileBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END