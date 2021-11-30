#include "stdafx.h"
#include "..\public\SecurityCamera.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"

CSecurityCamera::CSecurityCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSecurityCamera::CSecurityCamera(const CSecurityCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSecurityCamera::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSecurityCamera::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SecurityCamera"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	ROBOTDESC SecurityCameraDesc;
	if (nullptr != pArg)
		memcpy(&SecurityCameraDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, SecurityCameraDesc.vPosition);

	return S_OK;
}

_int CSecurityCamera::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);


	Find_Target(dTimeDelta);

	if(nullptr != m_pTargetTransform)
		Watch_Target(dTimeDelta);



	return NO_EVENT;
}

_int CSecurityCamera::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CSecurityCamera::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CSecurityCamera::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CSecurityCamera::Find_Target(_double dTimeDelta)
{
	CGameObject* pCody = DATABASE->GetCody();
	if (nullptr == pCody)
		return;

	CGameObject* pMay = DATABASE->GetMay();
	if (nullptr == pMay)
		return;

	_vector ToCodyDir = ((CCody*)pCody)->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector ToMayDir = ((CMay*)pMay)->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float CodyDist = XMVectorGetX(XMVector3Length(ToCodyDir));
	_float MayDist = XMVectorGetX(XMVector3Length(ToMayDir));

	if (CodyDist < MayDist)
		m_pTargetTransform = (CTransform*)(dynamic_cast<CCody*>(DATABASE->GetCody()))->Get_Transform();
	else if (CodyDist > MayDist)
		m_pTargetTransform = (CTransform*)(dynamic_cast<CMay*>(DATABASE->GetMay()))->Get_Transform();;

}
 
void CSecurityCamera::Watch_Target(_double dTimeDelta)
{
	_vector TargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Rotate_ToTarget(TargetPos);
	//_vector vDir = TargetPos -  m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//vDir = XMVector3Normalize(vDir,);
	
	//_vector DirHorizon = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	//_vector DirVertical = XMVector3Normalize(XMVectorSetZ(XMVectorSetX(vDir, 0.f), 0.f));

	//m_pTransformCom->RotateYawDirectionOnLand(DirHorizon, dTimeDelta);
	//m_pTransformCom->RotatePitchDirectionOnLand(DirVertical, dTimeDelta);


}

CSecurityCamera * CSecurityCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSecurityCamera* pInstance = new CSecurityCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSecurityCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSecurityCamera::Clone_GameObject(void * pArg)
{
	CSecurityCamera* pInstance = new CSecurityCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSecurityCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSecurityCamera::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}