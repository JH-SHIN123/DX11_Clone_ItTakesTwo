#include "stdafx.h"
#include "..\Public\RotationBox.h"
#include "Cody.h"

CRotationBox::CRotationBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CRotationBox::CRotationBox(const CRotationBox & rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CRotationBox::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotationBox::NativeConstruct(void * pArg)
{
	CDynamic_Env::NativeConstruct(pArg);

	m_UserData.eID = GameID::eROTATIONBOX;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int CRotationBox::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	Movement(dTimeDelta);

	return NO_EVENT;
}

_int CRotationBox::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	_vector vScale, vRotQuat, vPosition;
	XMMatrixDecompose(&vScale, &vRotQuat, &vPosition, m_pTransformCom->Get_WorldMatrix());
	(m_pDynamicActorCom->Get_Actor())->setGlobalPose(MH_PxTransform(vRotQuat, vPosition));

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CRotationBox::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMaterialIndex);

	return S_OK;
}

HRESULT CRotationBox::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRotationBox::Movement(_double dTimeDelta)
{
	Rotate_Angle(5.0, 5.0, dTimeDelta);
	Rotate_Random(5.0, dTimeDelta);
}

void CRotationBox::Rotate_Angle(_double fMaxAngle, _double dSpeed, _double dTimeDelta)
{
	if (true == m_bRotate_Random)
		return;

	if (true == m_bRotate_Start)
		fMaxAngle *= 2.0;

	m_dCoolTime += dTimeDelta;
	if (2.0 <= m_dCoolTime)
	{
		if (true == m_bDirChange)
			m_pTransformCom->RotatePitch_Angle(m_dAngle - (fMaxAngle * 0.5));
		else
			m_pTransformCom->RotatePitch_Angle((fMaxAngle * 0.5) - m_dAngle);

		m_bRotate_Random = true;
		m_dCoolTime = 0.0;
		m_dAngle = 0.0;
		m_iRandom = rand() % 2;
		return;
	}

	_double dAngle = (dTimeDelta * (dSpeed * fMaxAngle));
	if (fMaxAngle <= m_dAngle + dAngle)
	{
		m_bDirChange = !m_bDirChange;
		dAngle = fMaxAngle - m_dAngle;
		m_dAngle = 0.0;
		m_bRotate_Start = true; 
	}
	else
		m_dAngle += dAngle;
		
	if (true == m_bDirChange)
		m_pTransformCom->RotatePitch_Angle(-dAngle);
	else
		m_pTransformCom->RotatePitch_Angle(dAngle);
}

void CRotationBox::Rotate_Random(_double dSpeed, _double dTimeDelta)
{
	if (false == m_bRotate_Random)
		return;

	_double dAngle = dTimeDelta * (90.0 * dSpeed);
	if (m_dAngle + dAngle >= 90.0)
	{
		dAngle = 90.0 - m_dAngle;
		m_bDirChange = true;
		m_bRotate_Start = false;
		m_bRotate_Random = false;
		m_dAngle = 0;
		Rotate_Fix();
		return;
	}
	else
		m_dAngle += dAngle;

	if (0 == m_iRandom)
		m_pTransformCom->RotatePitch_Angle(dAngle);
	else
		m_pTransformCom->RotatePitch_Angle(-dAngle);
}

void CRotationBox::Rotate_Fix()
{
	_vector vecFix[4] = {};

	vecFix[0] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vecFix[1] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	vecFix[2] = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	vecFix[3] = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	if (XMVectorGetY(vUp) > 0.9f)
	{
		_vector vTempUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vTempLook = XMVector3Cross(vRight, vTempUp);

		m_pTransformCom->Set_State(CTransform::STATE_UP, vTempUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTempLook);
	}
	else if (XMVectorGetY(vUp) < -0.9f)
	{
		_vector vTempUp = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		_vector vTempLook = XMVector3Cross(vRight, vTempUp);

		m_pTransformCom->Set_State(CTransform::STATE_UP, vTempUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTempLook);
	}
	else if (XMVectorGetY(vLook) > 0.9f)
	{
		_vector vTempLook = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vTempUp = XMVector3Cross(vTempLook, vRight);

		m_pTransformCom->Set_State(CTransform::STATE_UP, vTempUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTempLook);
	}
	else if (XMVectorGetY(vLook) < -0.9f)
	{
		_vector vTempLook = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		_vector vTempUp = XMVector3Cross(vTempLook, vRight);

		m_pTransformCom->Set_State(CTransform::STATE_UP, vTempUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTempLook);
	}
}

HRESULT CRotationBox::Ready_Component(void * pArg)
{
	/* Dynamic */
	PxGeometry* Geom = new PxBoxGeometry(0.5f, 0.25f, 0.25f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);

	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	return S_OK;
}

CRotationBox * CRotationBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotationBox* pInstance = new CRotationBox(pDevice, pDeviceContext);

	if (pInstance->NativeConstruct_Prototype())
	{
		MSG_BOX("Failed to Create Instance - CRotationBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRotationBox::Clone_GameObject(void * pArg)
{
	CRotationBox* pInstance = new CRotationBox(*this);

	if (pInstance->NativeConstruct(pArg))
	{
		MSG_BOX("Failed to Clone Instance - CRotationBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotationBox::Free()
{
	Safe_Release(m_pDynamicActorCom);

	CDynamic_Env::Free();
}
