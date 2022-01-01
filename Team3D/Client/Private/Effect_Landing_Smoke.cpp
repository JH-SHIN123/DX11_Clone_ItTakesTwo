#include "stdafx.h"
#include "..\Public\Effect_Landing_Smoke.h"

CEffect_Landing_Smoke::CEffect_Landing_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Landing_Smoke::CEffect_Landing_Smoke(const CEffect_Landing_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Landing_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.fLifeTime = 2.f;

	return S_OK;
}

HRESULT CEffect_Landing_Smoke::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	
	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pInstanceBuffer[0].vSize.x *= fSize;
	m_pInstanceBuffer[0].vSize.y *= fSize;

	return S_OK;
}

_int CEffect_Landing_Smoke::Tick(_double TimeDelta)
{
	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	Control_Alpha(TimeDelta);

	Control_Instance((_float)TimeDelta);

	m_pInstanceBuffer[0].vSize.x += (_float)TimeDelta * 0.5f;
	m_pInstanceBuffer[0].vSize.y += (_float)TimeDelta * 0.5f;

	return _int();
}

_int CEffect_Landing_Smoke::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Landing_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_float4 vColor = { 0.6f, 0.6f, 0.6f, (_float)m_dAlphaTime };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);


	return S_OK;
}

void CEffect_Landing_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Landing_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

	vPos += vDir * TimeDelta * m_EffectDesc_Prototype.fDirMoveSpeed;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Landing_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
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

void CEffect_Landing_Smoke::Control_Alpha(_double TimeDelta)
{
	m_dAlphaTime -= TimeDelta ;	
}

void CEffect_Landing_Smoke::Control_Instance(_double TimeDelta)
{
	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta);
		m_pInstanceBuffer[iIndex].vTextureUV = Check_UV(TimeDelta, iIndex, false);
	}
}

CEffect_Landing_Smoke * CEffect_Landing_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Landing_Smoke*	pInstance = new CEffect_Landing_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Landing_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Landing_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_Landing_Smoke* pInstance = new CEffect_Landing_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Landing_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Landing_Smoke::Free()
{
	__super::Free();
}
