#include "stdafx.h"
#include "..\public\HookUFO.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "RobotParts.h"
#include "May.h"
#include "UI_Generator.h"
#include "Gauge_Circle.h"

CHookUFO::CHookUFO(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHookUFO::CHookUFO(const CHookUFO & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHookUFO::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CHookUFO::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Hook_UFO"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_CodyGauge_Circle(TEXT("Layer_CodyGauge_Circle")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MayGauge_Circle(TEXT("Layer_MayGauge_Circle")), E_FAIL);

	ROBOTDESC HookUFODesc;
	if (nullptr != pArg)
		memcpy(&HookUFODesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, HookUFODesc.vPosition);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransform, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans) - 10.f, XMVectorGetZ(vTrans));
	m_pPhysxTransform->Set_WorldMatrix(PhysxWorldMatrix);	


	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eHOOKUFO, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransform;
	ArgDesc.pGeometry = new PxSphereGeometry(20.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	DATABASE->Set_HookUFO(this);
	Set_MeshRenderGroup();
	//
	return S_OK;
}

_int CHookUFO::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F) && m_IsCodyCollide || m_IsMayCollide && m_pGameInstance->Pad_Key_Down(DIP_Y))
	{
		m_bLaunch = true;

		if (true == m_IsCodyCollide)
		{
			m_IsCodyUIDisable = true;
			UI_Delete(Cody, InputButton_InterActive_HookUFO);
		}

		if (true == m_IsMayCollide)
		{
			m_IsMayUIDisalbe = true;
			UI_Delete(May, InputButton_PS_InterActive_HookUFO);
		}
	}

	InterActive_UI(((CCody*)DATABASE->GetCody())->Get_Position(), GameID::eCODY, m_IsCodyUIDisable);
	InterActive_UI(((CCody*)DATABASE->GetMay())->Get_Position(), GameID::eMAY, m_IsMayUIDisalbe);


	return NO_EVENT;
}


_int CHookUFO::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);


	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CHookUFO::Render(RENDER_GROUP::Enum eGroup)
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
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 2;
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	// 0: Alpha 
	iMaterialIndex = 0;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 18, false, eGroup);

	return S_OK;
}


HRESULT CHookUFO::Set_MeshRenderGroup()
{
	m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_EFFECT_PRE_CUSTOM_BLUR);
	m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	return S_OK;
}

HRESULT CHookUFO::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_EFFECT_PRE_CUSTOM_BLUR, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
	return S_OK;
}

HRESULT CHookUFO::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true, RENDER_GROUP::RENDER_NONALPHA);

	return S_OK;
}

void CHookUFO::InterActive_UI(_vector vTargetPos, GameID::Enum eID, _bool IsDisable)
{
	if (nullptr == m_pCodyGauge_Circle || nullptr == m_pMayGauge_Circle) return;

	if (true == IsDisable)
		return;

	_vector vComparePos;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;
	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.y -= 5.f;
	vPos = XMLoadFloat4(&vConvertPos);

	vComparePos = vPos - vTargetPos;

	_float fRange = 40.f;

	_float vComparePosX = fabs(XMVectorGetX(vComparePos));
	_float vComparePosY = fabs(XMVectorGetY(vComparePos));
	_float vComparePosZ = fabs(XMVectorGetZ(vComparePos));

	/* 범위 안에 있다*/
	if (fRange >= vComparePosX && fRange >= vComparePosY && fRange >= vComparePosZ)
	{
		if (eID == GameID::Enum::eCODY)
		{
			if (true == m_IsCodyCollide)
			{
				m_pCodyGauge_Circle->Set_Active(false);
				UI_CreateOnlyOnce(Cody, InputButton_InterActive_HookUFO);
				UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_InterActive, vPos);
				return;
			}
			else
			{
				UI_Delete(Cody, InputButton_PS_InterActive_HookUFO);
				m_pCodyGauge_Circle->Set_Active(true);
			}
		}
		else if (eID == GameID::Enum::eMAY)
		{
			if (true == m_IsMayCollide)
			{
				m_pMayGauge_Circle->Set_Active(false);
				UI_CreateOnlyOnce(May, InputButton_PS_InterActive_HookUFO);
				UI_Generator->Set_TargetPos(Player::May, UI::InputButton_PS_InterActive, vPos);
				return;
			}
			else
			{
				UI_Delete(May, InputButton_PS_InterActive_HookUFO);
				m_pMayGauge_Circle->Set_Active(true);
			}
		}
	}
	else
	{
		if (eID == GameID::Enum::eCODY)
			m_pCodyGauge_Circle->Set_Active(false);
		else
			m_pMayGauge_Circle->Set_Active(false);
	}

	if (nullptr != m_pCodyGauge_Circle)
	{
		vPos.m128_f32[1] += 5.f;
		m_pCodyGauge_Circle->Set_TargetPos(vPos);
	}
	if (nullptr != m_pMayGauge_Circle)
		m_pMayGauge_Circle->Set_TargetPos(vPos);
}

HRESULT CHookUFO::Ready_Layer_CodyGauge_Circle(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	_uint m_iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Gauge_Circle"), &m_iOption, &pGameObject), E_FAIL);
	m_pCodyGauge_Circle = static_cast<CGauge_Circle*>(pGameObject);
	m_pCodyGauge_Circle->Set_SwingPointPlayerID(Player::Cody);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] -= 5.f;
	m_pCodyGauge_Circle->Set_Position(vPos);

	// 범위 설정
	m_pCodyGauge_Circle->Set_Range(20.f);

	return S_OK;
}

void CHookUFO::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		if (((CCody*)pGameObject)->Get_Position().m128_f32[1] < m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1])
		{
			((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eHOOKUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsCodyCollide = true;
			m_PlayerID = GameID::Enum::eCODY;
		}
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
 		m_IsCodyCollide = false;
	}

	// May
	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		if (((CMay*)pGameObject)->Get_Position().m128_f32[1] < m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1])
		{
			((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eHOOKUFO, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_IsMayCollide = true;
			m_PlayerID = GameID::Enum::eMAY;
		}
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		m_IsMayCollide = false;
	}
}

HRESULT CHookUFO::Ready_Layer_MayGauge_Circle(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;

	_uint m_iOption = 0;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STATIC, TEXT("Layer_UI"), Level::LEVEL_STATIC, TEXT("Gauge_Circle"), &m_iOption, &pGameObject), E_FAIL);
	m_pMayGauge_Circle = static_cast<CGauge_Circle*>(pGameObject);
	m_pMayGauge_Circle->Set_SwingPointPlayerID(Player::May);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] -= 5.f;
	m_pCodyGauge_Circle->Set_Position(vPos);

	// 범위 설정
	m_pCodyGauge_Circle->Set_Range(20.f);

	return S_OK;
}

CHookUFO * CHookUFO::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHookUFO* pInstance = new CHookUFO(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CHookUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHookUFO::Clone_GameObject(void * pArg)
{
	CHookUFO* pInstance = new CHookUFO(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CHookUFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHookUFO::Free()
{
	Safe_Release(m_pMayGauge_Circle);
	Safe_Release(m_pCodyGauge_Circle);
	Safe_Release(m_pPhysxTransform);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}