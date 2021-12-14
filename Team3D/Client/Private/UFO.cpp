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

	DATABASE->Set_BossUFO(this);

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

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_X))
		DATABASE->GoUp_BossFloor(99.f);
	else if(m_pGameInstance->Key_Down(DIK_Z))
		DATABASE->GoUp_BossFloor(100.f);


	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		m_IsCutScene = false;
		DATABASE->Close_BossDoor();
	}
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
		m_ePhase = CUFO::PHASE_2;
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += 6.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pModelCom->Set_Animation(CutScene_PowerCoresDestroyed_UFO);
		m_pModelCom->Set_NextAnimIndex(UFO_KnockDownMH);
	}
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD8))
	{
		m_pModelCom->Set_Animation(CutScene_RocketPhaseFinished_FlyingSaucer);
		m_pModelCom->Set_NextAnimIndex(UFO_RocketKnockDown_MH);
		m_IsCutScene = true;
	}
	else if (m_pGameInstance->Key_Down(DIK_NUMPAD9))
	{
		m_IsCodyEnter = true;
	}
	////////////////////////////////////////////////////////////////////////////////////

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

	return NO_EVENT;
}

_int CUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
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

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f);

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
	vConvertDir.y += 0.0005f;

	/* 레이저에 시작위치랑 방향 벡터 던져주자 */
	XMStoreFloat4(&m_vLaserGunPos, matLaserRingWorld.r[3]);
	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(XMLoadFloat4(&vConvertDir)));

	/* 레이저 발사!!!!!!!!!! */
	if (true == m_IsLaserCreate)
	{
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeA"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA"));
		m_IsLaserCreate = false;

		/* 레이저 패턴 3번 나오면 2페이즈로 바뀐다. */
		++m_iPhaseChangeCount;
	}
}
 
