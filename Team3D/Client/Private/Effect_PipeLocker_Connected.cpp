#include "stdafx.h"
#include "..\Public\Effect_PipeLocker_Connected.h"
#include "VIBuffer_SimplePointInstance.h"

CEffect_PipeLocker_Connected::CEffect_PipeLocker_Connected(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_PipeLocker_Connected::CEffect_PipeLocker_Connected(const CEffect_PipeLocker_Connected & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_PipeLocker_Connected::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CEffect_PipeLocker_Connected::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_SimplePointInstance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_DiffuseTexture"), (CComponent**)&m_pDiffuseTextureCom), E_FAIL); // 2¹ø
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Mask_Drop"), TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom), E_FAIL); // 2¹ø

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 0.f, 30.f, 1.f));

	m_iInstanceCount = 36;
	m_pVertexLocals = new _float4x4[m_iInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		//_matrix WorldMatrix = XMMatrixScaling(0.1f, 1.f, 0.3f) * XMMatrixRotationX(XMConvertToRadians((rand() % 100) * 0.3f) + 15.f) * XMMatrixRotationY(XMConvertToRadians(iIndex * 10.f));
		_matrix WorldMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationX(XMConvertToRadians((rand() % 100) * 0.3f) + 15.f) * XMMatrixRotationY(XMConvertToRadians(iIndex * 10.f));

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], WorldMatrix);

		//m_pVertexLocals[iIndex]._42 = (rand() % 100) * 0.01f;
	}

	return S_OK;
}

_int CEffect_PipeLocker_Connected::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		_matrix LocalMatrix = XMLoadFloat4x4(&m_pVertexLocals[iIndex]);

		_vector vLook = XMVector3Normalize(LocalMatrix.r[2]);

		LocalMatrix.r[3] += vLook * (_float)dTimeDelta;

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], LocalMatrix);
	}

	return NO_EVENT;
}

_int CEffect_PipeLocker_Connected::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CEffect_PipeLocker_Connected::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pDiffuseTextureCom->Get_ShaderResourceView(2));
	m_pVIBufferCom->Set_ShaderResourceView("g_MaskTexture", m_pMaskTextureCom->Get_ShaderResourceView(0));

	m_pVIBufferCom->Render(0, m_pVertexLocals, m_iInstanceCount);

	return S_OK;
}

CEffect_PipeLocker_Connected * CEffect_PipeLocker_Connected::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_PipeLocker_Connected* pInstance = new CEffect_PipeLocker_Connected(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CEffect_PipeLocker_Connected");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_PipeLocker_Connected::Clone_GameObject(void * pArg)
{
	CEffect_PipeLocker_Connected* pInstance = new CEffect_PipeLocker_Connected(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_PipeLocker_Connected");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_PipeLocker_Connected::Free()
{
	Safe_Delete_Array(m_pVertexLocals);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
