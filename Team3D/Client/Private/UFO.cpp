#include "stdafx.h"
#include "..\Public\UFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"
#include "CutScenePlayer.h"
#include "MoonBaboon_MainLaser.h"
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

	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
		m_IsCutScene = false;
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
	_uint iGravitationalBombMaxCount = 10;

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
	_vector vDir, vCenterPos, vUFORight, vDot;
	/* 중심점은 1페 때 잡아줬던 시작점 */
	vCenterPos = XMLoadFloat4(&m_vStartUFOPos);
	vUFORight = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float3 vConverCenterPos;
	XMStoreFloat3(&vConverCenterPos, vCenterPos);

	vDir = vCenterPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vDot = XMVector3AngleBetweenNormals(XMVector3Normalize(vUFORight), XMVector3Normalize(vDir));

	_float fAngle = XMConvertToDegrees(XMVectorGetX(vDot));

	_matrix matWorld, matRotY, matTrans, matRevRotY, matParent;

	if (m_fRotAngle < fAngle)
		m_fRotAngle += (_float)dTimeDelta;
	else if (m_fRotAngle >= fAngle)
		m_fRotAngle -= (_float)dTimeDelta;

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

		if (nullptr == m_pCodyMissile)
		{
			/* true면 Cody */
			tMissileDesc.IsTarget_Cody = true;
			tMissileDesc.vPosition = (_float4)&LeftRocketHatch.r[3].m128_f32[0];
			XMStoreFloat4(&tMissileDesc.vDir, vCodyDir);
			m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc, &pGameObject);
			m_pCodyMissile = static_cast<CBoss_Missile*>(pGameObject);
		}

		if (nullptr == m_pMayMissile)
		{
			/* false면 May */
			tMissileDesc.IsTarget_Cody = false;
			tMissileDesc.vPosition = (_float4)&RightRocketHatch.r[3].m128_f32[0];
			XMStoreFloat4(&tMissileDesc.vDir, vMayDir);
			m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc, &pGameObject);
			m_pMayMissile = static_cast<CBoss_Missile*>(pGameObject);
		}
	}

	//m_fGuidedMissileTime += (_float)dTimeDelta;

	//if (2.f <= m_fGuidedMissileTime)
	//{
	//	/* 유도 미사일 발사!!!!!!!!!! */
	//	if (true == m_IsGuidedMissileCreate)
	//	{
	//		CBoss_Missile::BOSSMISSILE_DESC tMissileDesc;
	//		tMissileDesc.IsTarget_Cody = true;
	//		tMissileDesc.vPosition = vConvertPos;
	//		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc);
	//		
	//		/* false면 May */
	//		tMissileDesc.IsTarget_Cody = false;
	//		tMissileDesc.vPosition = vConvertPos;
	//		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_GuiedMissile"), Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &tMissileDesc);

	//		m_IsGuidedMissileCreate = false;
	//	}

	//	m_fGuidedMissileTime = 0.f;
	//}
}

void CUFO::Phase3_Pattern(_double dTimeDelta)
{
}

void CUFO::Phase1_End(_double dTimeDelta)
{
	if (m_pModelCom->Is_AnimFinished(CutScene_PowerCoresDestroyed_UFO))
		m_IsInterActive = true;

	/* 컷 신 애니메이션이 끝났다면 이제 상호작용 하자 ㅇㅇ */
	if(true == m_IsInterActive)
	{
		/* 여기서 그시기 하면 되겠군 */
		if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
		{
			m_pModelCom->Set_Animation(UFO_CodyHolding_Enter);
			m_pModelCom->Set_NextAnimIndex(UFO_CodyHolding_low);
		}

		if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		{
			m_pModelCom->Set_Animation(UFO_CodyHolding);
			m_pModelCom->Set_NextAnimIndex(UFO_CodyHolding);
		}

		if (m_pModelCom->Is_AnimFinished(UFO_CodyHolding))
		{
			m_pModelCom->Set_Animation(UFO_LaserRippedOff);

		}

		if (m_pModelCom->Is_AnimFinished(UFO_LaserRippedOff))
		{
			/* 스태틱, 트리거 액터 생성 */
			/* 얘 왜 터지냐 ㅡㅡ */
			/*Ready_Actor_Component();*/

			/* 애니메이션이 딱 끝났을 때 뼈의 위치를 받아서 UFO 월드에 세팅해준다. */
			_matrix BaseBone = m_pModelCom->Get_BoneMatrix("Base");
			_matrix UFOWorld = m_pTransformCom->Get_WorldMatrix();
			_matrix AnimUFOWorld = BaseBone * UFOWorld;
			m_pTransformCom->Set_WorldMatrix(AnimUFOWorld);

			/* 중점에서 부터 마지막 애니메이션의 포지션을 빼서 그 지점에서부터 공전할 수 있도록 구해줌. 2페이지에서 보스 움직일 때 사용 */
			_vector vUFOPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&m_vStartUFOPos);
			XMStoreFloat4(&m_vTranslationPos, vUFOPos);

			m_pModelCom->Set_Animation(UFO_Left);
			m_pModelCom->Set_NextAnimIndex(UFO_Left);

			/* 보스 2페이즈로 바꿔주자 */
			m_ePhase = CUFO::PHASE_2;
			m_ePattern = CUFO::GUIDEDMISSILE;
			m_IsCutScene = false;

			/* 레이저 건 안달린 애니메이션이 없다... 직접 없애주자...ㅠㅠ 잘가라 나중에 컷신 나오면 이펙트랑 같이 맞춰주자 ㅇㅇ */
			_matrix LaserBaseBone = m_pModelCom->Get_BoneMatrix("LaserBase");
			_matrix matScale;

			matScale = XMMatrixScaling(0.f, 0.f, 0.f);
			LaserBaseBone *= matScale;

			m_pModelCom->Set_PivotTransformation(m_pModelCom->Get_BoneIndex("LaserBase"), LaserBaseBone);
		}
	}
}

HRESULT CUFO::Ready_Actor_Component()
{
	m_UserData = USERDATA(GameID::eBOSSUFO, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerActorCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

void CUFO::Phase2_End(_double dTimeDelta)
{
}

void CUFO::Phase3_End(_double dTimeDelta)
{
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
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Back, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Fwd, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_MH, UFO_Left, true);


	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_Laser_MH, true);

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
	m_pModelCom->Add_LerpInfo(UFO_LaserRippedOff, UFO_MH, true);
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

void CUFO::Set_IsGuidedMissileDeadCheck(_bool IsCheck)
{
	if (true == IsCheck)
		m_pCodyMissile = nullptr;
	else
		m_pMayMissile = nullptr;
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
	if (m_ePhase != CUFO::PHASE_1)
	{
		Safe_Release(m_pStaticActorCom);
		Safe_Release(m_pTriggerActorCom);
	}

	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}