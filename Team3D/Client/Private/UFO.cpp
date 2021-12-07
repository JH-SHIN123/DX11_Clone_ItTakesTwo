#include "stdafx.h"
#include "..\Public\UFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"

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

	ROBOTDESC UFODesc;
	if (nullptr != pArg)
		memcpy(&UFODesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, UFODesc.vPosition);

	m_pCodyTransform = ((CCody*)CDataStorage::GetInstance()->GetCody())->Get_Transform();
	NULL_CHECK_RETURN(m_pCodyTransform, E_FAIL);
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataStorage::GetInstance()->GetMay())->Get_Transform();
	NULL_CHECK_RETURN(m_pMayTransform, E_FAIL);
	Safe_AddRef(m_pMayTransform);

	//m_eNextState = UFO_LASER_MH;
	//m_pModelCom->Set_Animation(ANI_UFO_LASER_MH);
	//m_pModelCom->Set_NextAnimIndex(ANI_UFO_LASER_MH);

	// DataBase에 포인터 등록
	CDataStorage::GetInstance()->Set_UFOPtr(this);

	m_ePhase = UFO_PHASE::PHASE_1;

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	switch (m_ePhase)
	{
	case Client::CUFO::PHASE_1:
		Phase1_Pattern(dTimeDelta);
		break;
	case Client::CUFO::PHASE_2:
		break;
	case Client::CUFO::PHASE_3:
		break;
	case Client::CUFO::PHASE_4:
		break;
	}
	
	//Check_State(dTimeDelta);
	Change_State(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}


void CUFO::Change_State(_double dTimeDelta)
{

}

void CUFO::Laser_Pattern(_double dTimeDelta)
{
	_vector vDir = m_pCodyTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_vector vDirForLaser = XMVector3Normalize(vDir);

	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f); // 플레이어 쪽으로 천천히 회전.

}

void CUFO::Phase1_Pattern(_double dTimeDelta)
{
}

HRESULT CUFO::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}


void CUFO::Add_LerpInfo_To_Model()
{
	return;
}

HRESULT CUFO::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}