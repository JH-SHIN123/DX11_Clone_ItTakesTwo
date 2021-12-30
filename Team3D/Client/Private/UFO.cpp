#include "stdafx.h"
#include "..\Public\UFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"
#include "CutScenePlayer.h"
#include "MoonBaboon_MainLaser.h"
#include "MoonBaboon_SubLaser.h"
#include "Laser_TypeA.h"
#include "Boss_Missile.h"
#include "MoonBaboon.h"
#include "Effect_Generator.h"
#include "BossHpBar.h"
#include "MoonUFO.h"
#include "HpBar.h"
#include "MoonBaboonCore.h"
#include "BossDoor.h"
#include "UI_Generator.h"
#include "MainCamera.h"
#include "SubCamera.h"
#include "Script.h"

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
	FAILED_CHECK_RETURN(Ready_UI(), E_FAIL);

	Add_LerpInfo_To_Model();

	ROBOTDESC UFODesc;

	if (nullptr != pArg)
		memcpy(&UFODesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, UFODesc.vPosition);

	m_pModelCom->Set_Animation(UFO_Fwd);
	m_pModelCom->Set_NextAnimIndex(UFO_Laser_MH);

	/* 초반 상태들 세팅 */
	m_ePhase = UFO_PHASE::PHASE_1;
	m_eTarget = UFO_TARGET::TARGET_MAY;
	m_ePattern = UFO_PATTERN::LASER;
	m_IsCutScene = true;

	/* 컷 신 끝나고 기본 위치로 이동해야되는 포지션 세팅 */
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += 6.f;
	XMStoreFloat4(&m_vStartUFOPos, vPos);
	m_IsStartingPointMove = true;

	Set_MeshRenderGroup();

	DATABASE->Set_BossUFO(this);

	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying, m_pTransformCom->Get_WorldMatrix());
	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying_Particle, m_pTransformCom->Get_WorldMatrix());
	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying_Particle_Flow, m_pTransformCom->Get_WorldMatrix());

	// Light
	LIGHT_DESC lightDesc;
	lightDesc.eType = LIGHT_DESC::TYPE_POINT;
	lightDesc.fRange = 15.f;
	lightDesc.vDiffuse = { 0.f,0.f,1.f,1.f };
	lightDesc.vSpecular = { 0.f,0.f,1.f,1.f };
	XMStoreFloat3(&lightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pBossLight = CLight::Create(TEXT("Boss_UFO_Light"),&lightDesc);
	m_pGameInstance->Add_Light(LightStatus::eDYNAMIC, m_pBossLight);
	//Safe_AddRef(m_pBossLight);

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		m_IsActive = true;
	}
	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_IsCutScene = false;
		DATABASE->Close_BossDoor();
		m_pMoonBaboon->Set_Animation(Moon_Ufo_Programming, Moon_Ufo_MH);
		((CCody*)DATABASE->GetCody())->Set_ActiveHpBar(true);
		((CMay*)DATABASE->GetMay())->Set_ActiveHpBar(true);
		Set_HpBarActive(true);
		SCRIPT->VoiceFile_No27();
	}
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
	{
		// 마지막에 누가 박았는지에 따라 뷰포트 전환이 다름
		if (m_WhoCollide == GameID::eCODY)
		{
			// 지우지마세용
		}
		else if (m_WhoCollide == GameID::eMAY)
		{
			// 지우지마세용
		}
		m_pModelCom->Set_Animation(CutScene_RocketPhaseFinished_FlyingSaucer);
		m_pModelCom->Set_NextAnimIndex(UFO_RocketKnockDown_MH);
		m_IsCutScene = true;
	}
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD5) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		_float fMaxDistance = 201.f;
		DATABASE->Close_BossDoor();
		DATABASE->GoUp_BossFloor(fMaxDistance, 10.f);
		m_IsCutScene = false;
		m_ePhase = UFO_PHASE::PHASE_3;
	}
	else if (m_pGameInstance->Key_Down(DIK_F1) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		m_ePhase = UFO_PHASE::PHASE_3;
		m_IsCutScene = true;
	}

	if (true == m_pModelCom->Is_AnimFinished(CutScene_UFO_Boss_Intro))
		Set_EndIntroCutScene();

	/* 컷 신 재생중이 아니라면 보스 패턴 진행하자 나중에 컷 신 생기면 바꿈 */
	if (false == m_IsCutScene)
	{
		switch (m_ePhase)
		{
		case Client::CUFO::PHASE_1:
			Phase1_Pattern(dTimeDelta);
			break;
		case Client::CUFO::PHASE_2:
			Phase2_Pattern(dTimeDelta);
			break;
		case Client::CUFO::PHASE_3:
			Phase3_Pattern(dTimeDelta);
			break;
		}
	} 
	else
	{
		/* 끝 난 페이즈에 따라 컷 신이나 상호작용 진행 해 주자 */
		switch (m_ePhase)
		{
		case Client::CUFO::PHASE_1:
			Phase1_End(dTimeDelta);
			break;
		case Client::CUFO::PHASE_2:
			Phase2_End(dTimeDelta);
			break;
		case Client::CUFO::PHASE_3:
			Phase3_End(dTimeDelta);
			break;
		}
	}

	GoUp(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);

	/* Light */
	if (m_pBossLight)
	{
		LIGHT_DESC* pLightDesc = m_pBossLight->Get_LightDescPtr();
		if (nullptr != pLightDesc)
		{
			XMStoreFloat3(&pLightDesc->vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			pLightDesc->vPosition.y -= 3.f; // offset
		}
	}

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (false == m_IsActive)
		return NO_EVENT;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3000.f))
		return Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}


