#include "stdafx.h"
#include "PipeWallCameraTrigger.h"
#include "RobotParts.h"
#include "Cody.h"
#include "May.h"

CPipeWallCameraTrigger::CPipeWallCameraTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPipeWallCameraTrigger::CPipeWallCameraTrigger(const CPipeWallCameraTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPipeWallCameraTrigger::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPipeWallCameraTrigger::NativeConstruct(void* pArg)
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

	m_UserData.eID = GameID::ePIPEWALLCAMERATRIGGER;
	m_UserData.pGameObject = this;

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pGeometry = new PxBoxGeometry(1.f, /*3.f*/6.f, 1.f);
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

_int CPipeWallCameraTrigger::Tick(_double dTimeDelta)
{
	// +Z 할수록 뒤쪽
	// +X 할수록 오른쪽
	//_vector vPosition = { -815.311f, 775.083f, 193.72f, 1.f };
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	//m_pTriggerActorCom->Update_TriggerActor();
	return _int();
}

void CPipeWallCameraTrigger::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	// Cody
	_matrix CameraMatrix = XMMatrixIdentity();
	_vector vRight =	XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector vUp =		XMVectorSet(0.f, 1.f, 0.f, 0.f); 
	_vector vLook =		XMVectorSet(0.f, 0.f,-1.f, 0.f);
	_vector vPosition = XMVectorSet(45.4295120f, 221.562866f, 222.605621f, 1.f);

	CameraMatrix.r[0] = vRight;
	CameraMatrix.r[1] = vUp;
	CameraMatrix.r[2] = vLook;
	CameraMatrix.r[3] = vPosition;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::ePIPEWALLCAMERATRIGGER, true, CameraMatrix);
		((CCody*)pGameObject)->SetCameraTriggerID_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::ePIPEWALLCAMERATRIGGER, false, CameraMatrix);
		m_IsCollide = false;
	}
	
}

CPipeWallCameraTrigger* CPipeWallCameraTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CPipeWallCameraTrigger* pInstance = new CPipeWallCameraTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CPipeWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPipeWallCameraTrigger::Clone_GameObject(void * pArg)
{
	CPipeWallCameraTrigger* pInstance = new CPipeWallCameraTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPipeWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeWallCameraTrigger::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
