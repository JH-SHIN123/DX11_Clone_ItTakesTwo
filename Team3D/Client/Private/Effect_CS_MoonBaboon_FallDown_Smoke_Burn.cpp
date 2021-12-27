#include "stdafx.h"
#include "..\Public\Effect_CS_MoonBaboon_FallDown_Smoke_Burn.h"
#include "DataStorage.h"
#include "MoonBaboon.h"

CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::CEffect_CS_MoonBaboon_FallDown_Smoke_Burn(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::CEffect_CS_MoonBaboon_FallDown_Smoke_Burn(const CEffect_CS_MoonBaboon_FallDown_Smoke_Burn & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 40;
	m_EffectDesc_Prototype.fLifeTime = 0.f;
	return S_OK;
}

HRESULT CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::NativeConstruct(void * pArg)
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

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Check_TargetMatrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.0 >= m_dControlTime)
		return EVENT_DEAD;

	if (2.f < m_EffectDesc_Prototype.fLifeTime)
		m_IsActivate = false;

	m_EffectDesc_Prototype.fLifeTime += (_float)TimeDelta;

	if (true == m_IsActivate && false == m_isDead)
	{
		m_dControlTime += TimeDelta * 0.5f;
		if (1.0 <= m_dControlTime)
			m_dControlTime = 1.0;
	}
	else
	{
		m_dControlTime -= TimeDelta * 0.75f;
		if (0.0 >= m_dControlTime)
			m_dControlTime = 0.0;
	}

	Check_TargetMatrix();
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Render(RENDER_GROUP::Enum eGroup)
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

	m_pPointInstanceCom_STT->Render(19, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.55f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex])
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_UV((_float)TimeDelta, iIndex);
	}
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon())->Get_Model()->Get_BoneMatrix("Hips");

	WorldMatrix = BoneMatrix * WorldMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x += TimeDelta * 4.25f;
	m_pInstanceBuffer_STT[iIndex].vSize.y += TimeDelta * 4.25f;
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Instance_UV(_float TimeDelta, _int iIndex)
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

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
	m_pInstanceBuffer_STT[iIndex].vRight = Get_RandTexUV();
	m_pInstanceBuffer_STT[iIndex].vUp = Get_RandTexUV();

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.0f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_Update_TextureUV_Time = new _double[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(m_vTexUV.x, m_vTexUV.y, true).z;


	// µðÁ¹ºê´Â Right
	// ¿Ö°îÀº Up

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

		m_pInstance_Pos_UpdateTime[iIndex] = 0.05f  * _double(iIndex) /*+ 0.025f*/;
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
	}
	return S_OK;
}

_float4 CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Get_RandTexUV()
{
	_float fRandUV = (_float)(rand() % 10) / 10.f;

	_float4 vTexLTRB = { fRandUV , fRandUV , fRandUV + 1.f , fRandUV + 1.f };
	return vTexLTRB;
}

CEffect_CS_MoonBaboon_FallDown_Smoke_Burn * CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_CS_MoonBaboon_FallDown_Smoke_Burn*	pInstance = new CEffect_CS_MoonBaboon_FallDown_Smoke_Burn(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_CS_MoonBaboon_FallDown_Smoke_Burn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Clone_GameObject(void * pArg)
{
	CEffect_CS_MoonBaboon_FallDown_Smoke_Burn* pInstance = new CEffect_CS_MoonBaboon_FallDown_Smoke_Burn(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_CS_MoonBaboon_FallDown_Smoke_Burn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_CS_MoonBaboon_FallDown_Smoke_Burn::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_Dissolve);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