void CUFO::Laser_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	_vector vDir, vTargetPos;

	/* 지정된 타겟에 따라 포지션 세팅 */
	// 죽었을때 다른 타겟 설정
	CCody* pCody = (CCody*)DATABASE->GetCody();
	CMay* pMay = (CMay*)DATABASE->GetMay();
	_bool bCodyDead = pCody->Get_IsDeadInBossroom();
	_bool bMayDead = pMay->Get_IsDeadInBossroom();

	switch (m_eTarget)
	{
	case Client::CUFO::TARGET_CODY:
	{
		if(false == bCodyDead)
			vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);
		else
			vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);

		break;
	}
	case Client::CUFO::TARGET_MAY:
	{
		if (false == bMayDead)
			vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);
		else
			vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);

		break;
	}
	}

	vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));
	_float vAngle = XMConvertToDegrees(XMVectorGetX(vDirForRotate));

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 3.f);

	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vUFOLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	/* 레이저 건에서 타겟의 방향 벡터를 구하자 */
	_vector vLaserDir = vTargetPos - vUFOPos;

	_vector vDot = XMVector3AngleBetweenNormals(XMVector3Normalize(vUFOLook), XMVector3Normalize(vLaserDir));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(vDot));

	_matrix matPivot, matRotY, matTrans, matAnim;
	matTrans = XMMatrixTranslation(0.f, -0.5f, 0.f);
	matRotY = XMMatrixRotationY(XMConvertToRadians(-fAngle));
	matPivot = matRotY * matTrans;

	_uint LaserGunBoneIndex = m_pModelCom->Get_BoneIndex("LaserGun");
	matAnim = m_pModelCom->Get_AnimTransformation(LaserGunBoneIndex);
	matAnim = XMMatrixInverse(nullptr, matAnim);

	matPivot *= matAnim;

	m_pModelCom->Set_PivotTransformation(LaserGunBoneIndex, matPivot);

	/* LaserGunRing3 뼈가 레이저 총구 ㅇㅇ */
	/* LaserGun의 Right 벡터를 사용하니까 너무 이상하게 달달거려서 안움직이는 Align 뼈를 가져와서 사용함 그래도 움직이는건 UFO 행렬이 애니메이션 돌리면서 계속 바뀌기 떄문에 그런듯 */
	_matrix matUFOWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matLaserGunRing = m_pModelCom->Get_BoneMatrix("LaserGunRing3");
	_matrix matLaserGun = m_pModelCom->Get_BoneMatrix("Align");
	
	/* 레이저 나가는 오프셋 */
	_matrix matLaserRingWorld = matRotY * matLaserGunRing * matUFOWorld;

	/* 레이저 방향 */
	_matrix matAlign = matRotY * matLaserGun * matUFOWorld;
	_vector vLaserGunDir = XMLoadFloat4((_float4*)&matAlign.r[0].m128_f32[0]);

	/* 레이저 높이 보정 */
	_float4 vConvertDir;
	XMStoreFloat4(&vConvertDir, vLaserGunDir);
	vConvertDir.y += 0.0004f;

	/* 레이저에 시작위치랑 방향 벡터 던져주자 */
	XMStoreFloat4(&m_vLaserGunPos, matLaserRingWorld.r[3]);
	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(XMLoadFloat4(&vConvertDir)));

	/* 레이저 발사!!!!!!!!!! */
	if (true == m_IsLaserCreate)
	{
		_uint iOption = 0;
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeA"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA"), &iOption);
		m_IsLaserCreate = false;

		/* 레이저 패턴 3번 나오면 2페이즈로 바뀐다. */
		++m_iPhaseChangeCount;
	}
}
 
void CUFO::MoveStartingPoint(_double dTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4(&m_vStartUFOPos);
	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (true == ((CBossDoor*)DATABASE->Get_BossDoor01())->Get_Close())
		((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_Active(true);

	/* 처음에 저장해둔 타겟 포스의 Y위치까지 천천히 위로 이동해라. */
	if (vTargetPos.m128_f32[1] >= vUFOPos.m128_f32[1])
		m_pTransformCom->Go_Up(dTimeDelta / 3.f);
	else
		m_IsStartingPointMove = false;
}

void CUFO::GravitationalBomb_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	_vector vDir, vTargetPos;
	_uint iGravitationalBombMaxCount = 8;

	/* 지정된 타겟에 따라 포지션 세팅 */
	// 죽었을때 다른 타겟 설정
	CCody* pCody = (CCody*)DATABASE->GetCody();
	CMay* pMay = (CMay*)DATABASE->GetMay();
	_bool bCodyDead = pCody->Get_IsDeadInBossroom();
	_bool bMayDead = pMay->Get_IsDeadInBossroom();

	switch (m_eTarget)
	{
	case Client::CUFO::TARGET_CODY:
	{
		if (false == bCodyDead)
			vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);
		else
			vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);

		break;
	}
	case Client::CUFO::TARGET_MAY:
	{
		if (false == bMayDead)
			vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);
		else
			vTargetPos = m_pCodyTransform->Get_State(CTransform::STATE_POSITION);

		break;
	}
	}

	vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f);

	/* 몇 발 쏠지 정해줄려면 iGravitationalBombMaxCount 바꾸면 됨*/
	if (iGravitationalBombMaxCount >= m_iGravitationalBombCount)
	{
		m_fGravitationalBombLanchTime += (_float)dTimeDelta;

		if (2.f <= m_fGravitationalBombLanchTime)
		{
			/* 중력자 폭탄 생성 */
			_matrix vUFOWorld = m_pTransformCom->Get_WorldMatrix();
			_matrix LeftLaserHatch = m_pModelCom->Get_BoneMatrix("LeftLaserHatch");
			_matrix RightLaserHatch = m_pModelCom->Get_BoneMatrix("RightLaserHatch");

			LeftLaserHatch *= vUFOWorld;
			RightLaserHatch *= vUFOWorld;

			EFFECT_DESC_CLONE tEffectDesc;

			CCody* pCody = (CCody*)DATABASE->GetCody();
			CMay* pMay = (CMay*)DATABASE->GetMay();
			_bool bCodyDead = pCody->Get_IsDeadInBossroom();
			_bool bMayDead = pMay->Get_IsDeadInBossroom();

			if (m_eTarget == CUFO::TARGET_MAY) 
			{
				if(bMayDead)
					tEffectDesc.vStartPos = (_float4)&RightLaserHatch.r[3].m128_f32[0];
				else
					tEffectDesc.vStartPos = (_float4)&LeftLaserHatch.r[3].m128_f32[0];
			}
			else
			{
				if (bCodyDead)
					tEffectDesc.vStartPos = (_float4)&LeftLaserHatch.r[3].m128_f32[0];
				else
					tEffectDesc.vStartPos = (_float4)&RightLaserHatch.r[3].m128_f32[0];
			}

			XMStoreFloat3(&tEffectDesc.vDir, vDir);

			m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Boss_Gravitational_Bomb"), Level::LEVEL_STAGE, 
				TEXT("GameObject_3D_Boss_Gravitational_Bomb"), &tEffectDesc);

			/* 한 발 쏠때마다 타겟 바꿔주자 */
			if (m_iGravitationalBombCount % 2)
				m_eTarget = CUFO::TARGET_MAY;
			else
				m_eTarget = CUFO::TARGET_CODY;

			m_fGravitationalBombLanchTime = 0.f;
			++m_iGravitationalBombCount;
		}
	}
	else
	{
		/* 다시 초기화 해주자 */
		m_iGravitationalBombCount = 0;
		m_fGravitationalBombLanchTime = 0.f;

		/* 메인 레이저 다시 내려가자 */
		((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_LaserOperation(false);

		/* 중력자 폭탄 패턴 끝났으니 상호작용 패턴으로 바꾸자 */
		m_ePattern = CUFO::INTERACTION;
	}
}

void CUFO::Core_Destroyed()
{
	for (_uint i = 0; i < 3; ++i)
	{
		_bool IsBroken = ((CMoonBaboonCore*)DATABASE->Get_MoonBaboonCore(i))->Get_Broken();
		_int iActive = ((CMoonBaboonCore*)DATABASE->Get_MoonBaboonCore(i))->Get_ActiveCore();

		if (true == IsBroken && 1 == iActive)
		{
			m_ePattern = UFO_PATTERN::INTERACTION;

			if (false == m_IsHit)
			{
				Set_BossHpBarReduction(110);
				m_IsHit = true;
			}

			/* 페이즈가 바꼇다면 HitPod 애니메이션이 아니라 바로 CutScene_PowerCoresDestroyed_UFO로 바꿔줘야함 */
			if (3 != m_iPhaseChangeCount)
			{
				m_pModelCom->Set_Animation(UFO_Laser_HitPod);
				m_pModelCom->Set_NextAnimIndex(UFO_MH);
				m_pMoonBaboon->Set_Animation(Moon_Ufo_Laser_HitPod, UFO_MH);
			}

			m_IsLaserCreate = true;
			((CLaser_TypeA*)DATABASE->Get_LaserTypeA())->Set_Dead();
		}
	}

	/* 이건 테스트용 키 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		m_ePattern = UFO_PATTERN::INTERACTION;

		/* 페이즈가 바꼇다면 HitPod 애니메이션이 아니라 바로 CutScene_PowerCoresDestroyed_UFO로 바꿔줘야함 */
		if (3 != m_iPhaseChangeCount)
		{
			m_pModelCom->Set_Animation(UFO_Laser_HitPod);
			m_pModelCom->Set_NextAnimIndex(UFO_MH);
			m_pMoonBaboon->Set_Animation(Moon_Ufo_Laser_HitPod, UFO_MH);
		}

		m_IsLaserCreate = true;
		((CLaser_TypeA*)DATABASE->Get_LaserTypeA())->Set_Dead();
	}
}

