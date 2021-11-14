#pragma once

#include "InGameEffect.h"

BEGIN(Engine)
class CVIBuffer_RectInstance_Custom;
END

BEGIN(Client)
class CInGameEffect_Rect abstract : public CGameObject
{
protected:
	explicit CInGameEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInGameEffect_Rect(const CInGameEffect_Rect& rhs);
	virtual ~CInGameEffect_Rect() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	HRESULT SetUp_InstanceCount(_uint iInstanceCount);
	HRESULT SetUp_Instance_WorldMatrix(_uint iIndex, _float4x4 WolrdMatrix);
	HRESULT SetUp_Instance_WorldMatrix(_uint iIndex, _fmatrix WolrdMatrix);
	HRESULT SetUp_Instance_Position(_uint iIndex, _fvector vPosition, _fvector vOffSetPosition = XMVectorZero());


public: // 기본적인 기능
	void	Check_Color(_double TimeDelta);
	void	Check_Gravity(_double TimeDelta);
	_float2 Check_Size(_double TimeDelta, _int iIndex = 0);
	_float4 Check_Move(_double TimeDelta, _int iIndex = 0);


public:
	HRESULT Ready_Component(void* pArg);
	HRESULT	Ready_InstanceBuffer(_bool IsRenderTerm = false);

	void SetUp_Shader_Data();

protected:
	EFFECT_DESC_PROTO m_EffectDesc_Prototype;
	EFFECT_DESC_CLONE m_EffectDesc_Clone;	

protected:
	_int			m_iInstance_RenderCount = 1;

	VTXMATRIX*		m_pInstanceBuffer			= nullptr;
	_float3*		m_pInstance_Dir				= nullptr;
	_double			m_UVTime = 0.0;

protected:
	CTransform*						m_pTransformCom			= nullptr;
	CRenderer*						m_pRendererCom			= nullptr;
	CTextures*						m_pTexturesCom			= nullptr;
	CTextures*						m_pTexturesCom_Second	= nullptr;
	CTextures*						m_pTexturesCom_Color	= nullptr;
	CVIBuffer_RectInstance_Custom*	m_pRectInstanceCom		= nullptr;

public:
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override; private:
};
END
