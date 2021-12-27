#include "stdafx.h"
#include "..\Public\Effect_Boss_Core.h"
#include "Effect_Generator.h"

CEffect_Boss_Core::CEffect_Boss_Core(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Core::CEffect_Boss_Core(const CEffect_Boss_Core & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Core::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_Core::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Flow_01"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Color"), (CComponent**)&m_pTextureCom_Color), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_Instance();

	return S_OK;
}

_int CEffect_Boss_Core::Tick(_double TimeDelta)
{
	if (true == m_IsHit)//
		return Explosion();

	if (m_isDead) return EVENT_DEAD;

	if (nullptr == m_pPointInstanceCom || nullptr == m_pInstanceBuffer)
		return EVENT_DEAD;


	// 문제없음
	m_fDistortion_Time += (_float)TimeDelta * 0.5f;
	if (1.f < m_fDistortion_Time)
		m_fDistortion_Time = 0.f;

	m_fDegree_Angle += (_float)TimeDelta * 10.f;
	if (360.f <= m_fDegree_Angle)
		m_fDegree_Angle = 0.f;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition, vPos);

	m_fTextureUV_Time -= (_float)TimeDelta * 0.75f;
	if (0.f >= m_fTextureUV_Time)
	{
		m_fTextureUV_Time = 0.01f;
		m_pInstanceBuffer[0].vTextureUV = Check_UV();
	}

	return _int();
}

_int CEffect_Boss_Core::Late_Tick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Boss_Core::Render(RENDER_GROUP::Enum eGroup)
{
	if (nullptr == m_pPointInstanceCom || nullptr == m_pInstanceBuffer)
		return S_OK;

	_float fRandian = XMConvertToRadians(m_fDegree_Angle);
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	_float4 vColorRamp_UV = { 0.f, 0.f, 1.f, 1.f };
	_int i = 1;
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstanceCom->Set_Variable("g_fRadianAngle", &fRandian, sizeof(_float));
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vColorRamp_UV, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fDistortion_Time, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTextureCom_Color->Get_ShaderResourceView(2));

	m_pPointInstanceCom->Render(14, m_pInstanceBuffer, 2); /////

	return S_OK;
}

void CEffect_Boss_Core::Set_Pos(_fvector vPos)
{
	_vector vOffSetPos = XMLoadFloat3(&m_vOffSet_Pos) + vPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOffSetPos);
}

void CEffect_Boss_Core::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core::Instance_UV(_float TimeDelta, _int iIndex)
{
}

_int CEffect_Boss_Core::Explosion()
{
	m_isDead = true;
	EFFECT->Add_Effect(Effect_Value::BossCore_Hit, m_pTransformCom->Get_WorldMatrix());
	return EVENT_DEAD;
}

HRESULT CEffect_Boss_Core::Ready_Instance()
{
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[2];

	m_pInstanceBuffer[0].vRight	= { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer[0].vUp	= { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer[0].vLook	= { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer[0].vSize	= { 2.5f, 2.75f };
	m_pInstanceBuffer[0].vTextureUV = { 0.f, 0.f, 1.f, 1.f };
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition, vPos);

	m_pInstanceBuffer[1].vRight = { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer[1].vUp = { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer[1].vLook = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer[1].vSize = { 0.f, 0.f };
	m_pInstanceBuffer[1].vTextureUV = { 0.f, 0.f, 1.f, 1.f };
	m_pInstanceBuffer[1].vPosition = m_pInstanceBuffer[0].vPosition;

	return S_OK;
}

_float4 CEffect_Boss_Core::Check_UV()
{
	_float4 vUV;

	if (m_vTexUV.y == m_vTexUV_Max.y - 1 && m_vTexUV.x == m_vTexUV_Max.x - 1)
	{
		m_vTexUV.x = 0;
		m_vTexUV.y = 0;
	}
	else if (m_vTexUV.x == m_vTexUV_Max.x - 1)
	{
		m_vTexUV.x = 0;
		++m_vTexUV.y;
	}
	else
		++m_vTexUV.x;

	_float fLeft	= (1.f / m_vTexUV_Max.x) *  m_vTexUV.x;
	_float fTop		= (1.f / m_vTexUV_Max.y) *  m_vTexUV.y;
	_float fRight	= (1.f / m_vTexUV_Max.x) * (m_vTexUV.x + 1);
	_float fBottom	= (1.f / m_vTexUV_Max.y) * (m_vTexUV.y + 1);

	vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

CEffect_Boss_Core * CEffect_Boss_Core::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Core*	pInstance = new CEffect_Boss_Core(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Core");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Core::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Core* pInstance = new CEffect_Boss_Core(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Core");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Core::Free()
{
	Safe_Release(m_pTextureCom_Color);

	__super::Free();
}
