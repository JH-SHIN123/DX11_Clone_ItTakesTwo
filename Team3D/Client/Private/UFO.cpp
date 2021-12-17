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
#include "Effect_Generator.h"

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
	XMStoreFloat4(&m_vStartTargetPos, vPos);
	m_IsStartingPointMove = true;

 	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying, m_pTransformCom->Get_WorldMatrix());
	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying_Particle, m_pTransformCom->Get_WorldMatrix());
	EFFECT->Add_Effect(Effect_Value::Boss_UFO_Flying_Particle_Flow, m_pTransformCom->Get_WorldMatrix());

	Set_MeshRenderGroup();

	return S_OK;
}

_int CUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	/* 테스트 용 */
	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
		m_IsCutScene = false;

	if (m_pGameInstance->Key_Down(DIK_F11))
	{
		CBoss_Missile::tagBossMissile_Desc MissileDesc;
		MissileDesc.IsTarget_Cody = true;
		MissileDesc.vPosition = { 75.f, 265.f, 207.f, 1.f };
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Boss_Missile", Level::LEVEL_STAGE, TEXT("GameObject_Boss_Missile"), &MissileDesc), E_FAIL);
	}

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
		return Add_GameObject_ToRenderGroup();

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

	/* LaserGunRing3 뼈가 레이저 총구 ㅇㅇ */
	/* LaserGun의 Right 벡터를 사용하니까 너무 이상하게 달달거려서 안움직이는 Align 뼈를 가져와서 사용함 그래도 움직이는건 UFO 행렬이 애니메이션 돌리면서 계속 바뀌기 떄문에 그런듯 */
	_matrix matUFOWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matLaserGunRing = m_pModelCom->Get_BoneMatrix("LaserGunRing3");
	_matrix matLaserGun = m_pModelCom->Get_BoneMatrix("Align");
	_matrix matLaserRingWorld = matRotY * matLaserGunRing * matUFOWorld;
	_matrix matAlign = matRotY * matLaserGun * matUFOWorld;
	_vector vLaserGunDir = XMLoadFloat4((_float4*)&matAlign.r[0].m128_f32[0]);

	/* 레이저에 시작위치랑 방향 벡터 던져주자 */
	XMStoreFloat4(&m_vLaserGunPos, matLaserRingWorld.r[3]);
	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(vLaserGunDir));

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
	m_pModelCom->Add_LerpInfo(UFO_Laser_MH, CutScene_PowerCoresDestroyed_UFO, true);

	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_MH, true);
	m_pModelCom->Add_LerpInfo(UFO_Laser_HitPod, UFO_Laser_MH, true);

	m_pModelCom->Add_LerpInfo(CutScene_PowerCoresDestroyed_UFO, UFO_Laser_MH, true);
	m_pModelCom->Add_LerpInfo(CutScene_PowerCoresDestroyed_UFO, UFO_KnockDownMH, true);

	m_pModelCom->Add_LerpInfo(UFO_KnockDownMH, UFO_CodyHolding_Enter, true);

	m_pModelCom->Add_LerpInfo(UFO_CodyHolding_Enter, UFO_CodyHolding_low, true);
	m_pModelCom->Add_LerpInfo(UFO_CodyHolding_low, UFO_CodyHolding, true);
	m_pModelCom->Add_LerpInfo(UFO_CodyHolding, UFO_CodyHolding, true);


	m_pModelCom->Add_LerpInfo(UFO_CodyHolding, UFO_LaserRippedOff, true);
	m_pModelCom->Add_LerpInfo(UFO_LaserRippedOff, UFO_Left, true);
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
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}