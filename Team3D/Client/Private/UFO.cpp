#include "stdafx.h"
#include "..\Public\UFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"
#include "CutScenePlayer.h"

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

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	Add_LerpInfo_To_Model();

	ROBOTDESC UFODesc;

	if (nullptr != pArg)
		memcpy(&UFODesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, UFODesc.vPosition);

	m_pModelCom->Set_Animation(UFO_Fwd);
	m_pModelCom->Set_NextAnimIndex(UFO_Laser_MH);

	DATABASE->Set_UFOPtr(this);

	/* 초반 상태들 세팅 */
	m_ePhase = UFO_PHASE::PHASE_1;
	m_eTarget = UFO_TARGET::TARGET_MAY;
	m_IsCutScene = true;

	/* 컷 신 끝나고 기본 위치로 이동해야되는 포지션 세팅 */
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += 5.f;
	XMStoreFloat4(&m_vStartTargetPos, vPos);

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
		m_IsCutScene = false;

	m_pModelCom->Update_Animation(dTimeDelta);


	/* 컷 신 재생중이 아니라면 보스 패턴 진행하자 나중에 컷 신 생기면 바꿈 */
	if (false == m_IsCutScene)
	{
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
	}

	_uint test = m_pModelCom->Get_CurAnimIndex();

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}


void CUFO::Laser_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	_vector vDir, vTargetPos, vTargetLook;

	/* 지정된 타겟에 따라 포지션과 룩 세팅 */
	switch (m_eTarget)
	{
	case Client::CUFO::TARGET_CODY:
		vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);
		vTargetLook = m_pCodyTransform->Get_State(CTransform::STATE_LOOK);
		break;
	case Client::CUFO::TARGET_MAY:
		vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);
		vTargetLook = m_pMayTransform->Get_State(CTransform::STATE_LOOK);
		break;
	}

	vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f);

	_matrix matLaserGun = m_pModelCom->Get_BoneMatrix("LaserGun");

	/* 레이저건의 포지션을 받아오자*/
	_vector vLaserGunPos = XMLoadFloat4((_float4*)&matLaserGun.r[3].m128_f32[0]);

	/* 레이저 건에서 타겟의 방향 벡터를 구하자 */
	_vector vLaserDir = vTargetPos - vLaserGunPos;

	/* 레이저건 뼈의 Look을 받아오자 */
	_vector vLaserGunLook = XMLoadFloat4((_float4*)&matLaserGun.r[2].m128_f32[0]);

	_vector vUFOLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	/* 레이저건의 Look과 위에서 구한 방향 벡터를 내적해서 각도를 구해주자 */
	_vector vDot = XMVector3AngleBetweenVectors(vUFOLook, vLaserDir);
	_float fRadian = XMVectorGetX(vDot);
	_float fAngle = XMConvertToDegrees(fRadian);

	_matrix matRotX;

	if (1.f <= fAngle)
	{
		matRotX = XMMatrixRotationX(fAngle);

		matLaserGun = matRotX * matLaserGun;
		m_pModelCom->Set_BoneMatrix("LaserGun", matLaserGun);
	}
}

void CUFO::Phase1_Pattern(_double dTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4(&m_vStartTargetPos);
	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);\

	/* 처음에 저장해둔 타겟 포스의 Y위치까지 천천히 위로 이동해라. */
	if (vTargetPos.m128_f32[1] >= vUFOPos.m128_f32[1])
		m_pTransformCom->Go_Up(dTimeDelta / 4.f);

	Laser_Pattern(dTimeDelta);
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
	//CutScene_Eject_FlyingSaucer 1
	//CutScene_EnterUFO_FlyingSaucer 2
	//CutScene_PowerCoresDestroyed_UFO 3
	//CutScene_RocketPhaseFinished_FlyingSaucer 4
	//CutScene_UFO_Boss_Intro 5
	//CutScene_UFO_LaserRippedOff_FlyingSaucer 6
	//CutScene_UFO_Outro 7
	//UFO_Back 8
	//UFO_CodyHolding 9
	//UFO_CodyHolding_Enter 10
	//UFO_CodyHolding_low 11
	//UFO_Controllable_Additive 12
	//UFO_Controllable_Additive_Boost 13
	//UFO_Controllable_Additive_Flying 14
	//UFO_Controllable_Pose_Bck 15
	//UFO_Controllable_Pose_Fwd 16
	//UFO_Controllable_Pose_Left 17
	//UFO_Controllable_Pose_Right 18
	//UFO_FireRocket_Additive_Left 19
	//UFO_FireRocket_Additive_Right 20
	//UFO_Fwd 21
	//UFO_GroundPound 22
	//UFO_HitReaction_Bck 23
	//UFO_HitReaction_Fwd 24
	//UFO_HitReaction_Left 25
	//UFO_HitReaction_Right 26
	//UFO_KnockDownMH 27
	//UFO_LaserRippedOff 28
	//UFO_Laser_HitPod 29
	//UFO_Laser_MH 30
	//UFO_Left 31
	//UFO_MH 32
	//UFO_Ref 33
	//UFO_Right 34
	//UFO_RocketKnockDown_MH 35

	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Back, false);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Fwd, false);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Laser_MH, false);
	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_Laser_MH, false);

}

HRESULT CUFO::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pCodyTransform = ((CCody*)DATABASE->GetCody())->Get_Transform();
	NULL_CHECK_RETURN(m_pCodyTransform, E_FAIL);
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)DATABASE->GetMay())->Get_Transform();
	NULL_CHECK_RETURN(m_pMayTransform, E_FAIL);
	Safe_AddRef(m_pMayTransform);

	return S_OK;
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