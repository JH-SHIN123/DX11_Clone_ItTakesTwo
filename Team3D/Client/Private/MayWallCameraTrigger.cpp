#include "stdafx.h"
#include "MayWallCameraTrigger.h"
#include "RobotParts.h"
#include "Cody.h"
#include "May.h"

CMayWallCameraTrigger::CMayWallCameraTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMayWallCameraTrigger::CMayWallCameraTrigger(const CMayWallCameraTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMayWallCameraTrigger::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMayWallCameraTrigger::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);

	ROBOTDESC CameraTriggerDesc;
	if (nullptr != pArg)
		memcpy(&CameraTriggerDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));


	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CameraTriggerDesc.vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_UserData.eID = GameID::eMAYWALLCAMERATRIGGER;
	m_UserData.pGameObject = this;

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pGeometry = new PxBoxGeometry(15.f, 3.f, 8.f);
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

_int CMayWallCameraTrigger::Tick(_double dTimeDelta)
{
	_int i = 0;
	return _int();
}

void CMayWallCameraTrigger::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	// Cody
	_matrix CameraMatrix = XMMatrixIdentity();
	_vector vRight = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	_vector vUp = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector vLook = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	_vector vPosition = XMVectorSet(-829.936523f, 780.056213f, 193.710083f, 1.f);

	CameraMatrix.r[0] = vRight;
	CameraMatrix.r[1] = vUp;
	CameraMatrix.r[2] = vLook;
	CameraMatrix.r[3] = vPosition;

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::eMAYWALLCAMERATRIGGER, true, CameraMatrix);
		((CMay*)pGameObject)->SetCameraTriggerID_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetCameraTriggerID_Matrix(GameID::Enum::eMAYWALLCAMERATRIGGER, false, CameraMatrix);
		m_IsCollide = false;
	}
	
}

CMayWallCameraTrigger* CMayWallCameraTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CMayWallCameraTrigger* pInstance = new CMayWallCameraTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CMayWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMayWallCameraTrigger::Clone_GameObject(void * pArg)
{
	CMayWallCameraTrigger* pInstance = new CMayWallCameraTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMayWallCameraTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMayWallCameraTrigger::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