void CUFO::Phase1_InterAction(_double dTimeDelta)
{
	/* 레이저 패턴이 3번 진행됬고 코어가 터졌다면 2페로 넘어갈 컷 신을 진행하자 */
	if (3 == m_iPhaseChangeCount)
	{
		m_IsCutScene = true;
		m_pModelCom->Set_Animation(CutScene_PowerCoresDestroyed_UFO);
		m_pModelCom->Set_NextAnimIndex(UFO_KnockDownMH);
		Ready_TriggerActor_Component();
		return;
	}

	/* 레이저에서 중력자 폭탄 패턴으로 변경 */
	if (true == m_pModelCom->Is_AnimFinished(UFO_Laser_HitPod))
	{
		((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_LaserOperation(true);

		/* 메인 레이저가 다 올라오면 중력자탄 패턴으로 바꿔라 */
		if (true == ((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Get_LaserUp())
		{
			m_ePattern = UFO_PATTERN::GRAVITATIONALBOMB;

			m_pModelCom->Set_Animation(UFO_MH);
			m_pModelCom->Set_NextAnimIndex(UFO_MH);

			m_fWaitingTime = 0.f;

			/* 페이즈가 2번 진행됬다면 보스 Floor를 올려라 */
			if (2 == m_iPhaseChangeCount)
			{
				_float fMaxDistance = 100.f;
				DATABASE->GoUp_BossFloor(fMaxDistance, 10.f);
			}
		}
	}

	/* 중력자 폭탄에서 레이저 패턴으로 변경 */
	if (true == m_pModelCom->Is_AnimFinished(UFO_MH))
	{
		m_fWaitingTime += (_float)dTimeDelta;

		/* 1초 대기했다가 레이저 패턴으로 바꿔라 */
		if (1.f <= m_fWaitingTime)
		{
			m_ePattern = UFO_PATTERN::LASER;
			m_IsHit = false;

			m_pModelCom->Set_Animation(UFO_Laser_MH);
			m_pModelCom->Set_NextAnimIndex(UFO_Laser_MH);
			m_pMoonBaboon->Set_Animation(Moon_Ufo_Programming, Moon_Ufo_MH);

			m_fWaitingTime = 0.f;

			++m_iPatternChangeCount;

			/* 레이저 패턴으로 바뀔 때 메이 <-> 코디 순으로 타겟 바꿔주자 */
			if (m_iPatternChangeCount % 2)
				m_eTarget = CUFO::TARGET_CODY;
			else
				m_eTarget = CUFO::TARGET_MAY;
		}
	}
}

void CUFO::Phase1_Pattern(_double dTimeDelta)
{
	// 컷 신 끝나고 보스전이 시작되었을 때 위로 올라감
	if (true == m_IsStartingPointMove)
		MoveStartingPoint(dTimeDelta);

	/* 테스트 용 나중에 코어 터지는거 구현되면 바꿈 */
	Core_Destroyed();

	/* InterAction은 패턴이 끝나고 다음 패턴이 나오기 전까지 상호작용 해야 할 것들 진행시켜주는 상태 */
	switch (m_ePattern)
	{
	case Client::CUFO::INTERACTION:
		Phase1_InterAction(dTimeDelta);
		break;
	case Client::CUFO::LASER:
		Laser_Pattern(dTimeDelta);
		break;
	case Client::CUFO::GRAVITATIONALBOMB:
		GravitationalBomb_Pattern(dTimeDelta);
		break;
	}
}

void CUFO::Phase2_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	/* 공전 드가자 */
	OrbitalMovementCenter(dTimeDelta);

	if (4 == m_iGuidedMissileHitCount)
	{
		switch (m_WhoCollide)
		{
		case Engine::GameID::eCODY:
			static_cast<CSubCamera*>(DATABASE->Get_SubCam())->Start_HitRocket_Boss();
			break;
		case Engine::GameID::eMAY:
			static_cast<CMainCamera*>(DATABASE->Get_MainCam())->Start_HitRocket_Boss();
			break;
		}
		m_pModelCom->Set_Animation(CutScene_RocketPhaseFinished_FlyingSaucer);
		m_pModelCom->Set_NextAnimIndex(UFO_RocketKnockDown_MH);
		m_IsCutScene = true;
	}

	switch (m_ePattern)
	{
	case Client::CUFO::INTERACTION:
		Phase2_InterAction(dTimeDelta);
		break;
	case Client::CUFO::GUIDEDMISSILE:
		GuidedMissile_Pattern(dTimeDelta);
		break;
	}
}

void CUFO::Phase2_InterAction(_double dTimeDelta)
{
}

void CUFO::OrbitalMovementCenter(_double dTimeDelta)
{
	_vector vDir, vCenterPos, vUFOLook, vDot;
	_vector vTest;
	/* 중심점은 1페 때 잡아줬던 시작점 */
	vCenterPos = XMLoadFloat4(&m_vStartUFOPos);
	vUFOLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	/* 애니메이션 UFO_LaserRippedOff 끝나고 그 자리에서 바로 공전시켜야 하는데 UFO_LEFT로 바뀔때 위치가 안맞아서 세팅해줌 */
	SetUp_AnimationTransform();
	
	_float3 vConverCenterPos;
	XMStoreFloat3(&vConverCenterPos, vCenterPos);

	vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vDir = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vDot = XMVector3AngleBetweenNormals(XMVector3Normalize(vUFOLook), XMVector3Normalize(vDir));

	_float fAngle = XMConvertToDegrees(XMVectorGetX(vDot));

	if (true == m_IsFirstAngleSetting)
	{
		m_fRotAngle = fAngle;
		m_IsFirstAngleSetting = false;
	}

	/* 자전 각도 돌리는 속도 */
	if (m_fRotAngle < fAngle)
		m_fRotAngle += (_float)dTimeDelta * 50.f;
	else if (m_fRotAngle >= fAngle)
		m_fRotAngle -= (_float)dTimeDelta * 50.f;

	/* 공전 속도 */
	m_fRevAngle += (_float)dTimeDelta * 20.f;

	_matrix matWorld, matRotY, matTrans, matRevRotY, matParent;

	matRotY = XMMatrixRotationY(XMConvertToRadians(-m_fRotAngle));
	matTrans = XMMatrixTranslation(m_vTranslationPos.x, m_vTranslationPos.y, m_vTranslationPos.z);
	matParent = XMMatrixTranslation(vConverCenterPos.x, vConverCenterPos.y, vConverCenterPos.z);
	matRevRotY = XMMatrixRotationY(XMConvertToRadians(m_fRevAngle));

	matWorld = matRotY * matTrans * matRevRotY * matParent;

	m_pTransformCom->Set_WorldMatrix(matWorld);
	m_pTriggerActorCom->Update_TriggerActor();
}

void CUFO::GuidedMissile_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyMissile || nullptr == m_pMayMissile)
	{
		m_fGuidedMissileTime += (_float)dTimeDelta;

		if (2.f <= m_fGuidedMissileTime)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4 vConvertPos;
			XMStoreFloat4(&vConvertPos, vPos);

			_matrix vUFOWorld = m_pTransformCom->Get_WorldMatrix();
			_matrix LeftRocketHatch = m_pModelCom->Get_BoneMatrix("LeftFrontRocketHatch");
			_matrix RightRocketHatch = m_pModelCom->Get_BoneMatrix("RightFrontRocketHatch");

			_vector vCodyDir = XMVector3Normalize(((CCody*)DATABASE->GetCody())->Get_Position() - vPos);
			_vector vMayDir = XMVector3Normalize(((CCody*)DATABASE->GetMay())->Get_Position() - vPos);

			LeftRocketHatch = LeftRocketHatch * vUFOWorld;
			RightRocketHatch = RightRocketHatch * vUFOWorld;

			CGameObject* pGameObject = nullptr;
			CBoss_Missile::tagBossMissile_Desc tMissileDesc;

			/* 유도 미사일 발사!!!!!!!!!!!!!!!!! */
			if (nullptr == m_pCodyMissile)
			{
				/* true면 Cody */
				tMissileDesc.IsTarget_Cody = true;
				tMissileDesc.vPosition = (_float4)&LeftRocketHatch.r[3].m128_f32[0];
				m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc, &pGameObject);
				m_pCodyMissile = static_cast<CBoss_Missile*>(pGameObject);
				m_fGuidedMissileTime = 0.f;
			}

			if (nullptr == m_pMayMissile)
			{
				/* false면 May */
				tMissileDesc.IsTarget_Cody = false;
				tMissileDesc.vPosition = (_float4)&RightRocketHatch.r[3].m128_f32[0];
				m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc, &pGameObject);
				m_pMayMissile = static_cast<CBoss_Missile*>(pGameObject);
				m_fGuidedMissileTime = 0.f;
			}
		}
	}
}

