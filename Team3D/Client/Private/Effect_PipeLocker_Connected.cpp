#include "stdafx.h"
#include "..\Public\Effect_PipeLocker_Connected.h"
#include "VIBuffer_SimplePointInstance.h"
#include "InGameEffect.h"

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

	NULL_CHECK_RETURN(pArg, E_FAIL);
	EFFECT_DESC_CLONE ArgDesc = *(EFFECT_DESC_CLONE*)pArg;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_SimplePointInstance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_DiffuseTexture"), (CComponent**)&m_pDiffuseTextureCom), E_FAIL); // 2¹ø
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Mask_Drop"), TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom), E_FAIL); // 2¹ø

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&ArgDesc.WorldMatrix));

	m_iInstanceCount	= 36;
	m_fGrvityWeight		= 40.f;
	m_fShootingPower	= 10.f;
	m_pVertexLocals		= new _float4x4[m_iInstanceCount];
	m_pSize				= new _float3[m_iInstanceCount];

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		_float fSize = (rand() % 100) * 0.02f;

		m_pSize[iIndex] = _float3(0.2f * fSize, 1.f, 0.4f * fSize);

		_matrix WorldMatrix = XMMatrixScaling(m_pSize[iIndex].x, m_pSize[iIndex].y, m_pSize[iIndex].z) * XMMatrixRotationX(XMConvertToRadians((rand() % 100) * 0.3f) - 15.f) * XMMatrixRotationY(XMConvertToRadians(iIndex * 10.f));

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], WorldMatrix);
	}

	//FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Effect", Level::LEVEL_STAGE, TEXT("Effect_PipeLocker_Connected")), E_FAIL);

	return S_OK;
}

_int CEffect_PipeLocker_Connected::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_float fTimeDelta = (_float)dTimeDelta;
	_float fResize = fTimeDelta * 0.6f;

	_vector vRight, vLook, vUp;
	_vector vGravity	= XMVectorSet(0.f, -9.8f, 0.f, 0.f);
	_vector vVelocity	= vGravity * fTimeDelta * m_fGrvityWeight;

	_bool	isFinish = true;

	for (_uint iIndex = 0; iIndex < m_iInstanceCount; ++iIndex)
	{
		_matrix LocalMatrix = XMLoadFloat4x4(&m_pVertexLocals[iIndex]);

		vRight	= XMVector3Normalize(LocalMatrix.r[0]);
		vLook	= XMVector3Normalize(LocalMatrix.r[2]);
		vLook	= XMVector3Normalize(vLook + (vLook + vVelocity) * fTimeDelta);
		vUp		= XMVector3Normalize(XMVector3Cross(vLook, vRight));

		LocalMatrix.r[3] += vLook * fTimeDelta * m_fShootingPower;
		LocalMatrix.r[0] = vRight * m_pSize[iIndex].x;
		LocalMatrix.r[1] = vUp * m_pSize[iIndex].y;
		LocalMatrix.r[2] = vLook * m_pSize[iIndex].z;

		XMStoreFloat4x4(&m_pVertexLocals[iIndex], LocalMatrix);

		if (m_pSize[iIndex].x > fResize)
		{
			m_pSize[iIndex].x -= fResize;

			if (m_pSize[iIndex].x < 0.f)
				m_pSize[iIndex].x = 0.f;

			isFinish = false;
		}
	}

	if (isFinish)
		return EVENT_DEAD;

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
	Safe_Delete_Array(m_pSize);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
