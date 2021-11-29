#include "stdafx.h"
#include "..\public\Performer.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include"CutScenePlayer.h"
#ifdef _AFX
#include"PerformerOption.h"
#include"MainForm.h"
#endif
CPerformer::CPerformer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPerformer::CPerformer(const CPerformer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPerformer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPerformer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);


	memcpy(&m_tDesc, pArg, sizeof(PERFORMERDESC));
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tDesc.strModelTag.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_tDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tDesc.vPosition), 1.f));

	m_pModelCom->Set_Animation(0);
	return S_OK;
}

_int CPerformer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_matrix matWorld = XMMatrixIdentity();

	matWorld = XMMatrixScaling(m_tDesc.vScale.x, m_tDesc.vScale.y, m_tDesc.vScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_tDesc.vRot.z), XMConvertToRadians(m_tDesc.vRot.x), XMConvertToRadians(m_tDesc.vRot.y))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);
	m_pTransformCom->Set_WorldMatrix(matWorld);


	

	m_pModelCom->Update_Animation(CCutScenePlayer::GetInstance()->Get_TimeDelta());

	return NO_EVENT;
}

_int CPerformer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}






HRESULT CPerformer::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

void CPerformer::Start_Perform()
{
	m_pModelCom->Set_Animation(1);
	m_pModelCom->Set_Animation(0);
	m_bStartAnim = true;
}

void CPerformer::Finish_Perform()
{
	m_bStartAnim = false;
}


CPerformer * CPerformer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPerformer* pInstance = new CPerformer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPerformer::Clone_GameObject(void * pArg)
{
	CPerformer* pInstance = new CPerformer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPerformer::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}