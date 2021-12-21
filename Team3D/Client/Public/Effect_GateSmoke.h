#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CEffect_GateSmoke final : public CGameObject
{
private:
	explicit CEffect_GateSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_GateSmoke(const CEffect_GateSmoke& rhs);
	virtual ~CEffect_GateSmoke() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	/* For.Component */
	CTransform*								m_pTransformCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CTextures*								m_pDiffuseTextureCom = nullptr;
	class CVIBuffer_SimplePointInstance*	m_pVIBufferCom = nullptr;
	_float4x4*								m_pVertexLocals = nullptr;
	_float*									m_pSize = nullptr;
	_float*									m_pAlphaWeight = nullptr;
	_uint									m_iInstanceCount = 0;

public:
	static CEffect_GateSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END