void CUFO::SetUp_AnimationTransform()
{
	if (UFO_Left == m_pModelCom->Get_CurAnimIndex() && m_IsAnimationTransform)
	{
		_matrix BaseBone = m_pModelCom->Get_BoneMatrix("Root");
		_matrix UFOWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix AnimUFOWorld = BaseBone * UFOWorld;

		_vector vOffSet = XMLoadFloat4((_float4*)&AnimUFOWorld.r[3].m128_f32[0]) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_matrix matTrans = XMMatrixTranslation(vOffSet.m128_f32[0], vOffSet.m128_f32[1], vOffSet.m128_f32[2]);

		m_pModelCom->Set_PivotTransformation(m_pModelCom->Get_BoneIndex("Root"), matTrans);

		m_IsAnimationTransform = false;
	}
}

void CUFO::Phase3_Pattern(_double dTimeDelta)
{
	if (nullptr == m_pCodyTransform || nullptr == m_pMayTransform)
		return;

	if (true == m_IsStartingPointMove)
	{
		Phase3_MoveStartingPoint(dTimeDelta);
		return;
	}

	if (true == ((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Get_LaserUp() && false == m_IsGoingLastFloor)
	{
		DATABASE->GoUp_BossFloor(100.f, 15.f);
		m_IsGoingLastFloor = true;
	}

	/* 마지막 층에 도달했을 때 */
	if (true == ((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Get_ArrivalLastFloor() && false == m_IsLastFloor)
	{
		m_ePattern = CUFO::GROUNDPOUND;
		m_IsLastFloor = true;

		/* 서브 레이저 3개 생성 */
		Ready_Layer_MoonBaboon_SubLaser(TEXT("Layer_SubLaser"));
	}

	/* 3페는 메이만 타겟 */
	_vector vDir, vTargetPos;
	vTargetPos = m_pMayTransform->Get_State(CTransform::STATE_POSITION);

	vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta);

	/* SubLaser 시간마다 올라오자 ㅇㅇ */
	if (true == m_IsSubLaserOperation && 0 != m_vecSubLaser.size())
		DependingTimeSubLaserOperation(dTimeDelta);

	switch (m_ePattern)
	{
	case Client::CUFO::INTERACTION:
		Phase3_InterAction(dTimeDelta);
		break;
	case Client::CUFO::GROUNDPOUND:
		GroundPound_Pattern(dTimeDelta);
		break;
	}

}

void CUFO::Phase3_MoveStartingPoint(_double dTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4(&m_vStartUFOPos);
	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMVector3Normalize(vTargetPos - vUFOPos);
	_vector vComparePos = vTargetPos - vUFOPos;
	vComparePos.m128_f32[1] = 0.f;
	_float vDistance = XMVectorGetX(XMVector3Length(vComparePos));
	m_pTransformCom->RotateYawDirectionOnLand(vDir, dTimeDelta);

	vDir.m128_f32[1] = 0.f;

	if (1.f <= vDistance)
		m_pTransformCom->MoveToDir(XMVector3Normalize(vDir), dTimeDelta * 5.f);
	else
	{
		m_IsStartingPointMove = false;
		m_pModelCom->Set_Animation(UFO_MH);
		m_pModelCom->Set_NextAnimIndex(UFO_MH);

		/* 도착했으면 메인레이저 올라와라 ㅇㅇ */
		((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_LaserOperation(true);
	}
}

void CUFO::Phase3_InterAction(_double dTimeDelta)
{
}

void CUFO::GroundPound_Pattern(_double dTimeDelta)
{
	if(false == m_IsGroundPound)
		m_fGroundPoundTime += (_float)dTimeDelta;

	/* GroundPound가 시작됬을 때 메이의 포스 저장 */
	if (4.f <= m_fGroundPoundTime && false == m_IsGroundPound)
	{
		XMStoreFloat4(&m_vGroundPoundTargetPos, m_pMayTransform->Get_State(CTransform::STATE_POSITION));
		m_IsGroundPound = true;
	}

	/* UFO가 찍기 애니메이션을 진행중이다 밑에서 쓸데없이 연산하지말고 걍 나가자 ㅇㅇ */
	if (UFO_GroundPound == m_pModelCom->Get_CurAnimIndex())
	{
		if (false == m_IsGroundPoundEffectCreate && 30.f <= m_pModelCom->Get_CurrentTime(UFO_GroundPound))
		{
			//_vector MayPos = ((CMay*)DATABASE->GetMay())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_matrix UFOWorld = m_pTransformCom->Get_WorldMatrix();
			UFOWorld.r[3].m128_f32[1] -= 10.f;

			EFFECT->Add_Effect(Effect_Value::BossGroundPound, UFOWorld);
			EFFECT->Add_Effect(Effect_Value::BossGroundPound_Ring, UFOWorld);
			EFFECT->Add_Effect(Effect_Value::BossGroundPound_Smoke, UFOWorld);
			m_IsGroundPoundEffectCreate = true;
		}

		if (88.f <= m_pModelCom->Get_CurrentTime(UFO_GroundPound))
		{
			m_pModelCom->Set_Animation(UFO_MH);
			m_IsGroundPound = false;
			m_IsGroundPoundEffectCreate = false;
		}
		else
			return;
	}

	if (true == m_IsGroundPound)
	{
		_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vComparePos = XMLoadFloat4(&m_vGroundPoundTargetPos) - vUFOPos;
		_vector vDirForRotate = XMVector3Normalize(XMVectorSetY(vComparePos, 0.f));

		/* Y는 거리 계산하면 안됨 메이의 Y는 땅에 있기 때문 */
		vComparePos.m128_f32[1] = 0.f;
		_float fDistance = XMVectorGetX(XMVector3Length(vComparePos));

		if (1.f <= fDistance)
			m_pTransformCom->MoveToDir(XMVector3Normalize(vComparePos), dTimeDelta * 5.f);
		else
		{
			m_pModelCom->Set_Animation(UFO_GroundPound);
			m_pModelCom->Set_NextAnimIndex(UFO_MH);
			m_fGroundPoundTime = 0.f;
			m_IsGroundPound = false;
		}
	}
}

void CUFO::DependingTimeSubLaserOperation(_double dTimeDelta)
{
	m_fSubLaserTime += (_float)dTimeDelta;

	if (10.f <= m_fSubLaserTime)
	{
		m_vecSubLaser[m_iSubLaserIndex]->Set_LaserOperation(true);

		m_fSubLaserTime = 0.f;
		++m_iSubLaserIndex;

		if (3 <= m_iSubLaserIndex)
			m_IsSubLaserOperation = false;
	}
}

HRESULT CUFO::Phase1_End(_double dTimeDelta)
{
	if (m_pModelCom->Is_AnimFinished(CutScene_PowerCoresDestroyed_UFO))
	{

		m_pTriggerActorCom->Update_TriggerActor();
		m_IsInterActive = true;
	}

	/* 컷 신 애니메이션이 끝났다면 이제 상호작용 하자 ㅇㅇ */
	if(true == m_IsInterActive)
	{
		_matrix LaserGunRing = m_pModelCom->Get_BoneMatrix("LaserGunRing3");
		_matrix matLaserGunWorld = LaserGunRing * m_pTransformCom->Get_WorldMatrix();

		UI_Generator->CreateInterActiveUI_AccordingRange(Player::May, UI::Boss_UFO_LaserGunRing, XMLoadFloat4((_float4*)&matLaserGunWorld.r[3].m128_f32[0]),
			50.f, m_IsMayCollide, ((CMay*)DATABASE->GetMay())->Get_InterActiveUIDisable());

		/* 보정 */
		matLaserGunWorld.r[3].m128_f32[1] += 3.f;
		UI_Generator->CreateInterActiveUI_AccordingRange(Player::Cody, UI::Boss_UFO, XMLoadFloat4((_float4*)&matLaserGunWorld.r[3].m128_f32[0]),
			50.f, m_IsCodyCollide, ((CCody*)DATABASE->GetCody())->Get_InterActiveUICreate());

		/* 레이저 건 안달린 애니메이션이 없다... 직접 없애주자...ㅠㅠ 잘가라 */
		if(100.f <= m_pModelCom->Get_CurrentTime(UFO_LaserRippedOff) && false == m_IsLaserGunRid)
				GetRidLaserGun();

		if (m_pModelCom->Is_AnimFinished(UFO_LaserRippedOff))
		{
			/* 애니메이션이 딱 끝났을 때 뼈의 위치를 받아서 UFO 월드에 세팅해준다. */
			_matrix BaseBone = m_pModelCom->Get_BoneMatrix("Base");
			_matrix UFOWorld = m_pTransformCom->Get_WorldMatrix();
			_matrix AnimUFOWorld = BaseBone * UFOWorld;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)&AnimUFOWorld.r[3].m128_f32[0]));

			/* 중점에서 부터 마지막 애니메이션의 포지션을 빼서 그 지점에서부터 공전할 수 있도록 구해줌. 2페이지에서 보스 움직일 때 사용 */
			_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&m_vStartUFOPos);
			XMStoreFloat4(&m_vTranslationPos, vUFOPos);

			/* 보스 2페이즈로 바꿔주자 */
			m_ePhase = CUFO::PHASE_2;
			m_ePattern = CUFO::GUIDEDMISSILE;
			m_IsCutScene = false;

			((CCody*)DATABASE->GetCody())->Set_AllActiveHpBar(true);
			((CMay*)DATABASE->GetMay())->Set_AllActiveHpBar(true);
			Set_HpBarActive(true);
			UI_Generator->Set_AllActivation(true);
		}
	}

	return S_OK;
}

HRESULT CUFO::Ready_TriggerActor_Component()
{
	m_UserData = USERDATA(GameID::eBOSSUFO, this);
	
	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vUFOPos.m128_f32[1] -= 8.f;

	m_pTriggerTransformCom->Set_State(CTransform::STATE_POSITION, vUFOPos);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTriggerTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(5.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

HRESULT CUFO::Ready_StaticActor_Component()
{
	m_UserData = USERDATA(GameID::eBOSSUFO, this);

	m_pStaticTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	_matrix BaseBone = m_pModelCom->Get_BoneMatrix("Root");
	_matrix matTrans = XMMatrixScaling(100.f, 100.f, 100.f);
	BaseBone = matTrans * BaseBone * m_pTransformCom->Get_WorldMatrix();

	_matrix matRotY = XMMatrixRotationZ(XMConvertToRadians(180.f));
	BaseBone = matRotY * BaseBone;

	BaseBone.r[3].m128_f32[1] += 0.05f;
	m_pStaticTransformCom->Set_WorldMatrix(BaseBone);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pStaticModelCom;
	ArgDesc.pTransform = m_pStaticTransformCom;
	ArgDesc.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	m_IsActorCreate = false;

	return S_OK;
}

HRESULT CUFO::TriggerActorReplacement()
{
	Safe_Release(m_pTriggerActorCom);
	m_pTriggerActorCom = nullptr;

	m_UserData = USERDATA(GameID::eBOSSUFO, this);

	_matrix EntranceHatch = m_pModelCom->Get_BoneMatrix("EntranceHatch");
	EntranceHatch *= m_pTransformCom->Get_WorldMatrix();

	_vector TriggerPos = XMLoadFloat4((_float4*)&EntranceHatch.r[3].m128_f32[0]);
	TriggerPos.m128_f32[1] -= 0.5f;
	
	m_pTriggerTransformCom->Set_State(CTransform::STATE_POSITION, TriggerPos);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTriggerTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(0.3f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_EnterTrigger"), (CComponent**)&m_pTriggerActorCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

HRESULT CUFO::Phase2_End(_double dTimeDelta)
{
	/* UFO 다운 상태일 때 스태틱 액터 생성 트리거 액터 교체 */
	if (UFO_RocketKnockDown_MH == m_pModelCom->Get_CurAnimIndex() && true == m_IsActorCreate)
	{
		Ready_StaticActor_Component();
		TriggerActorReplacement();
		m_IsTriggerActive = true;
		m_IsPhase2InterActive = true;
	}

	if (true == m_IsCodyEnter)
	{
		m_pModelCom->Set_Animation(CutScene_EnterUFO_FlyingSaucer, 30.f);
		m_pModelCom->Set_NextAnimIndex(UFO_MH);
		m_IsCodyEnter = false;

		if (m_pModelCom->Is_AnimFinished(UFO_CodyHolding_low))
		{
			m_pModelCom->Set_Animation(UFO_CodyHolding);
			m_pModelCom->Set_NextAnimIndex(UFO_CodyHolding);
		}

		if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		{
			m_pModelCom->Set_Animation(UFO_LaserRippedOff);
			m_pModelCom->Set_NextAnimIndex(UFO_Left);
		}

	}

	if (89.f <= m_pModelCom->Get_CurrentTime(CutScene_EnterUFO_FlyingSaucer) && false == m_IsCodySetPos)
	{
		if (CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Eject_InUFO) == false)
		{
			CCutScenePlayer::GetInstance()->Set_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Eject_InUFO, true);
			CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Eject_InUFO"));
		}
	}

	if (m_pModelCom->Is_AnimFinished(CutScene_EnterUFO_FlyingSaucer))
	{
		m_IsCutScene = false;
		m_ePhase = CUFO::PHASE_3;
		m_IsStartingPointMove = true;

		/* 애니메이션이 딱 끝났을 때 뼈의 위치를 받아서 UFO 월드에 세팅해준다. */
		_matrix BaseBone = m_pModelCom->Get_BoneMatrix("Base");
		_matrix UFOWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix AnimUFOWorld = BaseBone * UFOWorld;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)&AnimUFOWorld.r[3].m128_f32[0]));
	}

	return S_OK;
}

