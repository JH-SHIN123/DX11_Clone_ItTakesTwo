#include "stdafx.h"
#include "..\Public\Effect_GateSmoke.h"
#include "VIBuffer_SimplePointInstance.h"
#include "InGameEffect.h"

CEffect_GateSmoke::CEffect_GateSmoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_GateSmoke::CEffect_GateSmoke(const CEffect_GateSmoke & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_GateSmoke::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CEffect_GateSmoke::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	//NULL_CHECK_RETURN(pArg, E_FAIL);
	//EFFECT_DESC_CLONE ArgDesc = *(EFFECT_DESC_CLONE*)pArg;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_SimplePointInstance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_DiffuseTexture"), (CComponent**)&m_pDiffuseTextureCom), E_FAIL);

	//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&ArgDesc.WorldMatrix));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 1.f, 30.f, 1.f));

	m_iInstanceCount	= 40;
	m_pVertexLocals		= new _float4x4[m_iInstanceCount];
	m_pSize				= new _float[m_iInstanceCount];
	m_pAlphaWeight		= new _float[m_iInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		m_pAlphaWeight[iIndex] = (rand() % 100) * 0.01f;
		m_pSize[iIndex] = m_pAlphaWeight[iIndex] * 4.f + 2.f;

		_matrix WorldMatrix = XMMatrixScaling(m_pSize[iIndex], m_pSize[iIndex], m_pSize[iIndex]) * XMMatrixTranslation((rand() % 100) * 0.05f, 0.f, m_pAlphaWeight[iIndex] * 10.f);

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], WorldMatrix);
	}

	return S_OK;
}

_int CEffect_GateSmoke::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_float fTimeDelta = (_float)dTimeDelta;

	_vector vRight, vLook, vUp;

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		_matrix LocalMatrix = XMLoadFloat4x4(&m_pVertexLocals[iIndex]);

		m_pAlphaWeight[iIndex] += fTimeDelta * 0.1f;

		if (m_pAlphaWeight[iIndex] > 1.f)
			m_pAlphaWeight[iIndex] -= 1.f;

		m_pSize[iIndex] = m_pAlphaWeight[iIndex] * 4.f + 2.f;

		vRight	= XMVector3Normalize(LocalMatrix.r[0]);
		vUp		= XMVector3Normalize(LocalMatrix.r[1]);
		vLook	= XMVector3Normalize(LocalMatrix.r[2]);

		LocalMatrix.r[0] = vRight * m_pSize[iIndex];
		LocalMatrix.r[1] = vUp * m_pSize[iIndex];
		LocalMatrix.r[2] = vLook * m_pSize[iIndex];
		LocalMatrix.r[3] = XMVectorSet(m_pVertexLocals[iIndex]._41, 0.f, m_pAlphaWeight[iIndex] * 10.f, 1.f - m_pAlphaWeight[iIndex]);

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], LocalMatrix);
	}

	return NO_EVENT;
}

_int CEffect_GateSmoke::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);

	return NO_EVENT;
}

HRESULT CEffect_GateSmoke::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pDiffuseTextureCom->Get_ShaderResourceView(0));

	m_pVIBufferCom->Render(1, m_pVertexLocals, m_iInstanceCount);

	return S_OK;
}

CEffect_GateSmoke * CEffect_GateSmoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_GateSmoke* pInstance = new CEffect_GateSmoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CEffect_GateSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_GateSmoke::Clone_GameObject(void * pArg)
{
	CEffect_GateSmoke* pInstance = new CEffect_GateSmoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_GateSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GateSmoke::Free()
{
	Safe_Delete_Array(m_pVertexLocals);
	Safe_Delete_Array(m_pSize);
	Safe_Delete_Array(m_pAlphaWeight);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
