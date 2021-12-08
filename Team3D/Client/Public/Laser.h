#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CLaser abstract : public CGameObject
{
protected:
	explicit CLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaser(const CLaser& rhs);
	virtual	~CLaser() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

protected:
	CTransform*							m_pTransformCom = nullptr;
	CTextures*							m_pTextureCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	_float4								m_vCollisionPoint = _float4(0.f, 0.f, 0.f, 0.f);
	VTXMATRIX_CUSTOM_ST*				m_pInstanceMatrices = nullptr;
	VTXMATRIX_CUSTOM_ST*				m_pCircleMatrice = nullptr;
	CVIBuffer_PointInstance_Custom_ST*	m_pVIBufferCom = nullptr;


public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() override;
};

END