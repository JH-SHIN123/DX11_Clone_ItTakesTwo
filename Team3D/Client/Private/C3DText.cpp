#include "stdafx.h"
#include "..\Public\C3DText.h"
#include "EndingCredit_Manager.h"
#include "Cody.h"
#include "PlayerActor.h"
#include "MeshParticle.h"
#include "EndingRocket.h"


C3DText::C3DText(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

C3DText::C3DText(const C3DText & rhs)
	: CGameObject(rhs)
{
}

HRESULT C3DText::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT C3DText::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::e3DTEXT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int C3DText::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	_float fMyPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fCodyY = XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));

	/* 플레이어가 충돌을 하지않고 지나갔을 때*/
	if (fMyPosY > fCodyY + 20.f)
	{
		ENDINGCREDIT->Create_3DText(false);
		Create_Particle();
		Set_Dead();
		return NO_EVENT;
	}

	/* 스케일 세팅 */
	m_fScale += (_float)dTimeDelta / 4.f;

	if (m_fMaxScale <= m_fScale)
		m_fScale = m_fMaxScale;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));

	return NO_EVENT;
}

_int C3DText::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	m_pTriggerActorCom->Update_TriggerActor();

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_CUSTOM_BLUR, this);

	return NO_EVENT;
}

HRESULT C3DText::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(28, 0);

	return S_OK;
}

void C3DText::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		/* 충돌시 로켓에 부스트 세팅해줌 */
		((CEndingRocket*)(DATABASE->Get_EndingRocket()))->Set_Boost();
		ENDINGCREDIT->Create_3DText(true);
		Create_Particle();
		Set_Dead();
	}
}

HRESULT C3DText::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, tArg.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pCodyTransformCom = ((CCody*)(DATABASE->GetCody()))->Get_Transform();
	Safe_AddRef(m_pCodyTransformCom);

	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.x = 0.f;
	vPos.z = 0.f;

	/* 타이틀 제외 랜덤 위치 생성 */
	if (0 != tArg.iIndex && 10 != tArg.iIndex && 16 != tArg.iIndex && 23 != tArg.iIndex)
	{
		/* 폰트 랜덤 생성 위치, 0,0 기준 */
		_int iX = rand() % 11 - 5;
		_int iZ = rand() % 3 - 1;
		vPos.x += iX;
		vPos.z += iZ;
	}
	
	/* 생성 시간이 짧은 경우 초기스케일 조절해줘야함 */
	if (1.f >= tArg.fTime)
		m_fScale = 1.5f;
	else if (3.f >= tArg.fTime)
		m_fScale = 1.f;
	else if (5.f >= tArg.fTime)
		m_fScale = 0.5f;

	/* 폰트 생성 시간에 따른 거리 계산 */
	if (true == tArg.IsBoost)
		vPos.y -= ((10.f * (tArg.fTime - 1.f)) + 20.f);
	else
		vPos.y -= (10.f * tArg.fTime);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_fScale, m_fScale, m_fScale, 1.f));
	m_IsBoost = tArg.IsBoost;
	m_fTime	= tArg.fTime;
	m_fMaxScale = tArg.fMaxScale;
	m_iIndex = tArg.iIndex;

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(tArg.vTriggerSize.x, tArg.vTriggerSize.y, tArg.vTriggerSize.z);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

HRESULT C3DText::Create_Particle()
{
	CMeshParticle::ARG_DESC tArg;

	if (0 == m_iIndex || 10 == m_iIndex || 16 == m_iIndex || 23 == m_iIndex)
		tArg.iColorType = 0;
	else
		tArg.iColorType = 1;

	for (_uint i = 0; i < 100; ++i)
	{
		_vector vPositoin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vRandomPos = XMVectorSet((rand() % 7 - 3.f), -10.f, (rand() % 3 - 1.f), 0.f);
		XMStoreFloat3(&tArg.vPosition, vPositoin + vRandomPos);

		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_EndingCredit"), Level::LEVEL_STAGE, TEXT("GameObject_MeshParticle"), &tArg), E_FAIL);
	}
	return S_OK;
}

C3DText * C3DText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	C3DText* pInstance = new C3DText(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - C3DText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * C3DText::Clone_GameObject(void * pArg)
{
	C3DText* pInstance = new C3DText(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - C3DText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void C3DText::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTriggerActorCom);
	Safe_Release(m_pCodyTransformCom);

	CGameObject::Free();
}