void CUFO::GetRidLaserGun()
{
	_matrix LaserBaseBone = m_pModelCom->Get_BoneMatrix("LaserBase");
	_matrix matDeleteScale;

	matDeleteScale = XMMatrixScaling(0.f, 0.f, 0.f);
	LaserBaseBone *= matDeleteScale;

	m_pModelCom->Set_PivotTransformation(m_pModelCom->Get_BoneIndex("LaserBase"), LaserBaseBone);

	m_IsLaserGunRid = true;

	EFFECT->Add_Effect(Effect_Value::Boss_BrokenLaser_Particle);
	EFFECT->Add_Effect(Effect_Value::Boss_BrokenLaser_Particle);
	EFFECT->Add_Effect(Effect_Value::Boss_BrokenLaser_Smoke);
}

HRESULT CUFO::Phase3_End(_double dTimeDelta)
{
	if (m_pModelCom->Get_CurrentTime(CutScene_Eject_FlyingSaucer) >= 848.f) // MoonBaboon 달에 굴러떨어지고 나서..
	{
		_vector vPosition = { 64.f + 11.f, 357.5f - 255.f, 195.f, 1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	}

	if (m_pModelCom->Is_AnimFinished(CutScene_Eject_FlyingSaucer) == true)
	{
		((CMoonUFO*)DATABASE->Get_MoonUFO())->Set_CutSceneEnd(true);
	}

	if (false == m_IsEjection)
	{
#ifdef __PLAY_CUTSCENE
		if (CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_GotoMoon) == false)
		{
			CCutScenePlayer::GetInstance()->Set_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_GotoMoon, true);
			CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_GotoMoon"));
		}
#endif
		_vector vPosition = { 64.f, 457.8895f, 195.f, 1.f };
		XMStoreFloat4(&m_vStartUFOPos, vPosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vStartUFOPos));
		m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));
		m_pModelCom->Set_Animation(CutScene_Eject_FlyingSaucer);
		m_pModelCom->Set_NextAnimIndex(UFO_MH);
		m_IsEjection = true;

		/* UI OFF */
		UI_Generator->Set_AllActivation(false);
		((CCody*)DATABASE->GetCody())->Set_AllActiveHpBar(false);
		((CMay*)DATABASE->GetMay())->Set_AllActiveHpBar(false);
		Set_HpBarActive(false);

		m_pGameInstance->Set_MainViewFog(false);
	}

	if(true == m_IsEjection && false == m_IsLaserDown)
	{
		((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_LaserOperation(false);

		for (auto pSubLaser : m_vecSubLaser)
			pSubLaser->Set_LaserOperation(false);

		m_IsLaserDown = true;
	}

	return S_OK;
}

HRESULT CUFO::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	_uint iMaterialIndex = 0;

	m_pModelCom->Sepd_Bind_Buffer();

	iMaterialIndex = 1;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	iMaterialIndex = 2;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	iMaterialIndex = 3;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	iMaterialIndex = 4;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	iMaterialIndex = 5;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	iMaterialIndex = 6;
	m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, 0);
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 0, false, eGroup);

	// 0: Alpha 
	iMaterialIndex = 0;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 20, false, eGroup);

	return S_OK;
}

