#include "stdafx.h"
#include "..\public\UmbrellaBeam.h"
#include "Cody.h"
#include "May.h"
#include "DataStorage.h"
#include "UmbrellaBeam_Stand.h"
#include "UmbrellaBeam_Joystick.h"
#include "Effect_Umbrella_Pipe.h"
#include "UI_Generator.h"

CUmbrellaBeam::CUmbrellaBeam(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUmbrellaBeam::CUmbrellaBeam(const CUmbrellaBeam & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUmbrellaBeam::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUmbrellaBeam::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(20.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_UmbrellaBeam_Stand(TEXT("Layer_UmbrellaBeam_Stand")), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-789.319824f, 769.882971f, 189.852661f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Umbrella_Effect", Level::LEVEL_STAGE, TEXT("GameObject_3D_Umbrella_Pipe"), nullptr, (CGameObject**)&m_pUmbrellaBeam_Effect), E_FAIL);
	m_pUmbrellaBeam_Effect->Set_ParentWorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_UserData = USERDATA(GameID::eUMBRELLABEAM, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	m_fHorizontalAngle = 45.f;
	m_fVerticalAngle = 0.f;

	DATABASE->Set_UmbrellaBeam(this);
	return S_OK;
}

_int CUmbrellaBeam::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	KeyInput_Rotate(dTimeDelta);

	if(true == m_IsBeamActivate)
		m_pUmbrellaBeam_Effect->Set_ParentWorldMatrix(m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CUmbrellaBeam::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (true == m_IsBeamActivate)
		m_pUmbrellaBeam_Effect->Set_Activate(true);
	else
	{
		m_pUmbrellaBeam_Effect->Set_Activate(false);
		m_IsPutGravitationalField = false;
	}

	PutGravitationalField();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CUmbrellaBeam::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CUmbrellaBeam::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CUmbrellaBeam::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CUmbrellaBeam::Set_BeamActivate(_bool IsCheck)
{
	m_IsBeamActivate = IsCheck;
}

void CUmbrellaBeam::KeyInput_Rotate(_double TimeDelta)
{
	if (false == m_IsBeamActivate)
		return;

	_float fMaxHorizontalAngle = 95.f;
	_float fMaxVerticalAngle = 50.f;
	_float fRotationPerSec = XMConvertToDegrees((_float)m_pTransformCom->Get_RotationPerSec());
	CUmbrellaBeam_Joystick* pJoystick = (CUmbrellaBeam_Joystick*)DATABASE->Get_Umbrella_JoystickPtr();
	CCody* pCody = (CCody*)DATABASE->GetCody();

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (m_pGameInstance->Key_Pressing(DIK_W) && fMaxVerticalAngle >= m_fVerticalAngle)
	{
		m_fVerticalAngle += (_float)TimeDelta * fRotationPerSec;
		m_pTransformCom->Rotate_Axis(vRight, -TimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_S) && 0.f <= m_fVerticalAngle)
	{
		m_fVerticalAngle -= (_float)TimeDelta * fRotationPerSec;
		m_pTransformCom->Rotate_Axis(vRight, TimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_D) && fMaxHorizontalAngle >= m_fHorizontalAngle)
	{
		m_fHorizontalAngle += (_float)TimeDelta * fRotationPerSec;
		m_pTransformCom->Rotate_Axis(vUp, TimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_A) && 0.f <= m_fHorizontalAngle)
	{
		m_fHorizontalAngle -= (_float)TimeDelta * fRotationPerSec;
		m_pTransformCom->Rotate_Axis(vUp, -TimeDelta);
	}

	m_pUmbrellaBeam_Stand->Set_Rotate(m_fHorizontalAngle);
	pJoystick->Set_OnParentRotate(m_pUmbrellaBeam_Stand->Get_Transform()->Get_WorldMatrix());
	pCody->Set_OnParentRotate(pJoystick->Get_Transform()->Get_WorldMatrix());
}

void CUmbrellaBeam::PutGravitationalField()
{
	if (false == m_IsBeamActivate)
		return;

	CMay* pMay = (CMay*)DATABASE->GetMay();

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;
	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.z += 40.f;

	_matrix matWorld, matTrans, matParent;
	matParent = m_pTransformCom->Get_WorldMatrix();
	matTrans = XMMatrixTranslation(0.f, 0.f, 40.f);
	matWorld = matTrans * matParent;

	_vector vOffSetPos = XMLoadFloat4((_float4*)&matWorld.r[3].m128_f32[0]);
	_vector vTargetPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_IsPutGravitationalField = false;
		m_IsBeamActivate = false;
		pMay->Set_ActorGravity(false, false, false);

		CUmbrellaBeam_Joystick* pJoystick = (CUmbrellaBeam_Joystick*)DATABASE->Get_Umbrella_JoystickPtr();
		pJoystick->Set_ControlActivate(false);

		CCody* pCody = (CCody*)DATABASE->GetCody();
		pCody->Set_ControlJoystick(false);
		pCody->Get_Model()->Set_Animation(ANI_C_ActionMH);

		if(nullptr != m_pUmbrellaBeam_Effect)
			m_pUmbrellaBeam_Effect->Set_Dead();

		UI_Delete(Cody, InputButton_Cancle);
		UI_Delete(Cody, Arrowkeys_All);
	}

	if (true == m_IsPutGravitationalField)
	{
		//pMay->Get_Transform()->Set_WorldMatrix(matWorld);
		pMay->Get_Model()->Set_Animation(ANI_M_ZeroGravity_MH);
		pMay->Set_ActorPosition(vOffSetPos);
		pMay->Set_ActorGravity(true, false, true);
		return;
	}

	_vector vComparePos = vTargetPos - vOffSetPos;
	_float fDistance = XMVectorGetX(XMVector3Length(vComparePos));
	_float fRange = 3.f;

	if (fRange >= fDistance)
		m_IsPutGravitationalField = true;

}

HRESULT CUmbrellaBeam::Ready_Layer_UmbrellaBeam_Stand(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam_Stand"), nullptr, &pGameObject), E_FAIL);
	m_pUmbrellaBeam_Stand = static_cast<CUmbrellaBeam_Stand*>(pGameObject);
	
	return S_OK;
}

CUmbrellaBeam * CUmbrellaBeam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUmbrellaBeam* pInstance = new CUmbrellaBeam(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUmbrellaBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUmbrellaBeam::Clone_GameObject(void * pArg)
{
	CUmbrellaBeam* pInstance = new CUmbrellaBeam(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUmbrellaBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUmbrellaBeam::Free()
{
	Safe_Release(m_pUmbrellaBeam_Effect);

	Safe_Release(m_pUmbrellaBeam_Stand);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
