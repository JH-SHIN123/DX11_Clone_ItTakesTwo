#include "stdafx.h"
#include "..\Public\Effect_PipeLocker_Connected.h"

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
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);



	return S_OK;
}

_int CEffect_PipeLocker_Connected::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	//for (_uint iIndex = 0; iIndex < 50; ++iIndex)
	//{
	//	_matrix LocalMatrix = XMLoadFloat4x4(&m_pVertexLocals[iIndex]);

	//	_float fY = XMVectorGetY(LocalMatrix.r[3]);

	//	if (fY > 10.f)
	//		LocalMatrix.r[3] = XMVectorSetY(LocalMatrix.r[3], fY - 9.7f);

	//	LocalMatrix.r[3] += XMVectorSet(0.f, 1.f, 0.f, 0.f) * (_float)dTimeDelta;

	//	XMStoreFloat4x4(&m_pVertexLocals[iIndex], LocalMatrix);
	//}

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
	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
