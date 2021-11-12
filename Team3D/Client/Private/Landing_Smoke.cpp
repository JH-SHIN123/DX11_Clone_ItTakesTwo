#include "stdafx.h"
#include "..\Public\Landing_Smoke.h"
#include "GameInstance.h"

CLanding_Smoke::CLanding_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CLanding_Smoke::CLanding_Smoke(const CLanding_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CLanding_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.fLifeTime = 2.f;

	return S_OK;
}

HRESULT CLanding_Smoke::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CLanding_Smoke::Tick(_double TimeDelta)
{
	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	Control_Alpha(TimeDelta);

	Control_Instance((_float)TimeDelta);


	return _int();
}

_int CLanding_Smoke::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CLanding_Smoke::Render()
{
	SetUp_Shader_Data();

	_float4 vColor = { 0.6f, 0.6f, 0.6f, (_float)m_dAlphaTime };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);


	return S_OK;
}

void CLanding_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

	vPos += vDir * TimeDelta * m_EffectDesc_Prototype.fDirMoveSpeed;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CLanding_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
	if (-1.f == m_EffectDesc_Clone.UVTime)
		return;

	m_UVTime -= TimeDelta;

	if (m_UVTime <= 0.f)
	{
		m_UVTime = m_EffectDesc_Clone.UVTime;

		if (m_pInstance_UVCount[iIndex].x >= m_EffectDesc_Prototype.iTextureCount_U - 1)
		{
			m_pInstance_UVCount[iIndex].x = 0;

			m_pInstance_UVCount[iIndex].y += 1.f;
		}
		else
			m_pInstance_UVCount[iIndex].x += 1.f;

		if (m_pInstance_UVCount[iIndex].y >= m_EffectDesc_Prototype.iTextureCount_V)
			m_pInstance_UVCount[iIndex].y = 0;

		_float fLeft = (1.f / m_EffectDesc_Prototype.iTextureCount_U) *  m_pInstance_UVCount[iIndex].x;
		_float fTop = (1.f / m_EffectDesc_Prototype.iTextureCount_V) *  m_pInstance_UVCount[iIndex].y;
		_float fRight = (1.f / m_EffectDesc_Prototype.iTextureCount_U) * (m_pInstance_UVCount[iIndex].x + 1.f);
		_float fBottom = (1.f / m_EffectDesc_Prototype.iTextureCount_V) * (m_pInstance_UVCount[iIndex].y + 1.f);

		m_pInstanceBuffer[iIndex].vTextureUV = { fLeft, fTop, fRight, fBottom };
	}
}

void CLanding_Smoke::Control_Alpha(_double TimeDelta)
{
// 	if (false == m_IsDisapear)
// 	{
// 		m_dAlphaTime += TimeDelta * 50.0;
// 		if (0.6 >= m_dAlphaTime)
// 		{
// 			m_dAlphaTime = 0.6;
// 			m_IsDisapear = true;
// 		}
// 	}
// 	else
		m_dAlphaTime -= TimeDelta ;

	
}

void CLanding_Smoke::Control_Instance(_double TimeDelta)
{
	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta);
		m_pInstanceBuffer[iIndex].vTextureUV = Check_UV(TimeDelta, iIndex, false);
	}
}

CLanding_Smoke * CLanding_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CLanding_Smoke*	pInstance = new CLanding_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CLanding_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLanding_Smoke::Clone_GameObject(void * pArg)
{
	CLanding_Smoke* pInstance = new CLanding_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLanding_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLanding_Smoke::Free()
{
	__super::Free();
}
