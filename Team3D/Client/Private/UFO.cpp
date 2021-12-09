#include "stdafx.h"
#include "..\Public\UFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "RobotParts.h"
#include "CutScenePlayer.h"
#include "MoonBaboon_MainLaser.h"

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
	vPos.m128_f32[1] += 5.f;
	XMStoreFloat4(&m_vStartTargetPos, vPos);
	m_IsStartingPointMove = true;

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
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
			Phase2_Pattern(dTimeDelta);
			break;
		case Client::CUFO::PHASE_3:
			Phase3_Pattern(dTimeDelta);
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

	_vector vDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vUFOLook), XMVector3Normalize(vLaserDir));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(vDot));

	_matrix matPivot, matRotY, matTrans, matAnim;
	matTrans = XMMatrixTranslation(0.f, -0.5f, 0.f);
	matRotY = XMMatrixRotationY(XMConvertToRadians(-fAngle));
	matPivot = matRotY * matTrans;

	matAnim = m_pModelCom->Get_AnimTransformation(22);
	matAnim = XMMatrixInverse(nullptr, matAnim);

	matPivot *= matAnim;

	m_pModelCom->Set_PivotTransformation(22, matPivot);

	/* 레이저 건의 뼈랑 UFO 월드 매트릭스 가져오고 곱해서 레이저건의 월드 매트릭스를 구하자 (이거 각도 잘 안구해지는거 같음 애니메이션 떄문에;;)*/
	_matrix matLaserGun = m_pModelCom->Get_BoneMatrix("LaserGun");
	_matrix matUFOWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matLaserGunWorld = matLaserGun * matUFOWorld;

	XMStoreFloat4(&m_vLaserGunPos, matLaserGunWorld.r[3]);
	XMStoreFloat4(&m_vLaserDir, vLaserDir);

	/* 레이저건의 포지션을 받아오자*/
	//_vector vLaserGunPos = XMLoadFloat4((_float4*)&matLaserGunWorld.r[3].m128_f32[0]);

	/* 레이저건의 Look을 받아오자 */
	//_vector vLaserGunLook = XMLoadFloat4((_float4*)&matLaserGunWorld.r[2].m128_f32[0]);

	if (true == m_IsLaserCreate)
	{
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeA"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeA"));
		m_IsLaserCreate = false;
	}
}
 
void CUFO::MoveStartingPoint(_double dTimeDelta)
{
	_vector vTargetPos = XMLoadFloat4(&m_vStartTargetPos);
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



}

void CUFO::Phase1_InterAction(_double dTimeDelta)
{
	/* 레이저에서 중력자탄 패턴으로 변경 */
	if (true == m_pModelCom->Is_AnimFinished(UFO_Laser_HitPod))
	{
		m_fWaitingTime += (_float)dTimeDelta;

		/* 1초 대기했다가 회전하는 메인 레이저 올라오고 중력자탄 패턴으로 바꿔라 */
		if (1.f <= m_fWaitingTime)
		{
			((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Set_LaserOperation(true);

			if (true == ((CMoonBaboon_MainLaser*)DATABASE->Get_MoonBaboon_MainLaser())->Get_LaserUp())
				m_ePattern = UFO_PATTERN::GRAVITATIONALBOMB;

			m_pModelCom->Set_Animation(UFO_MH);
			m_pModelCom->Set_NextAnimIndex(UFO_MH);

			m_fWaitingTime = 0.f;
		}
	}

	/* 중력자탄에서 레이저 패턴으로 변경 */

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
		m_pModelCom->Set_Animation(UFO_Laser_HitPod);
		m_pModelCom->Set_NextAnimIndex(UFO_MH);
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
}

void CUFO::Phase3_Pattern(_double dTimeDelta)
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

	m_pModelCom->Add_LerpInfo(UFO_Fwd, UFO_Laser_MH, true);

	m_pModelCom->Add_LerpInfo(UFO_Laser_MH, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Laser_MH, UFO_Laser_HitPod, true);


	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_Laser_MH, true);

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