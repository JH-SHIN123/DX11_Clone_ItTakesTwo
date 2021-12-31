#include "stdafx.h"
#include "..\Public\Sky.h"

CSky::CSky(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();;

	return S_OK;
}

HRESULT CSky::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);
	m_iViewportIndex = *(_uint*)pArg;;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Sky_Space"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	return S_OK;
}

_int CSky::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_uint iCurLevelStep = m_pGameInstance->Get_CurrentLevelStep();

	if (2 == iCurLevelStep) m_bEnding = true;
	else m_bEnding = false;

	return NO_EVENT;
}

_int CSky::Late_Tick(_double dTimeDelta)
{
	if (1 == m_iViewportIndex)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_MainCamPosition());
	else if (2 == m_iViewportIndex)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pGameInstance->Get_SubCamPosition());

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_PRIORITY, this);

	return NO_EVENT;
}

HRESULT CSky::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_Variable("g_iViewportIndex", &m_iViewportIndex, sizeof(_uint));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	// 0: 전체를 감싸고 있는 구 (알파블랜딩)
	// 1: 4개짜리 클라우드 껍데기 -> G 채널
	// 2: 2개짜리 클라우드 껍데기 -> R 채널
	m_pModelCom->Sepd_Bind_Buffer();

	if (m_bEnding)
	{
		_uint iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0); // 잘못연결한거아님, 모델을 잘못뽑음.
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 3);
	}
	else
	{
		_uint iMaterialIndex = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);
		m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0); // 잘못연결한거아님, 모델을 잘못뽑음.
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0);

		iMaterialIndex = 1;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false);

		iMaterialIndex = 2;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
		m_pModelCom->Sepd_Render_Model(iMaterialIndex, 2, false);
	}

	return S_OK;
}

CSky * CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSky* pInstance = new CSky(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSky::Clone_GameObject(void * pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}