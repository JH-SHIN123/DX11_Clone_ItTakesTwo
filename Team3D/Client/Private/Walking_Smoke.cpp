#include "stdafx.h"
#include "..\Public\Walking_Smoke.h"
#include "GameInstance.h"

CWalking_Smoke::CWalking_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CWalking_Smoke::CWalking_Smoke(const CWalking_Smoke & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CWalking_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);
	
	return S_OK;
}

HRESULT CWalking_Smoke::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 3;
	m_EffectDesc_Prototype.vSize = { 0.1f, 0.1f, 0.f };
	m_EffectDesc_Prototype.fLifeTime = 2.f;

	__super::NativeConstruct(pArg);



	return S_OK;
}

_int CWalking_Smoke::Tick(_double TimeDelta)
{
	m_EffectDesc_Prototype.fLifeTime -= TimeDelta;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Size((_float)TimeDelta, iIndex);
		m_pInstanceBuffer[iIndex].vTextureUV = Check_UV(TimeDelta, iIndex);
	}

	return _int();
}

_int CWalking_Smoke::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CWalking_Smoke::Render()
{
	SetUp_Shader_Data();

	_float4 vColor = { 0.5f,0.5f ,0.5f ,0.5f };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));

	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CWalking_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x += TimeDelta * m_EffectDesc_Clone.fSizePower;
	m_pInstanceBuffer[iIndex].vSize.y += TimeDelta * m_EffectDesc_Clone.fSizePower;

	if (m_EffectDesc_Clone.vSize_Max.x <= m_pInstanceBuffer[iIndex].vSize.x)
		m_pInstanceBuffer[iIndex].vSize.x = m_EffectDesc_Clone.vSize_Max.x;

	if (m_EffectDesc_Clone.vSize_Max.y <= m_pInstanceBuffer[iIndex].vSize.y)
		m_pInstanceBuffer[iIndex].vSize.y = m_EffectDesc_Clone.vSize_Max.y;

	return;
}

void CWalking_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

CWalking_Smoke * CWalking_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWalking_Smoke*	pInstance = new CWalking_Smoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CWalking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWalking_Smoke::Clone_GameObject(void * pArg)
{
	CWalking_Smoke*	pInstance = new CWalking_Smoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CWalking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWalking_Smoke::Free()
{
	__super::Free();
}