void CUFO::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		if (m_ePhase == CUFO::PHASE_1)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCodyCollide = true;
		}
		else if (m_ePhase == CUFO::PHASE_2 && true == m_IsTriggerActive)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSENTERUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCodyEnter = true;
		}

	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		if (m_ePhase == CUFO::PHASE_1)
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSUFO, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCodyCollide = false;
		}
	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_IsMayCollide = true;
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		/* 여기 SetTriggerID false로 바꾸면 안됨 */
		m_IsMayCollide = false;
	}
}

void CUFO::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Back, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Fwd, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Left, true);

	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_GroundPound, true);

	m_pModelCom->Add_LerpInfo(UFO_Laser_MH, UFO_Laser_HitPod, true);
	m_pModelCom->Add_LerpInfo(UFO_Laser_MH, CutScene_PowerCoresDestroyed_UFO, true);

	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_Laser_MH, true);

	m_pModelCom->Add_LerpInfo(CutScene_PowerCoresDestroyed_UFO, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(CutScene_PowerCoresDestroyed_UFO, UFO_KnockDownMH, true);

	m_pModelCom->Add_LerpInfo(UFO_KnockDownMH, UFO_CodyHolding_Enter, true);

	m_pModelCom->Add_LerpInfo(UFO_CodyHolding_Enter, UFO_CodyHolding_low, true);
	m_pModelCom->Add_LerpInfo(UFO_CodyHolding_low, UFO_CodyHolding, true);

	m_pModelCom->Add_LerpInfo(UFO_CodyHolding, UFO_LaserRippedOff, true);

	m_pModelCom->Add_LerpInfo(UFO_LaserRippedOff, UFO_Left, true, 150.f);
	m_pModelCom->Add_LerpInfo(UFO_LaserRippedOff, UFO_MH, true);

	m_pModelCom->Add_LerpInfo(UFO_Left, CutScene_RocketPhaseFinished_FlyingSaucer, true);

	m_pModelCom->Add_LerpInfo(CutScene_RocketPhaseFinished_FlyingSaucer, UFO_RocketKnockDown_MH, true);

	m_pModelCom->Add_LerpInfo(UFO_RocketKnockDown_MH, CutScene_EnterUFO_FlyingSaucer, true, 150.f);
	m_pModelCom->Add_LerpInfo(CutScene_EnterUFO_FlyingSaucer, UFO_RocketKnockDown_MH, false);

	m_pModelCom->Add_LerpInfo(CutScene_EnterUFO_FlyingSaucer, UFO_MH, false);
	m_pModelCom->Add_LerpInfo(CutScene_EnterUFO_FlyingSaucer, UFO_Fwd, false);

	m_pModelCom->Add_LerpInfo(UFO_GroundPound, UFO_MH, true, 1.f);
}

