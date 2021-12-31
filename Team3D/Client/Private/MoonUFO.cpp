#include "stdafx.h"
#include "..\Public\MoonUFO.h"
#include "PlayerActor.h"
#include "May.h"
#include "Cody.h"
#include "Moon.h"
#include "PixelCrossHair.h"
#include "PixelUFO.h"
#include "RunningMoonBaboon.h"
#include "Script.h"

CMoonUFO::CMoonUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonUFO::CMoonUFO(const CMoonUFO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonUFO::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonUFO::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eMOONUFO;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pModelCom->Set_Animation(UFO_MH);
	m_pModelCom->Set_NextAnimIndex(UFO_MH);
	
	DATABASE->Set_MoonUFO(this);

	m_pTransformCom->Set_Speed(0.f, 45.f);
	Set_MeshRenderGroup();

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume);
	m_pGameInstance->Play_Sound(L"UFO_Move.wav", CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_MOVE_END, m_fMove_End_Volume);
	m_pGameInstance->Play_Sound(L"UFO_Move_End.wav", CHANNEL_MOONUFO_MOVE_END, m_fMove_End_Volume);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE_END);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
	m_pGameInstance->Play_Sound(L"UFO_Move_Start.wav", CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE_START);


	return S_OK;
}

_int CMoonUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == m_IsMayInUFO/* && true == m_IsCutSceneEnd*/)
	{
		KeyInPut(dTimeDelta);
		Calculate_Matrix(dTimeDelta);
	}

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CMoonUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CMoonUFO::Render(RENDER_GROUP::Enum eGroup)
{
	if (false == m_IsMayInUFO)
		return S_OK;

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

HRESULT CMoonUFO::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true, RENDER_GROUP::RENDER_NONALPHA);
	return S_OK;
}

HRESULT CMoonUFO::Set_MeshRenderGroup()
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

HRESULT CMoonUFO::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
	return S_OK;
}

void CMoonUFO::KeyInPut(_double dTimeDelta)
{
	_vector vLook	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vUp		= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));
	_vector vRight	= XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	if (m_pGameInstance->Key_Pressing(DIK_RIGHT) || m_pGameInstance->Get_Pad_LStickX() > 44000)
	{
		/* SCRIPT && SOUND */
		if (m_bMoving == false)
		{
			m_pGameInstance->Play_Sound(L"UFO_Move_Start.wav", CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
			m_bMoving = true;
		}

		if (m_bMoving && CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE_START) == false)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE) == false)
			{
				m_pGameInstance->Play_Sound(L"UFO_Move.wav", CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume, true);
			}
		}


		_bool IsMayUFO = ((CMay*)DATABASE->GetMay())->Get_IsInUFO();
		_bool IsCodyUFO = ((CCody*)DATABASE->GetCody())->Get_IsInArcadeJoyStick();
		if (SCRIPT->Get_Script_Played(51) == false && IsMayUFO == true && IsCodyUFO == true)
		{
			SCRIPT->VoiceFile_No51();
			SCRIPT->Set_Script_Played(51, true);
		}

		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * UFOFORCE, XMVectorGetY(vRight) * UFOFORCE, XMVectorGetZ(vRight) * UFOFORCE));
		//m_bRotateRight = true;
	}
	//else
		//m_bRotateRight = false;

	if (m_pGameInstance->Key_Pressing(DIK_LEFT) || m_pGameInstance->Get_Pad_LStickX() < 20000)
	{
		/* SCRIPT && SOUND */
		if (m_bMoving == false)
		{
			m_pGameInstance->Play_Sound(L"UFO_Move_Start.wav", CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
			m_bMoving = true;
		}

		if (m_bMoving && CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE_START) == false)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE) == false)
			{
				m_pGameInstance->Play_Sound(L"UFO_Move.wav", CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume, true);
			}
		}
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vRight) * -UFOFORCE, XMVectorGetY(vRight)  * -UFOFORCE, XMVectorGetZ(vRight) * -UFOFORCE));
		//m_bRotateLeft = true;
	} 
	//else
		//m_bRotateLeft = false;

	if (m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Get_Pad_LStickY() < 20000)
	{
		/* SCRIPT && SOUND */
		if (m_bMoving == false)
		{
			m_pGameInstance->Play_Sound(L"UFO_Move_Start.wav", CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
			m_bMoving = true;
		}

		if (m_bMoving && CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE_START) == false)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE) == false)
			{
				m_pGameInstance->Play_Sound(L"UFO_Move.wav", CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume, true);
			}
		}
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * UFOFORCE, XMVectorGetY(vLook) * UFOFORCE, XMVectorGetZ(vLook) * UFOFORCE));
	}
	if (m_pGameInstance->Key_Pressing(DIK_DOWN) || m_pGameInstance->Get_Pad_LStickY() > 44000)
	{
		/* SCRIPT && SOUND */
		if (m_bMoving == false)
		{
			m_pGameInstance->Play_Sound(L"UFO_Move_Start.wav", CHANNEL_MOONUFO_MOVE_START, m_fMove_Start_Volume);
			m_bMoving = true;
		}

		if (m_bMoving && CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE_START) == false)
		{
			if (CSound_Manager::GetInstance()->Is_Playing(CHANNEL_MOONUFO_MOVE) == false)
			{
				m_pGameInstance->Play_Sound(L"UFO_Move.wav", CHANNEL_MOONUFO_MOVE, m_fMove_Loop_Volume, true);
			}
		}
		m_pDynamicActorCom->Get_Actor()->addForce(PxVec3(XMVectorGetX(vLook) * -UFOFORCE, XMVectorGetY(vLook) * -UFOFORCE, XMVectorGetZ(vLook) * -UFOFORCE));
	}
