#include "stdafx.h"
#include "DummyWallCameraTrigger.h"
#include "RobotParts.h"
#include "Cody.h"
#include "May.h"

CDummyWallCameraTrigger::CDummyWallCameraTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CDummyWallCameraTrigger::CDummyWallCameraTrigger(const CDummyWallCameraTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyWallCameraTrigger::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDummyWallCameraTrigger::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);

	ROBOTDESC CameraTriggerDesc;
	if (nullptr != pArg)
		memcpy(&CameraTriggerDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	//CameraTriggerDesc.vPosition.m128_f32[0] -= 10.f;
	//CameraTriggerDesc.vPosition.m128_f32[1] += 10.f;
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraTriggerDesc.vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_UserData.eID = GameID::eDUMMYWALLCAMERATRIGGER;
	m_UserData.pGameObject = this;

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pGeometry = new PxBoxGeometry(3.f, 15.f, 8.f);
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

_int CDummyWallCameraTrigger::Tick(_double dTimeDelta)
{
	// +X 할수록 뒤쪽
	// +Z 할수록 오른쪽
	//_vector vPosition = { -815.311f, 775.083f, 193.72f, 1.f };
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	//m_pTriggerActorCom->Update_TriggerActor();
	return _int();
}

void CDummyWallCameraTrigger::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	// Cody
	_matrix CameraMatrix = XMMatrixIdentity();
	_vector vRight = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f); 
	_vector vLook = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	_vector vPosition = XMVectorSet(-792.152222f, 768.998901f + 2.f, 193.549713f, 1.f);

	CameraMatrix.r[0] = vRight;
	CameraMatrix.r[1] = vUp;
	CameraMatrix.r[2] = vLook;
	CameraMatrix.r[3] = vPosition;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::eDUMMYWALLCAMERATRIGGER, true, CameraMatrix);
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY) 
	{
		((CCody*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::eDUMMYWALLCAMERATRIGGER, false, CameraMatrix);
		m_IsCollide = false;
	}
	
}

CDummyWallCameraTrigger* CDummyWallCameraTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CDummyWallCameraTrigger* pInstance = new CDummyWallCameraTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CDummyWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummyWallCameraTrigger::Clone_GameObject(void * pArg)
{
	CDummyWallCameraTrigger* pInstance = new CDummyWallCameraTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDummyWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummyWallCameraTrigger::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
