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


void CUFO::Laser_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	_vector vDir, vTargetPos;

	/* 지정된 타겟에 따라 포지션 세팅 */
	switch (m_eTarget)
	{
	case Client::CUFO::TARGET_CODY:
		vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);
		break;
	case Client::CUFO::TARGET_MAY:
		vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);
		break;
	}

	vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	/* 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f);
}

void CUFO::Phase1_Pattern(_double dTimeDelta)
{
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
//#define CutScene_Eject_FlyingSaucer 1
//#define CutScene_EnterUFO_FlyingSaucer 2
//#define CutScene_PowerCoresDestroyed_UFO 3
//#define CutScene_RocketPhaseFinished_FlyingSaucer 4
//#define CutScene_UFO_Boss_Intro 5
//#define CutScene_UFO_LaserRippedOff_FlyingSaucer 6
//#define CutScene_UFO_Outro 7
//#define UFO_Back 8
//#define UFO_CodyHolding 9
//#define UFO_CodyHolding_Enter 10
//#define UFO_CodyHolding_low 11
//#define UFO_Controllable_Additive 12
//#define UFO_Controllable_Additive_Boost 13
//#define UFO_Controllable_Additive_Flying 14
//#define UFO_Controllable_Pose_Bck 15
//#define UFO_Controllable_Pose_Fwd 16
//#define UFO_Controllable_Pose_Left 17
//#define UFO_Controllable_Pose_Right 18
//#define UFO_FireRocket_Additive_Left 19
//#define UFO_FireRocket_Additive_Right 20
//#define UFO_Fwd 21
//#define UFO_GroundPound 22
//#define UFO_HitReaction_Bck 23
//#define UFO_HitReaction_Fwd 24
//#define UFO_HitReaction_Left 25
//#define UFO_HitReaction_Right 26
//#define UFO_KnockDownMH 27
//#define UFO_LaserRippedOff 28
//#define UFO_Laser_HitPod 29
//#define UFO_Laser_MH 30
//#define UFO_Left 31
//#define UFO_MH 32
//#define UFO_Ref 33
//#define UFO_Right 34
//#define UFO_RocketKnockDown_MH 35
//
//
//	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Back, false);
//	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Fwd, false);
//	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_GroundPound, false);
//	m_pModelCom->Add_LerpInfo(UFO_MH, ANI_C_Bhv_Valve_Rotate_MH, false);
//	m_pModelCom->Add_LerpInfo(UFO_MH, ANI_C_Bhv_Valve_Rotate_MH, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_Jump_Land, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jump_Land, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Start, ANI_C_Bhv_GroundPound_Falling, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Start, ANI_C_Bhv_GroundPound_Land, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land, ANI_C_Bhv_GroundPound_Land_Exit, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land, ANI_C_ChangeSize_Walk_Large_Fwd, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Falling, ANI_C_Bhv_GroundPound_Land_Exit, true, 5.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_MH, true, 5.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_Jog_Start_Fwd, true, 2.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_GroundPound_Land_Exit, ANI_C_Jog, true, 2.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_Bhv_ChangeSize_GroundPound_Start, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Start, ANI_C_Bhv_ChangeSize_GroundPound_Falling, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Falling, ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit, false);
//
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_Sprint, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_ChangeSize_GroundPound_Land_Exit, ANI_C_MH, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Walk_Large_Fwd, ANI_C_ChangeSize_Walk_Large_Fwd, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_Stop, true, 60.f);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Stop, ANI_C_Roll_To_Jog, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Roll_Start, ANI_C_Roll_To_Jog, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_DoubleJump, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_AirDash_Start, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_DoubleJump, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land_Still_Jump, ANI_C_AirDash_Start, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_DoubleJump, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Bhv_GroundPound_Start, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Bhv_GroundPound_Start, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Sprint, ANI_C_SprintTurnAround, true, 20.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_SprintTurnAround, true, 20.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd_Exhausted, ANI_C_SprintTurnAround, true, 20.f);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_MH, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_MH, ANI_C_ChangeSize_Jump_Start, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Large_Land, ANI_C_MH, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_ChangeSize_Jump_Large_Land_Jog, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_ChangeSize_Jump_Start, ANI_C_ChangeSize_Jump_Large_Land, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_180L, true, 10.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_180R, true, 10.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_Bhv_PlayRoom_ZeroGravity_MH, ANI_C_Jump_Falling, true, 10.f);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Start_Fwd, ANI_C_Jump_Falling, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jog_Stop_Fwd, ANI_C_Jump_Falling, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jog, ANI_C_Jump_Falling, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jump_Land_Jog, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog_Start_Fwd, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog_Stop_Fwd, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_DoubleJump, ANI_C_Jog, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Start, ANI_C_Jump_Land_Jog, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jump_Land_Jog, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog_Start_Fwd, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog_Stop_Fwd, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Land, ANI_C_Jog, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Jump_Falling, ANI_C_Jump_Land, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_AirDash_Start, ANI_C_Jump_Land_Jog, false);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_MH, ANI_C_WallSlide_Jump, true, 20.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_Jump, ANI_C_WallSlide_Enter, true, 20.f);
//	m_pModelCom->Add_LerpInfo(ANI_C_WallSlide_Enter, ANI_C_WallSlide_MH, true, 20.f);
//
//	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_Enter, ANI_C_Grind_Grapple_ToGrind, false);
//	m_pModelCom->Add_LerpInfo(ANI_C_Grind_Grapple_ToGrind, ANI_C_Grind_Slow_MH, false);
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