#ifdef __CONTROL_MAY_KEYBOARD
	if ((m_pGameInstance->Key_Up(DIK_UP) && !m_pGameInstance->Key_Pressing(DIK_DOWN) && !m_pGameInstance->Key_Pressing(DIK_RIGHT) && !m_pGameInstance->Key_Pressing(DIK_LEFT))
		|| (m_pGameInstance->Key_Up(DIK_DOWN) && !m_pGameInstance->Key_Pressing(DIK_UP) && !m_pGameInstance->Key_Pressing(DIK_RIGHT) && !m_pGameInstance->Key_Pressing(DIK_LEFT))
		|| (m_pGameInstance->Key_Up(DIK_LEFT) && !m_pGameInstance->Key_Pressing(DIK_UP) && !m_pGameInstance->Key_Pressing(DIK_DOWN) && !m_pGameInstance->Key_Pressing(DIK_RIGHT))
		|| (m_pGameInstance->Key_Up(DIK_RIGHT) && !m_pGameInstance->Key_Pressing(DIK_UP) && !m_pGameInstance->Key_Pressing(DIK_DOWN) && !m_pGameInstance->Key_Pressing(DIK_LEFT)))
	{
		m_bMoving = false;
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE_START);
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE);
		m_pGameInstance->Play_Sound(L"UFO_Move_End.wav", CHANNEL_MOONUFO_MOVE_END, m_fMove_End_Volume);
	}
#else
	if ((m_pGameInstance->Get_Pad_LStickY() > 20000 && m_pGameInstance->Get_Pad_LStickY() < 40000) && (m_pGameInstance->Get_Pad_LStickX() > 20000 && m_pGameInstance->Get_Pad_LStickX() < 40000) )
	{
		m_bMoving = false;
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE_START);
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_MOVE);
		m_pGameInstance->Play_Sound(L"UFO_Move_End.wav", CHANNEL_MOONUFO_MOVE_END, m_fMove_End_Volume);
	}
