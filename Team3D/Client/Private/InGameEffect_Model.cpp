#include "stdafx.h"
#include "..\Public\InGameEffect_Model.h"
#include "GameInstance.h"

CInGameEffect_Model::CInGameEffect_Model(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInGameEffect_Model::CInGameEffect_Model(const CInGameEffect_Model & rhs)
	: CGameObject(rhs)
	, m_EffectDesc_Prototype(rhs.m_EffectDesc_Prototype)
{
}

HRESULT CInGameEffect_Model::NativeConstruct_Prototype(void * pArg)
{
	memcpy(&m_EffectDesc_Prototype, pArg, sizeof(EFFECT_DESC_PROTO));

	return S_OK;
}

HRESULT CInGameEffect_Model::NativeConstruct(void * pArg)
{
	Ready_InstanceBuffer();
	Ready_Component(pArg);

	return S_OK;
}

_int CInGameEffect_Model::Tick(_double TimeDelta)
{
	return _int();
}

_int CInGameEffect_Model::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CInGameEffect_Model::Render()
{
	return S_OK;
}

HRESULT CInGameEffect_Model::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	if(1 < lstrlen(m_EffectDesc_Prototype.TextureName))
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Textrue"), (CComponent**)&m_pTexturesCom), E_FAIL);
	


	CModel_Instance::ARG_DESC Data;
	char szActorName[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, m_EffectDesc_Prototype.ModelName, MAX_PATH, szActorName, MAX_PATH, NULL, NULL);
	Data.pActorName		= szActorName;
	Data.fCullingRadius = m_EffectDesc_Clone.fCullingRadius;
	Data.pWorldMatrices = m_pInstanceBuffer;
	Data.iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelInstanceCom, &Data), E_FAIL);



	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);

	for (_int i = 0; i < 3; ++i)
		XMVector3Normalize(WolrdMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	return S_OK;
}

HRESULT CInGameEffect_Model::Ready_InstanceBuffer(_bool IsRenderTerm)
{
	m_pInstanceBuffer = new _float4x4[m_EffectDesc_Prototype.iInstanceCount];

	const _int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	_matrix WorldMatrix = XMMatrixIdentity();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		XMStoreFloat4x4(&m_pInstanceBuffer[iIndex], WorldMatrix);

		WorldMatrix.r[3].m128_f32[1] += 5.f;
		WorldMatrix.r[3].m128_f32[3] += 5.f;
	}

	return S_OK;
}

_float4 CInGameEffect_Model::Set_UV(_int iIndex)
{
	return _float4();
}

CGameObject * CInGameEffect_Model::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CInGameEffect_Model::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pModelInstanceCom);

	Safe_Delete_Array(m_pInstanceBuffer);

	__super::Free();
}
