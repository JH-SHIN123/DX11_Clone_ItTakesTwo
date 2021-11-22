#pragma once

#ifndef __INGAMEEFFECT_MODEL_H__
#include "InGameEffect.h"

BEGIN(Engine)
class CModel_Instance;
END

BEGIN(Client)
class CInGameEffect_Model abstract : public CGameObject
{
protected:
	explicit CInGameEffect_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInGameEffect_Model(const CInGameEffect_Model& rhs);
	virtual ~CInGameEffect_Model() = default; 

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void SetUp_WorldMatrix(_fmatrix WorldMatrix) PURE;

public:
	HRESULT Ready_Component(void* pArg);
	HRESULT	Ready_InstanceBuffer(_bool IsRenderTerm = false);

public:
	_float4 Set_UV(_int iIndex);

protected:
	// 프로토타입에 기본 세팅을 하고 클론에서 상황에 맞는 추가적인 데이터를 넘기자
	EFFECT_DESC_PROTO m_EffectDesc_Prototype;			// 프로토 타입이 가져야 할 데이터
	EFFECT_DESC_CLONE m_EffectDesc_Clone;				// 클론 생성시 추가적인 세팅이 필요 할 때

protected:
	_float4x4* m_pInstanceBuffer = nullptr;

protected:
	CRenderer*			m_pRendererCom		= nullptr;
	CTransform*			m_pTransformCom		= nullptr;
	CTextures*			m_pTexturesCom		= nullptr;
	CModel_Instance*	m_pModelInstanceCom = nullptr;


public:
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __INGAMEEFFECT_MODEL_H__
#endif