#endif
	//
	/* For.LaserGun */

	_vector vUFOPos = ((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = ((CPixelCrossHair*)DATABASE->Get_PixelCrossHair())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	_vector vUFOToTarget = XMVector3Normalize(vTargetPos - vUFOPos); // 크로스헤어 - UFO(중점) 방향벡터
	_vector vUFOToTargetNoNormalize = vTargetPos - vUFOPos;
	_vector vWorldUp = XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_float fAngle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vUFOToTarget, vWorldUp)));

	if (vUFOPos.m128_f32[0] > vTargetPos.m128_f32[0])
		fAngle = -1.f * fAngle;


	_float fLength = XMVectorGetX(XMVector3Length(vUFOToTargetNoNormalize)); // 중점에서 크로스헤어 까지의 거리.

	//if (vUFOPos.m128_f32[1] < vTargetPos.m128_f32[1])
	//	fSecondAngle = -1.f * fSecondAngle;

	_matrix matPivot, matRotUp, matTrans, matAnim = XMMatrixIdentity();

	matRotUp = XMMatrixRotationZ(XMConvertToRadians(-fAngle));
	// 레이더에서 보이는 중점에서 크로스헤어 까지의 거리에 비례해서 회전 시켜 줘야 합니다.

	matPivot = matRotUp/* * matRotRight*/;
	matAnim = m_pModelCom->Get_AnimTransformation(22);
	matAnim = XMMatrixInverse(nullptr, matAnim);

	matPivot *= matAnim;
	m_pModelCom->Set_PivotTransformation(22, matPivot); // 여기까지가 레이저건을 레이더에 보이는 크로스헤어 만큼 회전 시킨 것임.

	_matrix matUFOWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matLaserGunRing = m_pModelCom->Get_BoneMatrix("LaserGunRing3");
	_matrix matLaserRingWorld = matRotUp/* * matRotRight*/ * matLaserGunRing * matUFOWorld;
	_matrix matLaserGun = m_pModelCom->Get_BoneMatrix("Align");

	

	_matrix matAlign = matRotUp/* * matRotRight*/ * matLaserGun * matUFOWorld;
	_vector vLaserGunDir = XMLoadFloat4((_float4*)&matAlign.r[0].m128_f32[0]);

	/* 레이저에 시작위치랑 방향 벡터 던져주자 */
	XMStoreFloat4(&m_vLaserGunPos, matLaserRingWorld.r[3]);

	if (m_bCompensate == false)
	{
		vLaserGunDir = (XMVector3Normalize(-m_pTransformCom->Get_State(CTransform::STATE_UP)) / (2.95f * fLength) + XMVector3Normalize(vLaserGunDir));
		XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(vLaserGunDir));
	}
	else
	{
		vLaserGunDir = XMVector3Normalize(((CRunningMoonBaboon*)DATABASE->Get_RunningMoonBaboon())->Get_Transform()->Get_State(CTransform::STATE_POSITION) - XMVectorSetW(XMLoadFloat4(&m_vLaserGunPos), 1.f));
		XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(vLaserGunDir));
	}
	//XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	//XMStoreFloat4(&m_vLaserGunPos, matLaserGunWorld.r[3]);
	//XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(matLaserGunWorld.r[2]));


	if (m_IsShootLaser == true)
	{
		m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeC"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeC"));
		m_IsShootLaser = false;
	}
}

void CMoonUFO::Calculate_Matrix(_double dTimeDelta)
{
	PxMat44 pxMat = PxMat44(m_pDynamicActorCom->Get_Actor()->getGlobalPose());

	_vector vPosition = XMVectorSet(pxMat.column3.x, pxMat.column3.y, pxMat.column3.z, 1.f);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	vUp = XMVector3Normalize(vPosition - ((CMoon*)(DATABASE->Get_Mooon()))->Get_Position());
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	if (true == m_bRotateRight)
		m_pTransformCom->RotateYaw_Speed(dTimeDelta);
	if (true == m_bRotateLeft)
		m_pTransformCom->RotateYaw_Speed(-dTimeDelta);
}

HRESULT CMoonUFO::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	m_pModelCom = (CModel*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_STAGE, TEXT("Component_Model_UFO"));
	_matrix PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pModelCom->Change_PivotMatrix(PivotMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(800.f, 1000.f, 2000.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f + 11.f, 357.5f - 255.f, 195.f, 1.f));
	
	//m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -135.f);


	/* Dynamic */
	PxGeometry* Geom = new PxSphereGeometry(5.f);
	CDynamicActor::ARG_DESC tDynamicActorArg;
	tDynamicActorArg.pTransform = m_pTransformCom;
	tDynamicActorArg.fDensity = 1.f;
	tDynamicActorArg.pGeometry = Geom;
	tDynamicActorArg.vVelocity = PxVec3(0.f, 0.f, 0.f);
	tDynamicActorArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_DynamicActor"), TEXT("Com_DynamicActor"), (CComponent**)&m_pDynamicActorCom, &tDynamicActorArg), E_FAIL);
	Safe_Delete(Geom);


	m_pDynamicActorCom->Get_Actor()->setLinearDamping(1.2f);

	m_pDynamicActorCom->Get_Actor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	PxShape* pShape = nullptr;
	m_pDynamicActorCom->Get_Actor()->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);


	/* Joint */
	PxJointLimitCone LimitCone = PxJointLimitCone(PxPi, PxPi, 0.05f);
	m_pJoint = CPhysX::GetInstance()->Create_Joint(m_pDynamicActorCom->Get_Actor(), PxTransform(PxVec3(0.f, 90.f, 0.f)), nullptr, PxTransform(MH_PxVec3(((CMoon*)(DATABASE->Get_Mooon()))->Get_Position())), LimitCone, false);
	return S_OK;
}

CMoonUFO * CMoonUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonUFO* pInstance = new CMoonUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoonUFO::Clone_GameObject(void * pArg)
{
	CMoonUFO* pInstance = new CMoonUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonUFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoonUFO::Free()
{
	Safe_Release(m_pDynamicActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	CGameObject::Free();
}