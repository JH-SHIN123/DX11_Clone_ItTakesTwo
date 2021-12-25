#include "stdafx.h"
#include "..\Public\MoonBaboon.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"
#include "CutScenePlayer.h"
#include "RobotParts.h"
#include "Moon.h"

CMoonBaboon::CMoonBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	
}

CMoonBaboon::CMoonBaboon(const CMoonBaboon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMoonBaboon::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	Add_LerpInfo_To_Model();

	ROBOTDESC MoonBaboonDesc;
	if (nullptr != pArg)
		memcpy(&MoonBaboonDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, MoonBaboonDesc.vPosition);

	DATABASE->Set_MoonBaboon(this);

	m_pUFOModel = ((CUFO*)DATABASE->Get_BossUFO())->Get_Model();
	NULL_CHECK_RETURN(m_pUFOModel, E_FAIL);
	Safe_AddRef(m_pUFOModel);

	m_pUFOTransform = ((CUFO*)DATABASE->Get_BossUFO())->Get_Transform();
	NULL_CHECK_RETURN(m_pUFOTransform, E_FAIL);
	Safe_AddRef(m_pUFOTransform);

	m_pModelCom->Set_Animation(Moon_Ufo_MH);
	m_pModelCom->Set_NextAnimIndex(Moon_Ufo_MH);

	((CUFO*)DATABASE->Get_BossUFO())->Set_MoonBaboonPtr(this);

	return S_OK;
}

_int CMoonBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == CCutScenePlayer::GetInstance()->Get_IsPlayCutScene())
	{
		SetUp_IntroOffset(dTimeDelta);
		m_pModelCom->Update_Animation(dTimeDelta);
		return S_OK;
	}

	Fix_MoonBaboon_Chair(dTimeDelta);

	//m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3000.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}



CMoonBaboon::MOON_STATE CMoonBaboon::Check_State(_double dTimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		m_pModelCom->Set_Animation(m_eNextState);
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CMoonBaboon::Fix_MoonBaboon_Chair(_double dTimeDelta)
{
	if (((CUFO*)DATABASE->Get_BossUFO())->Get_BossPhase() == CUFO::UFO_PHASE::PHASE_3 &&
		((CUFO*)DATABASE->Get_BossUFO())->Get_IsCutScene() == true)
	{
		// 달 착지 하면서 구르는 타이밍! 848.f
		if (m_pModelCom->Get_CurrentTime(Moon_Eject) >= 548.f)
		{
			_vector vPosition = m_vChairOffSetPos;
			vPosition.m128_f32[0] += 11.f;
			vPosition.m128_f32[1] -= 229.f;
			vPosition.m128_f32[2] += 10.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
		if (m_bEjectOnce == false)
		{
			m_pModelCom->Set_Animation(Moon_Eject);
			m_pModelCom->Set_NextAnimIndex(Moon_Eject);

			m_fEjectDelay += (_float)dTimeDelta;
			// 아직 사출되기전 대기시간
			if (m_fEjectDelay <= 1.f)
			{
				_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
				_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
				XMStoreFloat4x4(&matWorld, XMMatrixRotationX(-90.f) * XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f) * BoneChair * m_pUFOTransform->Get_WorldMatrix());
				matWorld._42 += 2.f;
				m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
				m_vChairOffSetPos = { matWorld._41, matWorld._42, matWorld._43, 1.f };
			}
			// 원숭이 달로 발사
			else if (m_fEjectDelay > 1.f)
			{
				_vector vPosition = m_vChairOffSetPos;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
				m_pTransformCom->Set_Rotaion(XMVectorSet(0.f, 0.f, 0.f, 0.f));
				m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(-90.f));
				m_bEjectOnce = true;
			}

		}
	}
	else
	{
		_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
		_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
		XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f)  * BoneChair * m_pUFOTransform->Get_WorldMatrix());
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
		m_vChairOffSetPos = { matWorld._41, matWorld._42, matWorld._43, 1.f };
	}
}

void CMoonBaboon::SetUp_IntroOffset(_double dTimeDelta)
{

	_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
	_uint iBoneIndex = m_pUFOModel->Get_BoneIndex("Chair");
	_matrix UFOAnim = m_pUFOModel->Get_AnimTransformation(iBoneIndex);

	_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
	XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f) * UFOAnim * BoneChair * m_pUFOTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
	m_vChairOffSetPos = { matWorld._41, matWorld._42, matWorld._43, 1.f };

	//if (CutScene_BossIntro_MoonBaboon == m_pModelCom->Get_CurAnimIndex())
	//{
	//	if (2279.4f <= m_pModelCom->Get_CurrentTime(CutScene_BossIntro_MoonBaboon) &&
	//		2330.3f >= m_pModelCom->Get_CurrentTime(CutScene_BossIntro_MoonBaboon))
	//	{
	//		_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
	//		_uint iBoneIndex = m_pUFOModel->Get_BoneIndex("Chair");
	//		_matrix UFOAnim = m_pUFOModel->Get_AnimTransformation(iBoneIndex);

	//		_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
	//		XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f) * UFOAnim * BoneChair * m_pUFOTransform->Get_WorldMatrix());
	//		matWorld._42 -= 12.f;
	//		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
	//		m_vChairOffSetPos = { matWorld._41, matWorld._42, matWorld._43, 1.f };
	//	}
	//	else
	//	{
	//		_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
	//		_uint iBoneIndex = m_pUFOModel->Get_BoneIndex("Chair");
	//		_matrix UFOAnim = m_pUFOModel->Get_AnimTransformation(iBoneIndex);

	//		_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
	//		XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f) * UFOAnim * BoneChair * m_pUFOTransform->Get_WorldMatrix());
	//		matWorld._42 += 1.5f;
	//		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
	//		m_vChairOffSetPos = { matWorld._41, matWorld._42, matWorld._43, 1.f };
	//	}
	//}
}

HRESULT CMoonBaboon::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}


HRESULT CMoonBaboon::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}

void CMoonBaboon::Set_Animation(_uint iCurAnimIndex, _uint iNextAnimIndex)
{
	m_pModelCom->Set_Animation(iCurAnimIndex);
	m_pModelCom->Set_NextAnimIndex(iNextAnimIndex);
}


void CMoonBaboon::Add_LerpInfo_To_Model()
{
	m_pModelCom->Add_LerpInfo(Moon_Ufo_MH, Moon_Ufo_Programming, true);
	m_pModelCom->Add_LerpInfo(Moon_Ufo_MH, Moon_Ufo_Laser_HitPod, true);
	m_pModelCom->Add_LerpInfo(Moon_Ufo_MH, Moon_Ufo_KnockDownMH, true);
	m_pModelCom->Add_LerpInfo(Moon_Ufo_MH, Moon_Ufo_GroundPound, true);

	m_pModelCom->Add_LerpInfo(Moon_Ufo_Programming, Moon_Ufo_MH, true);
}

HRESULT CMoonBaboon::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	return S_OK;
}

CMoonBaboon * CMoonBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonBaboon* pInstance = new CMoonBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoonBaboon::Clone_GameObject(void * pArg)
{
	CMoonBaboon* pInstance = new CMoonBaboon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon::Free()
{
	Safe_Release(m_pUFOTransform);
	Safe_Release(m_pUFOModel);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}