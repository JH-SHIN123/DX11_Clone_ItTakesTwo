#include "stdafx.h"
#include "..\public\UFO.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "May.h"
#include "Cody.h"

CUFO::CUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUFO::CUFO(const CUFO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUFO::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUFO::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	m_pCodyTransform = ((CCody*)CDataBase::GetInstance()->GetCody())->Get_Transform();
	if (nullptr == m_pCodyTransform)
		return E_FAIL;
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataBase::GetInstance()->GetMay())->Get_Transform();
	if (nullptr == m_pMayTransform)
		return E_FAIL;
	Safe_AddRef(m_pMayTransform);

	m_pActorCom->Set_Gravity(0.f);
	m_pModelCom->Set_Animation(23, m_pTransformCom);
	m_pModelCom->Set_NextAnimIndex(23);

	// DataBase에 포인터 등록
	CDataBase::GetInstance()->Set_UFOPtr(this);

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	
	if (m_pGameInstance->Key_Pressing(DIK_K))
		m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_LOOK), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_M))
		m_pActorCom->Move(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_N))
		m_pActorCom->Move(XMVectorSet(0.f, 1.f, 0.f, 0.f) / 10.f, dTimeDelta);

	Check_State(dTimeDelta);
	Change_State(dTimeDelta);
	During_Animation_Behavior(dTimeDelta);


	PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);

	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}


CUFO::UFO_STATE CUFO::Check_State(_double dTimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		m_pModelCom->Set_Animation(m_eNextState, m_pTransformCom);
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CUFO::Change_State(_double dTimeDelta)
{

}

void CUFO::During_Animation_Behavior(_double dTimeDelta)
{
	Laser_Pattern(dTimeDelta);
}

void CUFO::Laser_Pattern(_double dTimeDelta)
{
	_vector vDir = m_pCodyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_vector vDirForLaser = XMVector3Normalize(vDir);

	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta); // 플레이어 쪽으로 천천히 회전.

}

HRESULT CUFO::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	s
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CUFO::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CUFO::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	return S_OK;
}

CUFO * CUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFO* pInstance = new CUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUFO::Clone_GameObject(void * pArg)
{
	CUFO* pInstance = new CUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUFO::Free()
{
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}