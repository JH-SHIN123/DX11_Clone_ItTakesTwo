#include "stdafx.h"
#include "..\Public\Effect_MoonUFO_Laser_ColorSmoke.h"
#include "Moon.h"
#include "DataStorage.h"
#include "Effect_MoonUFO_Laser_Smoke.h"

CEffect_MoonUFO_Laser_ColorSmoke::CEffect_MoonUFO_Laser_ColorSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_MoonUFO_Laser_ColorSmoke::CEffect_MoonUFO_Laser_ColorSmoke(const CEffect_MoonUFO_Laser_ColorSmoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_MoonUFO_Laser_ColorSmoke::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 300;
	return S_OK;
}

HRESULT CEffect_MoonUFO_Laser_ColorSmoke::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"), TEXT("Com_Texture_Dissolve"), (CComponent**)&m_pTexturesCom_Dissolve), E_FAIL);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Moon_Effects"), Level::LEVEL_STAGE, TEXT("GameObject_2D_MoonUFO_Laser_Smoke"), nullptr, (CGameObject**)&m_pEffect_Smoke);

	XMStoreFloat4(&m_vMoonPosition, DATABASE->Get_Mooon()->Get_Position());
	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_MoonUFO_Laser_ColorSmoke::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.0 >= m_dControlTime)
		return EVENT_DEAD;

	if (true == m_isDead)
	{
		m_pEffect_Smoke->Set_Dead();
		m_IsActivate = false;
	}

	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (1.0 <= m_dControlTime)m_dControlTime = 1.0;
	}
	else
	{
		m_dControlTime -= TimeDelta * 0.2f;
		if (0.0 >= m_dControlTime)m_dControlTime = 0.0;
	}

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_MoonUFO_Laser_ColorSmoke::Late_Tick(_double TimeDelta)
{
	//if (EFFECT_DESC_CLONE::PV_CODY == m_EffectDesc_Clone.iPlayerValue)
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
	//else
	//return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_MoonUFO_Laser_ColorSmoke::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fAlpha", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));	//½º¸ðÅ©
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3)); // »ö»ó
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(1)); // ¿Ö°î
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DissolveTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0)); // µðÁ¹ºê

	m_pPointInstanceCom_STT->Render(22, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_MoonUFO_Laser_ColorSmoke::Set_Pos(_fvector vPos)
{
	_vector vMoonPos = XMLoadFloat4(&m_vMoonPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vNormal = XMVector3Normalize(vPos - vMoonPos);
	m_pEffect_Smoke->Set_Pos(vPos, vNormal);
	XMStoreFloat3(&m_vNormal, vNormal);
}

void CEffect_MoonUFO_Laser_ColorSmoke::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.25f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_UV((_float)TimeDelta, iIndex);
	}
}

void CEffect_MoonUFO_Laser_ColorSmoke::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x += TimeDelta * 10.0f;
	m_pInstanceBuffer_STT[iIndex].vSize.y += TimeDelta * 10.0f;
}

void CEffect_MoonUFO_Laser_ColorSmoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);

	vPos += vDir * TimeDelta * 7.f * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_MoonUFO_Laser_ColorSmoke::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_Update_TextureUV_Time[iIndex] -= TimeDelta;

	if (0 >= m_pInstance_Update_TextureUV_Time[iIndex])
	{
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.01;

		m_pInstanceBuffer_STT[iIndex].vTextureUV.x += m_fNextUV;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.z += m_fNextUV;

		if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.y)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.y = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.w = m_fNextUV;
			if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
			{
				m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 1.f - m_fNextUV;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.y = 1.f - m_fNextUV;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.z = 1.f;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.w = 1.f;
			}
		}

		if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.z = m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.y += m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.w += m_fNextUV;
		}
	}
}

void CEffect_MoonUFO_Laser_ColorSmoke::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
	m_pInstanceBuffer_STT[iIndex].vRight = Get_RandTexUV();
	m_pInstanceBuffer_STT[iIndex].vUp = Get_RandTexUV();

	m_pInstance_Dir[iIndex] = m_vNormal;

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.0f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_MoonUFO_Laser_ColorSmoke::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_Update_TextureUV_Time = new _double[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(m_vTexUV.x, m_vTexUV.y, true).z;

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = Get_RandTexUV();
		m_pInstanceBuffer_STT[iIndex].vUp = Get_RandTexUV();
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize = { 0.f, 0.f };

		m_pInstance_Pos_UpdateTime[iIndex] = (_double(iIndex) / _double(iInstanceCount));
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
	}
	return S_OK;
}

_float4 CEffect_MoonUFO_Laser_ColorSmoke::Get_RandTexUV()
{
	_float fRandUV = (_float)(rand() % 10) / 10.f;

	_float4 vTexLTRB = { fRandUV , fRandUV , fRandUV + 1.f , fRandUV + 1.f };
	return vTexLTRB;
}

CEffect_MoonUFO_Laser_ColorSmoke * CEffect_MoonUFO_Laser_ColorSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_MoonUFO_Laser_ColorSmoke*	pInstance = new CEffect_MoonUFO_Laser_ColorSmoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_MoonUFO_Laser_ColorSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_MoonUFO_Laser_ColorSmoke::Clone_GameObject(void * pArg)
{
	CEffect_MoonUFO_Laser_ColorSmoke* pInstance = new CEffect_MoonUFO_Laser_ColorSmoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_MoonUFO_Laser_ColorSmoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_MoonUFO_Laser_ColorSmoke::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_Dissolve);
	Safe_Release(m_pPointInstanceCom_STT);

	Safe_Release(m_pEffect_Smoke);

	__super::Free();
}
