#include "stdafx.h"
#include "..\Public\PlayerActor.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerFilterCallback.h"
#include "PlayerHitReport.h"

CPlayerActor::CPlayerActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CPlayerActor::CPlayerActor(const CPlayerActor & rhs)
	: CActor(rhs)
{
}

void CPlayerActor::Set_Scale(_float fRadius, _float fHeight)
{
	m_pController->resize(fHeight);
	static_cast<PxCapsuleController*>(m_pController)->setRadius(fRadius);
}

void CPlayerActor::Set_Position(_fvector vPosition)
{
	m_pController->setFootPosition(MH_PxExtendedVec3(vPosition));
}

HRESULT CPlayerActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlayerActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

	m_pTransform = ArgDesc.pTransform;
	NULL_CHECK_RETURN(m_pTransform, E_FAIL);
	Safe_AddRef(m_pTransform);

	m_pBehaviorCallback = new CPlayerBehaviorCallback;
	m_pFilterCallback = new CPlayerFilterCallback;
	m_pHitReport = new CPlayerHitReport;
	m_pFilters = new PxControllerFilters(0, 0, m_pFilterCallback);

	ArgDesc.CapsuleControllerDesc.behaviorCallback = m_pBehaviorCallback;
	ArgDesc.CapsuleControllerDesc.reportCallback = m_pHitReport;

	m_pController = m_pPhysX->Create_CapsuleController(ArgDesc.CapsuleControllerDesc);

	m_pBehaviorCallback->Set_PlayerActorPtr(this);
	m_pBehaviorCallback->Set_Controller(m_pController);
	m_pHitReport->Set_PlayerActorPtr(this);

	m_pActor = m_pController->getActor();
	m_pController->setUserData(ArgDesc.pUserData);

	m_pUserData = ArgDesc.pUserData;
	m_pActor->userData = ArgDesc.pUserData;

	m_isGravityReordered = true;

	XMStoreFloat3(&m_vPlayerUp, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	return S_OK;
}

void CPlayerActor::Move(_fvector vMove, _double dTimeDelta)
{
	//_vector vTemp = XMVector3TransformCoord(vMove, MH_GetQuaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vPlayerUp)));
	m_pController->move(MH_PxVec3(vMove), 0.001f, (_float)dTimeDelta, *m_pFilters);
}

void CPlayerActor::Update(_double dTimeDelta)
{
	if (m_bZeroGravity == true)
	{
		if (true == m_bStatic)
		{
			m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
		}
		else
		{
			if (true == m_IsGoUp)
			{
				//PxVec3 vDist = MH_PxVec3(m_vPlayerUp, (_float)(dTimeDelta * 4.f));
				PxVec3 vDist = { 0.f, (_float)(dTimeDelta * 4.f), 0.f };
				m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());
				m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
			}
			else
			{
				//PxVec3 vDist = MH_PxVec3(m_vPlayerUp, (_float)(-dTimeDelta * 4.f));
				PxVec3 vDist = { 0.f, (_float)(-dTimeDelta * 4.f), 0.f };
				m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());
				m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
			}
		}
	}
	else
	{
		m_fHeightDelta = Get_Height(dTimeDelta);
		_float fY;

		if (m_fHeightDelta != 0.f)
			fY = m_fHeightDelta * 0.5f;
		else
			fY = m_fGravity * (_float)dTimeDelta;

		//PxVec3 vDist = PxVec3(0, fY, 0);
		PxVec3 vDist = MH_PxVec3(m_vPlayerUp, fY);
		PxU32 iFlags = m_pController->move(vDist, 0.f, (_float)dTimeDelta, *m_pFilters);

		if (PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags)
		{
			m_fFallingTime = 0.f;
			m_IsFalling = false;
			Jump_Stop();
		}
		else if (!(PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags) && !m_bJump && !m_bGroundPound && !m_bZeroGravity)
		{
			m_IsFalling = true;
			m_fFallingTime += (_float)dTimeDelta;
			// 자유낙하
			//vDist = PxVec3(0, (0.4f * m_fGravity * 0.8f * m_fFallingTime * m_fFallingTime), 0);
			vDist = MH_PxVec3(m_vPlayerUp, (0.4f * m_fGravity * 0.8f * m_fFallingTime * m_fFallingTime));
			m_pController->move(vDist, 0.f, (_float)dTimeDelta, *m_pFilters);
		}

		//m_pTransform->Set_RotateQuat(MH_GetQuaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vPlayerUp)));
		m_pTransform->RotateByUp(XMLoadFloat3(&m_vPlayerUp));
		m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
	}

	if (m_iReorderGravityStep == 1)
	{
		Step_GravityPath(m_vHitNormal);
		m_iReorderGravityStep = 2;
	}
	else if (m_iReorderGravityStep == 2)
	{
		Reorder_Gravity();
		m_iReorderGravityStep = 0;
	}
}

