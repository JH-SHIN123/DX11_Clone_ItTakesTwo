#include "stdafx.h"
#include "..\Public\Effect_Boss_Missile_Smoke_Black.h"
#include "DataStorage.h"
#include "Cody.h"
#include "May.h"
#include "EndingRocket.h"

CEffect_Boss_Missile_Smoke_Black::CEffect_Boss_Missile_Smoke_Black(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Missile_Smoke_Black::CEffect_Boss_Missile_Smoke_Black(const CEffect_Boss_Missile_Smoke_Black & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Missile_Smoke_Black::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 40;
	return S_OK;
}

HRESULT CEffect_Boss_Missile_Smoke_Black::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);


	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Missile_Smoke_Black::Tick(_double TimeDelta)
{
//	if (EFFECT_DESC_CLONE::PV_CODY == m_EffectDesc_Clone.iPlayerValue)
//		/*Gara*/ m_pTransformCom->Set_WorldMatrix(static_cast<CCody*>(DATABASE->GetCody())->Get_WorldMatrix());
//	else
//		/*Gara*/ m_pTransformCom->Set_WorldMatrix(static_cast<CMay*>(DATABASE->GetMay())->Get_WorldMatrix());
	/*Gara*/ m_pTransformCom->Set_WorldMatrix(static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Transform()->Get_WorldMatrix());

	if (m_dInstance_Pos_Update_Time + 1.5 <= m_dControlTime)
		return EVENT_DEAD;

	m_dControlTime += TimeDelta;
	if (true == m_IsActivate)
	{
		if (1.0 <= m_dControlTime)
			m_dControlTime = 1.0;
	}

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Missile_Smoke_Black::Late_Tick(_double TimeDelta)
{
	//if(EFFECT_DESC_CLONE::PV_CODY == m_EffectDesc_Clone.iPlayerValue)
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
	//else
	//	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);

}

HRESULT CEffect_Boss_Missile_Smoke_Black::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fAlpha", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(1));
	m_pPointInstanceCom_STT->Render(18, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Missile_Smoke_Black::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_Boss_Missile_Smoke_Black::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.56f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_UV((_float)TimeDelta, iIndex);
	}
}

void CEffect_Boss_Missile_Smoke_Black::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x += TimeDelta * 2.5f;
	m_pInstanceBuffer_STT[iIndex].vSize.y += TimeDelta * 2.5f;
}

void CEffect_Boss_Missile_Smoke_Black::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Missile_Smoke_Black::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_Update_TextureUV_Time[iIndex] -= TimeDelta;

	if (0 >= m_pInstance_Update_TextureUV_Time[iIndex])
	{
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;

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

void CEffect_Boss_Missile_Smoke_Black::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_Boss_Missile_Smoke_Black::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_Update_TextureUV_Time = new _double[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(m_vTexUV.x, m_vTexUV.y, true).z;

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize = {0.f, 0.f};

		m_pInstance_Pos_UpdateTime[iIndex] = 0.05f  * _double(iIndex);
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
	}
	return S_OK;
}

CEffect_Boss_Missile_Smoke_Black * CEffect_Boss_Missile_Smoke_Black::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Missile_Smoke_Black*	pInstance = new CEffect_Boss_Missile_Smoke_Black(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Missile_Smoke_Black");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Missile_Smoke_Black::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Missile_Smoke_Black* pInstance = new CEffect_Boss_Missile_Smoke_Black(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Missile_Smoke_Black");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Missile_Smoke_Black::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