HRESULT CUFO::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_TriggerTransform"), (CComponent**)&m_pTriggerTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_StaticTransform"), (CComponent**)&m_pStaticTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_StaticUFO"), TEXT("Com_StaticModel"), (CComponent**)&m_pStaticModelCom), E_FAIL);
	
	m_pCodyTransform = ((CCody*)DATABASE->GetCody())->Get_Transform();
	NULL_CHECK_RETURN(m_pCodyTransform, E_FAIL);
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)DATABASE->GetMay())->Get_Transform();
	NULL_CHECK_RETURN(m_pMayTransform, E_FAIL);
	Safe_AddRef(m_pMayTransform);

	return S_OK;
}

HRESULT CUFO::Ready_UI()
{
	CGameObject* pGameObject = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("BossHpBar"), nullptr, &pGameObject), E_FAIL);
	m_pBossHpBar = static_cast<CBossHpBar*>(pGameObject);

	return S_OK;
}

HRESULT CUFO::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true, RENDER_GROUP::RENDER_NONALPHA);

	return S_OK;
}

void CUFO::Set_IsGuidedMissileDeadCheck(_bool IsCheck)
{
	if (true == IsCheck)
		m_pCodyMissile = nullptr;
	else
		m_pMayMissile = nullptr;
}

void CUFO::Set_UFOAnimation(_uint iAnimIndex, _uint iNextAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
	m_pModelCom->Set_NextAnimIndex(iNextAnimIndex);
}

