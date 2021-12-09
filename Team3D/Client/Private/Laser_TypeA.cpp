#include "stdafx.h"
#include "..\Public\Laser_TypeA.h"
#include "Character.h"

CLaser_TypeA::CLaser_TypeA(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLaser(pDevice, pDeviceContext)
{
}

CLaser_TypeA::CLaser_TypeA(const CLaser_TypeA & rhs)
	: CLaser(rhs)
{
}

HRESULT CLaser_TypeA::NativeConstruct_Prototype()
{
	CLaser::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_TypeA::NativeConstruct(void * pArg)
{
	CLaser::NativeConstruct(pArg);

	////클라
	//NULL_CHECK_RETURN(pArg, E_FAIL);
	//m_eTargetID = *(GameID::Enum*)pArg;

	m_pCody = static_cast<CCharacter*>(DATABASE->GetCody());
	NULL_CHECK_RETURN(m_pCody, E_FAIL);
	Safe_AddRef(m_pCody);

	m_pMay = static_cast<CCharacter*>(DATABASE->GetMay());
	NULL_CHECK_RETURN(m_pMay, E_FAIL);
	Safe_AddRef(m_pMay);

	//테스트
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 20.f, 30.f, 1.f));

	return S_OK;
}

_int CLaser_TypeA::Tick(_double TimeDelta)
{
	CLaser::Tick(TimeDelta);

	////클라
	//if (GameID::eCODY == m_eTargetID)
	//	XMStoreFloat4(&m_pInstanceMatrices[0].vLook, XMVector3Normalize(m_pCody->Get_CenterPosition() - XMLoadFloat4(&m_pInstanceMatrices[0].vPosition)));
	//테스트
	//m_pTransformCom->Rotate_ToTarget(m_pCody->Get_CenterPosition());

	if (m_pGameInstance->Key_Pressing(DIK_N))
	{
		if (m_pInstanceMatrices[0].vSize.x > 0.01f)
			m_pInstanceMatrices[0].vSize.x -= 0.01f;
	}
	if (m_pGameInstance->Key_Pressing(DIK_M))
	{
		if (m_pInstanceMatrices[0].vSize.x < 5.f)
			m_pInstanceMatrices[0].vSize.x += 0.01f;
	}
	if (m_pGameInstance->Key_Pressing(DIK_V))
	{
		if (m_pInstanceMatrices[0].vSize.y > 0.1f)
			m_pInstanceMatrices[0].vSize.y -= 0.1f;
	}
	if (m_pGameInstance->Key_Pressing(DIK_B))
	{
		if (m_pInstanceMatrices[0].vSize.y < 100.f)
			m_pInstanceMatrices[0].vSize.y += 0.1f;
	}
	//m_pCircleMatrice[0].vSize.x = m_pInstanceMatrices[0].vSize.x;
	//m_pCircleMatrice[0].vSize.y = m_pCircleMatrice[0].vSize.x;
	//memcpy(&m_pInstanceMatrices[0].vRight, &m_pTransformCom->Get_State(CTransform::STATE_RIGHT), sizeof(_float4));
	//memcpy(&m_pInstanceMatrices[0].vUp, &m_pTransformCom->Get_State(CTransform::STATE_UP), sizeof(_float4));
	//memcpy(&m_pInstanceMatrices[0].vLook, &m_pTransformCom->Get_State(CTransform::STATE_LOOK), sizeof(_float4));

	//m_pInstanceMatrices[0].vLook = _float4()

	return NO_EVENT;
}

_int CLaser_TypeA::Late_Tick(_double TimeDelta)
{
	CLaser::Late_Tick(TimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CLaser_TypeA::Render(RENDER_GROUP::Enum eRender)
{
	CLaser::Render(eRender);

	_int i = 1;
	m_pVIBufferCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pVIBufferCom->Set_DefaultVariables_Perspective(XMMatrixIdentity());
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(3));
	m_pVIBufferCom->Render(12, m_pCircleMatrice, 1);
	m_pVIBufferCom->Render(11, m_pInstanceMatrices, 1);


	return NO_EVENT;
}

HRESULT CLaser_TypeA::Render_ShadowDepth()
{
	CLaser::Render_ShadowDepth();

	return S_OK;
}

CLaser_TypeA * CLaser_TypeA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLaser_TypeA::Clone_GameObject(void * pArg)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_TypeA::Free()
{
	Safe_Release(m_pCody);
	Safe_Release(m_pMay);

	CLaser::Free();
}