void CPlayerActor::Jump_Start(_float fJumpForce)
{
	m_fJumpTime = 0.f;
	m_fJumpForce = fJumpForce;
	m_fBaseJumpForce = fJumpForce;
	m_bJump = true;
}

void CPlayerActor::Jump_Higher(_float fJumpForce)
{
	if (!m_bJump || m_fHeightDelta <= 0.f)
		return;

	if (m_fBaseJumpForce * 1.2f < m_fJumpForce)
		return;

	m_fJumpForce += fJumpForce;
}

void CPlayerActor::Step_GravityPath(PxVec3 vNormal)
{
	NULL_CHECK(m_pController);


	//if (m_pUserData->eID == GameID::eCODY) return;

	m_pController->setUpDirection(vNormal);
	m_isGravityReordered = false;

	m_vPlayerUp = MH_XMFloat3(vNormal);

	//m_pTransform->Set_RotateQuat(MH_GetQuaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vPlayerUp)));
	m_pTransform->RotateByUp(XMLoadFloat3(&m_vPlayerUp));
	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));

	//디버그
	//static _uint iBuffer = 100;
	//++iBuffer;
	//if (iBuffer > 100)
	//	iBuffer = 0;
}

void CPlayerActor::Reorder_Gravity()
{
	if (m_pUserData->eID == GameID::eCODY) return;

	if (true == m_isGravityReordered) return;

	_float fX = abs(m_vPlayerUp.x);
	_float fY = abs(m_vPlayerUp.y);
	_float fZ = abs(m_vPlayerUp.z);

	_float fMax = fmax(fmax(fX, fY), fZ);

	if (fX == fMax)
	{
		if (m_vPlayerUp.x > 0.f)
			m_vPlayerUp = _float3(1.f, 0.f, 0.f);
		else
			m_vPlayerUp = _float3(-1.f, 0.f, 0.f);
	}
	else if (fY == fMax)
	{
		if (m_vPlayerUp.y > 0.f)
			m_vPlayerUp = _float3(0.f, 1.f, 0.f);
		else
			m_vPlayerUp = _float3(0.f, -1.f, 0.f);
	}
	else if (fZ == fMax)
	{
		if (m_vPlayerUp.z > 0.f)
			m_vPlayerUp = _float3(0.f, 0.f, 1.f);
		else
			m_vPlayerUp = _float3(0.f, 0.f, -1.f);
	}

	m_pController->setUpDirection(MH_PxVec3(m_vPlayerUp));
	m_isGravityReordered = true;

	//m_vPlayerUp;
	//_vector vTemp = XMVector3TransformCoord(XMVectorSet(-0.5f, 0.1f, 0.1f, 1.f), MH_GetQuaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vPlayerUp)));

	//m_pTransform->Set_RotateQuat(MH_GetQuaternion(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vPlayerUp)));
	m_pTransform->RotateByUp(XMLoadFloat3(&m_vPlayerUp));
	m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
}

void CPlayerActor::MoveToTarget(PxTransform PxTransform)
{
	m_pActor->setKinematicTarget(PxTransform);
}

void CPlayerActor::Jump_Stop()
{
	if (!m_bJump) return;

	m_bJump = false;
}

_float CPlayerActor::Get_Height(_double dTimeDelta)
{
	if (!m_bJump) return 0.f;

	m_fJumpTime += (_float)dTimeDelta;

	return (m_fGravity / 2.f * m_fJumpTime * m_fJumpTime + m_fJumpForce * m_fJumpTime)/* * (_float)dTimeDelta*/;
}