void CUFO::Set_CodyEnterUFO()
{
	m_IsCodyEnter = true;
}

void CUFO::Set_CutScene(_bool IsCheck)
{
	m_IsCutScene = IsCheck;
}

void CUFO::Set_EndIntroCutScene()
{
	m_IsCutScene = false;
	DATABASE->Close_BossDoor();
	m_pMoonBaboon->Set_Animation(Moon_Ufo_Programming, Moon_Ufo_MH);
	((CCody*)DATABASE->GetCody())->Set_ActiveHpBar(true);
	((CMay*)DATABASE->GetMay())->Set_ActiveHpBar(true);
	m_pBossHpBar->Set_Active(true);
	SCRIPT->VoiceFile_No27();
}

void CUFO::Set_MoonBaboonPtr(CMoonBaboon * pMoonBaboon)
{
	if (nullptr == pMoonBaboon)
		return;

	m_pMoonBaboon = pMoonBaboon;
}

void CUFO::Set_BossUFOUp(_float fMaxDistance, _float fSpeed)
{
	XMStoreFloat3(&m_vMaxPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vMaxPos.y += fMaxDistance;

	m_fMaxY = fMaxDistance;
	m_IsGoUp = true;
	m_fUpSpeed = fSpeed;

	/* 끝났다면 스피드 원래대로 초기화 해주자 */
	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);
}

void CUFO::GoUp(_double dTimeDelta)
{
	if (false == m_IsGoUp)
		return;

	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);

	_float fDist = (_float)dTimeDelta * m_fUpSpeed;
	m_fDistance += fDist;

	if (m_fMaxY <= m_fDistance)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vMaxPos), 1.f));
		m_fMaxY = 0.f;
		m_IsGoUp = false;
		m_fDistance = 0.f;
		DATABASE->Set_BossFloorUp(false);
		return;
	}

	m_pTransformCom->Go_Up(dTimeDelta);
}


void CUFO::Ready_Layer_MoonBaboon_SubLaser(const _tchar* pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	_uint iSubLaserCount = 4;
	m_vecSubLaser.reserve(iSubLaserCount);

	for (_uint i = 0; i < iSubLaserCount; ++i)
	{
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_MoonBaboon_SubLaser"), nullptr, &pGameObject);
		m_vecSubLaser.emplace_back(static_cast<CMoonBaboon_SubLaser*>(pGameObject));
		m_vecSubLaser[i]->SetUp_SubLaserPosition(i);
	}
}

HRESULT CUFO::Set_MeshRenderGroup()
{
	m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_ALPHA);
	m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(3, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(4, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(5, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(6, tagRenderGroup::RENDER_NONALPHA);
	return S_OK;
}

void CUFO::Set_MissilePtrReset(_bool IsTargetCheck)
{
	if (true == IsTargetCheck)
	{
		m_pCodyMissile = nullptr;
		Safe_Release(m_pCodyMissile);
	}
	else
	{
		m_pMayMissile = nullptr;
		Safe_Release(m_pMayMissile);
	}
}

void CUFO::Set_BossHpBarReduction(_float fDamage)
{
	m_pBossHpBar->Set_HpBarReduction(fDamage);
}

void CUFO::Set_GuidedMissileIncreaseHitCount()
{
	++m_iGuidedMissileHitCount;

	if (4 <= m_iGuidedMissileHitCount)
		m_iGuidedMissileHitCount = 4;
	
	if(3 >= m_iGuidedMissileHitCount)
		Set_UFOAnimation(UFO_Laser_HitPod, UFO_Left);
}

void CUFO::Set_Active(_bool IsActive)
{
	m_IsActive = IsActive;
	m_pMoonBaboon->Set_Active(IsActive);
}

void CUFO::Set_HpBarActive(_bool IsActive)
{
	m_pBossHpBar->Set_Active(IsActive);
}

HRESULT CUFO::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
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
	if (m_pBossLight)
	{
		m_pBossLight->Set_Dead(true);
		//Safe_Release(m_pBossLight);
		m_pBossLight = nullptr;
	}

	if (nullptr != m_pStaticActorCom)
		Safe_Release(m_pStaticActorCom);
	if (nullptr != m_pStaticTransformCom)
		Safe_Release(m_pStaticTransformCom);


	if (nullptr != m_pTriggerActorCom)
		Safe_Release(m_pTriggerActorCom);
	if (nullptr != m_pTriggerTransformCom)
		Safe_Release(m_pTriggerTransformCom);
	if (nullptr != m_pEnterTriggerActorCom)
		Safe_Release(m_pEnterTriggerActorCom);

	for (auto pSubLaser : m_vecSubLaser)
		Safe_Release(pSubLaser);

	m_vecSubLaser.clear();

	if(nullptr != m_pCodyMissile)
		Safe_Release(m_pCodyMissile);
	if (nullptr != m_pMayMissile)
		Safe_Release(m_pMayMissile);
	if (nullptr != m_pBossHpBar)
		Safe_Release(m_pBossHpBar);

	/*Safe_Release(m_pMoonBaboon);*/
	Safe_Release(m_pStaticModelCom);
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}