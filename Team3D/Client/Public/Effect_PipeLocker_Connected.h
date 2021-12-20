#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CEffect_PipeLocker_Connected final : public CGameObject
{
private:
	explicit CEffect_PipeLocker_Connected(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_PipeLocker_Connected(const CEffect_PipeLocker_Connected& rhs);
	virtual ~CEffect_PipeLocker_Connected() = default;

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
	CTextures*								m_pMaskTextureCom = nullptr;
	class CVIBuffer_SimplePointInstance*	m_pVIBufferCom = nullptr;
	_float4x4*								m_pVertexLocals = nullptr;
	_float3*								m_pSize = nullptr;
	_uint									m_iInstanceCount = 0;
	_float									m_fGrvityWeight = 0.f;
	_float									m_fShootingPower = 0.f;

public:
	static CEffect_PipeLocker_Connected* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END