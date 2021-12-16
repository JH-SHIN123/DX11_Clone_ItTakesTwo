#include "stdafx.h"
#include "..\Public\Effect_Boss_Core_Lightning.h"

CEffect_Boss_Core_Lightning::CEffect_Boss_Core_Lightning(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Core_Lightning::CEffect_Boss_Core_Lightning(const CEffect_Boss_Core_Lightning & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Core_Lightning::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 30;

	return S_OK;
}

HRESULT CEffect_Boss_Core_Lightning::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ligntning_01"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Core_Lightning::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.0 >= m_dControlTime)
		return EVENT_DEAD;


	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;

		if (1.0 <= m_dControlTime)
			m_dControlTime = 1.0;
	}
	else
		m_dControlTime -= TimeDelta;

	m_dActivateTime += TimeDelta;
	if (6.f <= m_dActivateTime)
		m_IsActivate = false;


	Check_InstanceBuffer(TimeDelta);

	return _int();
}

_int CEffect_Boss_Core_Lightning::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Core_Lightning::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.98f, 0.98f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));

	m_pPointInstanceCom_STT->Render(8, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Core_Lightning::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Core_Lightning::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBiffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);

	vPos += vDir * TimeDelta * m_fInstance_SpeedPerSec * 5.f;

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Boss_Core_Lightning::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_UVTime[iIndex] -= (_double)TimeDelta;
	if (0.0 >= m_pInstance_UVTime[iIndex])
	{
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;
		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Check_UV(m_vUVCount_Max.x, m_vUVCount_Max.y, &m_pInstanceBuffer_UVCount[iIndex].x, &m_pInstanceBuffer_UVCount[iIndex].y);
	}
}

HRESULT CEffect_Boss_Core_Lightning::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstanceBiffer_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_UVTime = new _double[iInstanceCount];
	m_pInstanceBuffer_UVCount = new XMINT2[iInstanceCount];

	_float4 vMyPos;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vUVCount_Max.x, m_vUVCount_Max.y, &m_pInstanceBuffer_UVCount[iIndex].x, &m_pInstanceBuffer_UVCount[iIndex].y);
		m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize = Get_Rand_Size();
		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time + (_double(iIndex) / iInstanceCount);
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos /*+ (vRandDir * 0.8f)*/);

		_float3 v3RandDir;
		XMStoreFloat3(&v3RandDir, vRandDir);
		m_pInstanceBiffer_Dir[iIndex] = v3RandDir;
		m_pInstanceBuffer_STT[iIndex].vUp = { v3RandDir.x, v3RandDir.y, v3RandDir.z, 0.f };
		
	}

	return S_OK;
}

_float2 CEffect_Boss_Core_Lightning::Get_Rand_Size()
{
	_float2 vSize = m_vDefaultSize;

	_int iRandNum = rand() % 3;
	switch (iRandNum)
	{
	case 0:
		vSize.x -= 0.5f;
		vSize.y -= 0.5f;
		break;
	case 1:
		vSize.x += 0.5f;
		vSize.y += 0.5f;
		break;
	}

	return vSize;
}

void CEffect_Boss_Core_Lightning::Check_InstanceBuffer(_double TimeDelta)
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		if (1.2 <= m_pInstance_Pos_UpdateTime[iIndex])
		{
			m_pInstanceBuffer_STT[iIndex].fTime += (_float)TimeDelta * m_fAlphaTime_Power;
			if (1.f <= m_pInstanceBuffer_STT[iIndex].fTime)
				m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		}
		else
		{
			m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * m_fAlphaTime_Power;
			if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
				m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
		}

		if (0.0 <= m_pInstance_Pos_UpdateTime[iIndex])
			Instance_UV((_float)TimeDelta, iIndex);
	}
}

CEffect_Boss_Core_Lightning * CEffect_Boss_Core_Lightning::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Core_Lightning*	pInstance = new CEffect_Boss_Core_Lightning(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Core_Lightning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Core_Lightning::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Core_Lightning* pInstance = new CEffect_Boss_Core_Lightning(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Core_Lightning");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Core_Lightning::Free()
{
	Safe_Release(m_pPointInstanceCom_STT);

	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBiffer_Dir);
	Safe_Delete_Array(m_pInstanceBuffer_UVCount);
	Safe_Delete_Array(m_pInstance_UVTime);

	__super::Free();
}
