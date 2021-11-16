#include "stdafx.h"
#include "..\Public\Effect_Walking_Smoke.h"
#include "GameInstance.h"

CEffect_Walking_Smoke::CEffect_Walking_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Walking_Smoke::CEffect_Walking_Smoke(const CEffect_Walking_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Walking_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);
	
	m_EffectDesc_Prototype.iInstanceCount = 3;
	m_EffectDesc_Prototype.vSize = { 1.f, 1.f, 0.f };
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.fDirMoveSpeed = 0.2f;

	return S_OK;
}

HRESULT CEffect_Walking_Smoke::NativeConstruct(void * pArg)
{

	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CEffect_Walking_Smoke::Tick(_double TimeDelta)
{
	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	Control_Alpha(TimeDelta);

	Control_Instance((_float)TimeDelta);

	return _int();
}

_int CEffect_Walking_Smoke::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Walking_Smoke::Render()
{
	SetUp_Shader_Data();

	_float4 vColor = { 0.3f, 0.3f, 0.3f ,(_float)m_dAlphaTime };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Walking_Smoke::Control_Alpha(_double TimeDelta)
{
	if (false == m_IsDisapear)
		m_dAlphaTime += TimeDelta;
	else
		m_dAlphaTime -= TimeDelta;

	if (false == m_IsDisapear && 0.5 <= m_dAlphaTime)
	{
		m_dAlphaTime = 0.5;
		m_IsDisapear = true;
	}
}

void CEffect_Walking_Smoke::Control_Instance(_float TimeDelta)
{
	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
		m_pInstanceBuffer[iIndex].vTextureUV = Check_UV(TimeDelta, iIndex);
	}
}

void CEffect_Walking_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x += TimeDelta * m_EffectDesc_Clone.fSizePower;
	m_pInstanceBuffer[iIndex].vSize.y += TimeDelta * m_EffectDesc_Clone.fSizePower;

	if (m_EffectDesc_Clone.vSize_Max.x <= m_pInstanceBuffer[iIndex].vSize.x)
		m_pInstanceBuffer[iIndex].vSize.x = m_EffectDesc_Clone.vSize_Max.x;

	if (m_EffectDesc_Clone.vSize_Max.y <= m_pInstanceBuffer[iIndex].vSize.y)
		m_pInstanceBuffer[iIndex].vSize.y = m_EffectDesc_Clone.vSize_Max.y;

	return;
}

void CEffect_Walking_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

	vPos += vDir * TimeDelta * m_EffectDesc_Prototype.fDirMoveSpeed;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Walking_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_Walking_Smoke * CEffect_Walking_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Walking_Smoke*	pInstance = new CEffect_Walking_Smoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Walking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Walking_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_Walking_Smoke*	pInstance = new CEffect_Walking_Smoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Walking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Walking_Smoke::Free()
{
	__super::Free();
}
