#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CPlanet final : public CGameObject
{
private:
	explicit CPlanet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlanet(const CPlanet& rhs);
	virtual ~CPlanet() = default;

public:
	HRESULT NativeConstruct_Prototype();
	HRESULT NativeConstruct(void* pArg);
	_int	Tick(_double dTimeDelta);
	_int	Late_Tick(_double dTimeDelta);
	HRESULT Render();

private:
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;

private:
	HRESULT Ready_Component(void* pArg);
public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	static CPlanet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;;
	virtual void Free() override;
};
END