CPlayerActor * CPlayerActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerActor* pInstance = new CPlayerActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlayerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CPlayerActor::Clone_Component(void * pArg)
{
	CPlayerActor* pInstance = new CPlayerActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlayerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerActor::Free()
{
	if (true == m_isClone)
	{
		Safe_Delete(m_pFilters);
		Safe_Delete(m_pHitReport);
		Safe_Delete(m_pFilterCallback);
		Safe_Delete(m_pBehaviorCallback);
		m_pPhysX->Remove_Actor(&m_pTrigger);
		m_pController->release();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}


/*

#include "stdafx.h"
#include "..\Public\PlayerActor.h"
#include "PlayerBehaviorCallback.h"
#include "PlayerFilterCallback.h"

CPlayerActor::CPlayerActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
: CActor(pDevice, pDeviceContext)
{
}

CPlayerActor::CPlayerActor(const CPlayerActor & rhs)
: CActor(rhs)
{
}

void CPlayerActor::Set_Scale(_float fRadius, _float fHeight)
{
m_pController->resize(fHeight);
static_cast<PxCapsuleController*>(m_pController)->setRadius(fRadius);
}

void CPlayerActor::Set_Position(_fvector vPosition)
{
m_pController->setPosition(MH_PxExtendedVec3(vPosition));
}

HRESULT CPlayerActor::NativeConstruct_Prototype()
{
CActor::NativeConstruct_Prototype();

return S_OK;
}

HRESULT CPlayerActor::NativeConstruct(void * pArg)
{
CActor::NativeConstruct(pArg);

NULL_CHECK_RETURN(pArg, E_FAIL);

ARG_DESC ArgDesc = *static_cast<ARG_DESC*>(pArg);

m_pTransform = ArgDesc.pTransform;
NULL_CHECK_RETURN(m_pTransform, E_FAIL);
Safe_AddRef(m_pTransform);

m_pBehaviorCallback = new CPlayerBehaviorCallback;
m_pFilterCallback = new CPlayerFilterCallback;
m_pFilters = new PxControllerFilters(0, 0, m_pFilterCallback);
ArgDesc.CapsuleControllerDesc.behaviorCallback = m_pBehaviorCallback;

m_pController = m_pPhysX->Create_CapsuleController(ArgDesc.CapsuleControllerDesc);

m_pBehaviorCallback->Set_Controller(m_pController);
m_pActor = m_pController->getActor();
m_pActor->userData = ArgDesc.pUserData;

//m_fJumpGravity = ArgDesc.fJumpGravity;

return S_OK;
}

void CPlayerActor::Move(_fvector vMove, _double dTimeDelta)
{
m_pController->move(MH_PxVec3(vMove), 0.001f, (_float)dTimeDelta, *m_pFilters);
}

void CPlayerActor::Update(_double dTimeDelta)
{
if (m_bZeroGravity == true && m_bStatic == true)
{
m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
return;
}

else if (m_bZeroGravity == true && m_IsGoUp == true)
{
PxVec3 vDist = { 0.f, (_float)(dTimeDelta * 4.f), 0.f };
m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());
m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
return;
}
else if (m_bZeroGravity == true && m_IsGoUp == false)
{
PxVec3 vDist = { 0.f, (_float)(-dTimeDelta * 4.f), 0.f };
m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());
m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
return;
}
else
{
m_fHeightDelta = Get_Height(dTimeDelta);
_float fY;

if (m_fHeightDelta != 0.f)
{
fY = m_fHeightDelta * 0.5f;
}
else
{
fY = m_fGravity * (_float)dTimeDelta;
}

PxVec3 vDist = PxVec3(0, fY, 0);
PxU32 iFlags = m_pController->move(vDist, 0.f, (_float)dTimeDelta, *m_pFilters);

if (PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags)
{
m_fFallingTime = 0.f;
m_IsFalling = false;
Jump_Stop();
}
else if (!(PxControllerCollisionFlag::eCOLLISION_DOWN & iFlags) && !m_bJump && !m_bGroundPound && !m_bZeroGravity)
{
m_IsFalling = true;
m_fFallingTime += (_float)dTimeDelta;
// 자유낙하
vDist = PxVec3(0, (0.4f * m_fGravity * 0.8f * m_fFallingTime * m_fFallingTime), 0);
m_pController->move(vDist, 0.f, (_float)dTimeDelta, PxControllerFilters());
}

m_pTransform->Set_State(CTransform::STATE_POSITION, MH_ConvertToXMVector(m_pController->getFootPosition(), 1.f));
}
}

void CPlayerActor::Jump_Start(_float fJumpForce)
{

m_fJumpTime = 0.f;
m_fJumpForce = fJumpForce;
m_fBaseJumpForce = fJumpForce;
m_bJump = true;
}

void CPlayerActor::Jump_Higher(_float fJumpForce)
{
	if (!m_bJump || m_fHeightDelta <= 0.f)
		return;

	if (m_fBaseJumpForce * 1.2f < m_fJumpForce)
		return;

	m_fJumpForce += fJumpForce;
}

void CPlayerActor::Jump_Stop()
{
	if (!m_bJump)
		return;

	m_bJump = false;
}

_float CPlayerActor::Get_Height(_double dTimeDelta)
{
	if (!m_bJump)
		return 0.f;

	m_fJumpTime += (_float)dTimeDelta;
	return (m_fGravity / 2.f * m_fJumpTime * m_fJumpTime + m_fJumpForce * m_fJumpTime);
}

CPlayerActor * CPlayerActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerActor* pInstance = new CPlayerActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlayerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CPlayerActor::Clone_Component(void * pArg)
{
	CPlayerActor* pInstance = new CPlayerActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlayerActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerActor::Free()
{
	if (true == m_isClone)
	{
		Safe_Delete(m_pFilters);
		Safe_Delete(m_pFilterCallback);
		Safe_Delete(m_pBehaviorCallback);
		m_pPhysX->Remove_Actor(&m_pTrigger);
		m_pController->release();
	}

	Safe_Release(m_pTransform);

	CActor::Free();
}

*/