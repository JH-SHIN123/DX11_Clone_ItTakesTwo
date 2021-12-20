#include "stdafx.h"
#include "..\Public\Effect_MoonBaboon_Booster.h"
#include "DataStorage.h"
#include "RunningMoonBaboon.h"
#ifdef __TEST_JUNG
#include "Cody.h"
#endif // __TEST_JUNG


CEffect_MoonBaboon_Booster::CEffect_MoonBaboon_Booster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_MoonBaboon_Booster::CEffect_MoonBaboon_Booster(const CEffect_MoonBaboon_Booster & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_MoonBaboon_Booster::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 1;
	return S_OK;
}

HRESULT CEffect_MoonBaboon_Booster::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	//

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	Check_TargetMatrix();
	return S_OK;
}

_int CEffect_MoonBaboon_Booster::Tick(_double TimeDelta)
{
	// 	/*Gara*/ m_pTransformCom->Set_WorldMatrix(static_cast<CCody*>(DATABASE->GetCody())->Get_WorldMatrix());

	if (2.0 < m_dActivateTime && 0.0 > m_dControlTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;

	if (2.0 > m_dActivateTime)
	{
		m_IsActivate = true;
		m_dControlTime += TimeDelta;
		if (1.0 <= m_dControlTime) m_dControlTime = 1.0;
	}
	else
	{
		m_IsActivate = false;
		m_dControlTime -= TimeDelta * 0.1;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	Check_TargetMatrix();
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_MoonBaboon_Booster::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_MoonBaboon_Booster::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(16, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_MoonBaboon_Booster::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_MoonBaboon_Booster::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		Instance_Size((_float)TimeDelta, 0);
		//Instance_Pos((_float)TimeDelta, 0);
		Instance_UV((_float)TimeDelta, 0);
}

void CEffect_MoonBaboon_Booster::Instance_Size(_float TimeDelta, _int iIndex)
{
	if (true == m_IsActivate)
	{
		m_pInstanceBuffer_STT[iIndex].vSize.x += TimeDelta * (m_vDefaultSize.x - m_pInstanceBuffer_STT[iIndex].vSize.x + 0.2f);
		m_pInstanceBuffer_STT[iIndex].vSize.y += TimeDelta * (m_fMaxSize_Y - m_pInstanceBuffer_STT[iIndex].vSize.y + 3.f);	
		if (m_fMaxSize_Y < m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = m_vDefaultSize.x;
		if (m_fMaxSize_Y < m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = m_fMaxSize_Y;
	}
	else
	{
		m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * (m_vDefaultSize.x - m_pInstanceBuffer_STT[iIndex].vSize.x + 0.5f);
		m_pInstanceBuffer_STT[iIndex].vSize.y -= TimeDelta * (m_fMaxSize_Y - m_pInstanceBuffer_STT[iIndex].vSize.y + 0.5f);
		if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
		if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;
	}
}

void CEffect_MoonBaboon_Booster::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_MoonBaboon_Booster::Instance_UV(_float TimeDelta, _int iIndex)
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

		else if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.z = m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.y += m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.w += m_fNextUV;
		}
	}
}

void CEffect_MoonBaboon_Booster::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV(7, 7, true);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_MoonBaboon_Booster::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT				= new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Update_TextureUV_Time	= new _double[iInstanceCount];
	m_pInstance_Pos_UpdateTime			= new _double[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(8, 8, true).z;

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, m_fNextUV , m_fNextUV };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex] = 0.f;
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
	}
	return S_OK;
}

void CEffect_MoonBaboon_Booster::Check_TargetMatrix()
{
	_matrix MyMatrix = XMMatrixIdentity();
	_matrix WorldMatrix	 = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix_1 = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Model()->Get_BoneMatrix("Spine1");
	_matrix BoneMatrix_2 = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Model()->Get_BoneMatrix("Spine2");
	_matrix BoneMatrix_New = XMMatrixIdentity();

	MyMatrix.r[3] = XMVectorSet(-1.f, 0.75f, 0.f, 1.f);

	for (_int i = 0; i < 4; ++i)
		BoneMatrix_New.r[i] = XMVectorLerp(BoneMatrix_2.r[i], BoneMatrix_1.r[i], 0.9f);

	BoneMatrix_New = BoneMatrix_1;

	BoneMatrix_New = BoneMatrix_New * WorldMatrix * MyMatrix;
	for (_int i = 0; i < 3; ++i)
		BoneMatrix_New.r[i] = XMVector3Normalize(BoneMatrix_New.r[i]);

	//BoneMatrix.r[3] -= WorldMatrix.r[1] * 0.9f;

	m_pTransformCom->Set_WorldMatrix(BoneMatrix_New);
	XMStoreFloat4(&m_pInstanceBuffer_STT[0].vPosition, BoneMatrix_New.r[3]);
	XMStoreFloat4(&m_pInstanceBuffer_STT[0].vUp, BoneMatrix_New.r[2]);
	 
}

CEffect_MoonBaboon_Booster * CEffect_MoonBaboon_Booster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_MoonBaboon_Booster*	pInstance = new CEffect_MoonBaboon_Booster(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_MoonBaboon_Booster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_MoonBaboon_Booster::Clone_GameObject(void * pArg)
{
	CEffect_MoonBaboon_Booster* pInstance = new CEffect_MoonBaboon_Booster(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_MoonBaboon_Booster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_MoonBaboon_Booster::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