void CUFO::MoveStartingPoint(_double dTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4(&m_vStartUFOPos);
	_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

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
	_uint iGravitationalBombMaxCount = 2;

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

	/* 우주선을 타겟쪽으로 천천히 회전 */
	m_pTransformCom->RotateYawDirectionOnLand(vDirForRotate, dTimeDelta / 5.f);

	/* 10발만 쏘자 */
	if (iGravitationalBombMaxCount >= m_iGravitationalBombCount)
	{
		m_fGravitationalBombLanchTime += (_float)dTimeDelta;

		if (2.f <= m_fGravitationalBombLanchTime)
		{
			/* 중력자 폭탄 생성 */

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

void CUFO::Phase1_InterAction(_double dTimeDelta)
{
	/* 레이저 패턴이 3번 진행됬고 코어가 터졌다면 2페로 넘어갈 컷 신을 진행하자 */
	if (3 == m_iPhaseChangeCount)
	{
		m_IsCutScene = true;
		m_pModelCom->Set_Animation(CutScene_PowerCoresDestroyed_UFO);
		m_pModelCom->Set_NextAnimIndex(UFO_KnockDownMH);
		Ready_Actor_Component();
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
				_float fMaxDistance = 99.f;
				DATABASE->GoUp_BossFloor(fMaxDistance);
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

			m_pModelCom->Set_Animation(UFO_Laser_MH);
			m_pModelCom->Set_NextAnimIndex(UFO_Laser_MH);

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
	if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
	{
		m_ePattern = UFO_PATTERN::INTERACTION;

		/* 페이즈가 바꼇다면 HitPod 애니메이션이 아니라 바로 CutScene_PowerCoresDestroyed_UFO로 바꿔줘야함 */
		if (3 != m_iPhaseChangeCount)
		{
			m_pModelCom->Set_Animation(UFO_Laser_HitPod);
			m_pModelCom->Set_NextAnimIndex(UFO_MH);
		}

		m_IsLaserCreate = true;
		((CLaser_TypeA*)DATABASE->Get_LaserTypeA())->Set_Dead();
	}

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

	_matrix matWorld, matRotY, matTrans, matRevRotY, matParent;

	if (m_fRotAngle < fAngle)
		m_fRotAngle += (_float)dTimeDelta * 50.f;
	else if (m_fRotAngle >= fAngle)
		m_fRotAngle -= (_float)dTimeDelta * 50.f;

	m_fRevAngle += (_float)dTimeDelta * 20.f;

	matRotY = XMMatrixRotationY(XMConvertToRadians(-m_fRotAngle));
	matTrans = XMMatrixTranslation(m_vTranslationPos.x, m_vTranslationPos.y, m_vTranslationPos.z);
	matParent = XMMatrixTranslation(vConverCenterPos.x, vConverCenterPos.y, vConverCenterPos.z);
	matRevRotY = XMMatrixRotationY(XMConvertToRadians(m_fRevAngle));

	matWorld = matRotY * matTrans * matRevRotY * matParent;

	m_pTransformCom->Set_WorldMatrix(matWorld);
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
			CBoss_Missile::BOSSMISSILE_DESC tMissileDesc;

			/* 유도 미사일 발사!!!!!!!!!!!!!!!!! */
			if (nullptr == m_pCodyMissile)
			{
				/* true면 Cody */
				tMissileDesc.IsTarget_Cody = true;
				tMissileDesc.vPosition = (_float4)&LeftRocketHatch.r[3].m128_f32[0];
				XMStoreFloat4(&tMissileDesc.vDir, vCodyDir);
				m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc, &pGameObject);
				m_pCodyMissile = static_cast<CBoss_Missile*>(pGameObject);
				m_fGuidedMissileTime = 0.f;
			}

			if (nullptr == m_pMayMissile)
			{
				/* false면 May */
				tMissileDesc.IsTarget_Cody = false;
				tMissileDesc.vPosition = (_float4)&RightRocketHatch.r[3].m128_f32[0];
				XMStoreFloat4(&tMissileDesc.vDir, vMayDir);
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
		DATABASE->GoUp_BossFloor(100.f);
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
	if (true == m_IsSubLaserOperation)
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

	if (1.f <= vDistance)
		m_pTransformCom->Go_Straight(dTimeDelta * 3.f);
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
	if (5.f <= m_fGroundPoundTime && false == m_IsGroundPound)
	{
		XMStoreFloat4(&m_vGroundPoundTargetPos, m_pMayTransform->Get_State(CTransform::STATE_POSITION));
		m_IsGroundPound = true;
	}

	/* UFO가 찍기 애니메이션을 진행중이다 밑에서 쓸데없이 연산하지말고 걍 나가자 ㅇㅇ */
	if (UFO_GroundPound == m_pModelCom->Get_CurAnimIndex())
	{
		if (m_pModelCom->Is_AnimFinished(UFO_GroundPound))
		{
			m_pModelCom->Set_Animation(UFO_MH);
			m_IsGroundPound = false;
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
		/* 레이저 건 안달린 애니메이션이 없다... 직접 없애주자...ㅠㅠ 잘가라 */
		//if (0.97 <= m_pModelCom->Get_ProgressAnim() && false == m_IsLaserGunRid)
		//	GetRidLaserGun();

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
		}
	}

	return S_OK;
}

HRESULT CUFO::Ready_Actor_Component()
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

HRESULT CUFO::Phase2_End(_double dTimeDelta)
{

	if (m_pModelCom->Get_CurAnimIndex() == UFO_RocketKnockDown_MH && m_IsCodyEnter == true)
	{
		m_pModelCom->Set_Animation(CutScene_EnterUFO_FlyingSaucer, 30.f);
		m_pModelCom->Set_NextAnimIndex(UFO_MH);
		m_IsCodyEnter = false;
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
}

HRESULT CUFO::Phase3_End(_double dTimeDelta)
{

	return S_OK;
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

void CUFO::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{

	}

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eBOSSUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{

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

	return;
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
	if (nullptr != m_pStaticActorCom)
		Safe_Release(m_pStaticActorCom);

	if (nullptr != m_pTriggerActorCom)
		Safe_Release(m_pTriggerActorCom);

	for (auto pSubLaser : m_vecSubLaser)
		Safe_Release(pSubLaser);

	m_vecSubLaser.clear();

	if(nullptr != m_pCodyMissile)
		Safe_Release(m_pCodyMissile);

	if (nullptr != m_pMayMissile)
		Safe_Release(m_pMayMissile);

	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}