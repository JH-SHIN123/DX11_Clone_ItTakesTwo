#include "stdafx.h"
#include "..\Public\Effect_CS_MoonBaboon_Land.h"
#include "DataStorage.h"
#include "MoonBaboon.h"

CEffect_CS_MoonBaboon_Land::CEffect_CS_MoonBaboon_Land(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_CS_MoonBaboon_Land::CEffect_CS_MoonBaboon_Land(const CEffect_CS_MoonBaboon_Land & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_CS_MoonBaboon_Land::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 10;

	return S_OK;
}

HRESULT CEffect_CS_MoonBaboon_Land::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Check_WorldMatrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_CS_MoonBaboon_Land::Tick(_double TimeDelta)
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

_int CEffect_CS_MoonBaboon_Land::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_CS_MoonBaboon_Land::Render(RENDER_GROUP::Enum eGroup)
{
	m_pPointInstanceCom_STT->Set_DefaultVariables();

	_float4 vColor = { 0.1f, 0.1f, 0.1f, 1.f };
	m_pPointInstanceCom_STT->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom_STT->Render(25, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_CS_MoonBaboon_Land::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_MoonBaboon_Land::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBiffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);

	vPos += vDir * TimeDelta * m_fInstance_SpeedPerSec * 5.f;

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_CS_MoonBaboon_Land::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_UVTime[iIndex] -= (_double)TimeDelta;
	if (0.0 >= m_pInstance_UVTime[iIndex])
	{
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;
		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Check_UV(m_vUVCount_Max.x, m_vUVCount_Max.y, &m_pInstanceBuffer_UVCount[iIndex].x, &m_pInstanceBuffer_UVCount[iIndex].y);
	}
}

HRESULT CEffect_CS_MoonBaboon_Land::Ready_InstanceBuffer()
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
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vUVCount_Max.x, m_vUVCount_Max.y, &m_pInstanceBuffer_UVCount[iIndex].x, &m_pInstanceBuffer_UVCount[iIndex].y);
		m_pInstanceBuffer_STT[iIndex].fTime = 0.5f;
		m_pInstanceBuffer_STT[iIndex].vSize = Get_Rand_Size();
		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time + (_double(iIndex) / iInstanceCount);
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos + (vRandDir * 0.8f));

		_float3 v3RandDir;
		XMStoreFloat3(&v3RandDir, vRandDir);
		m_pInstanceBiffer_Dir[iIndex] = v3RandDir;
		if (0.f <= m_pInstanceBiffer_Dir[iIndex].y)
			m_pInstanceBiffer_Dir[iIndex].y *= -1.2f;
	}

	return S_OK;
}

_float2 CEffect_CS_MoonBaboon_Land::Get_Rand_Size()
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

void CEffect_CS_MoonBaboon_Land::Check_InstanceBuffer(_double TimeDelta)
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.25f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		if (0.0 <= m_pInstance_Pos_UpdateTime[iIndex])
		{
			Instance_Pos((_float)TimeDelta, iIndex);
			Instance_UV((_float)TimeDelta, iIndex);
		}
	}
}

void CEffect_CS_MoonBaboon_Land::Check_WorldMatrix()
{
	_matrix WorldMatrix = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CMoonBaboon*>(DATABASE->Get_MoonBaboon())->Get_Model()->Get_BoneMatrix("Hips");

	WorldMatrix = BoneMatrix * WorldMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

CEffect_CS_MoonBaboon_Land * CEffect_CS_MoonBaboon_Land::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_CS_MoonBaboon_Land*	pInstance = new CEffect_CS_MoonBaboon_Land(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_CS_MoonBaboon_Land");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_CS_MoonBaboon_Land::Clone_GameObject(void * pArg)
{
	CEffect_CS_MoonBaboon_Land* pInstance = new CEffect_CS_MoonBaboon_Land(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_CS_MoonBaboon_Land");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_CS_MoonBaboon_Land::Free()
{
	Safe_Release(m_pPointInstanceCom_STT);

	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBiffer_Dir);
	Safe_Delete_Array(m_pInstanceBuffer_UVCount);
	Safe_Delete_Array(m_pInstance_UVTime);

	__super::Free();
}
