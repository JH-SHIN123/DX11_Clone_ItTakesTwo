#include "stdafx.h"
#include "..\public\StarBuddy.h"
#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"

CStarBuddy::CStarBuddy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStarBuddy::CStarBuddy(const CStarBuddy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStarBuddy::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStarBuddy::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Com"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 1.f, 10.f, 1.f));

	return S_OK;
}

_int CStarBuddy::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	//if (m_pGameInstance->Key_Down(DIK_0))
	//	m_bLaunch = true;

	//if (m_bLaunch == false)
	//{
	//	m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
	//	m_pTransformCom->RotatePitch(dTimeDelta * 0.2f);
	//}

	//else if (m_bLaunch == true)
	//{
	//	Launch_StarBuddy(dTimeDelta);
	//	m_fLifeTime += (_float)dTimeDelta;
	//	if (m_fLifeTime > 3.5f)
	//		return EVENT_DEAD; // 
	//}

	return NO_EVENT;
}


_int CStarBuddy::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	InterActive_UI();

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CStarBuddy::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CStarBuddy::InterActive_UI()
{
	CCody* pCody = (CCody*)DATABASE->GetCody();
	NULL_CHECK_RETURN(pCody, E_FAIL);
	CMay* pMay = (CMay*)DATABASE->GetMay();
	NULL_CHECK_RETURN(pMay, E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCodyComparePos = vPos - vCodyPos;
	_vector vMayComparePos = vPos - vMayPos;

	_float fRange = 20.f;

	_float vCodyComparePosX = abs(XMVectorGetX(vCodyComparePos));
	_float vCodyComparePosZ = abs(XMVectorGetZ(vCodyComparePos));

	if (fRange >= vCodyComparePosX && fRange >= vCodyComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::Cody, UI::InputButton_Dot))
			UI_Create(Cody, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::Cody, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(Cody, InputButton_Dot);

	_float vMayComparePosX = abs(XMVectorGetX(vMayComparePos));
	_float vMayComparePosZ = abs(XMVectorGetZ(vMayComparePos));

	if (fRange >= vMayComparePosX && fRange >= vMayComparePosZ)
	{
		if (UI_Generator->Get_EmptyCheck(Player::May, UI::InputButton_Dot))
			UI_Create(May, InputButton_Dot);

		UI_Generator->Set_TargetPos(Player::May, UI::InputButton_Dot, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
		UI_Delete(May, InputButton_Dot);

	return S_OK;
}

HRESULT CStarBuddy::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CStarBuddy::Launch_StarBuddy(_double TimeDelta)
{
	// 실제로 상호작용 할땐 Player -> StarBuddy Dir 방향으로 이동
	m_pTransformCom->Move_ToTarget(XMVectorSet(100.f, 10.f, 100.f, 0.f), TimeDelta * 5.f);
	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 4.f);
	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * 4.f);
		
}


CStarBuddy * CStarBuddy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStarBuddy* pInstance = new CStarBuddy(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStarBuddy::Clone_GameObject(void * pArg)
{
	CStarBuddy* pInstance = new CStarBuddy(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStarBuddy::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}