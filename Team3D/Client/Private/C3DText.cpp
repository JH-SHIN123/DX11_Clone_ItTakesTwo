#include "stdafx.h"
#include "..\Public\C3DText.h"
#include <string>
#include "EndingCredit_Manager.h"
#include "Cody.h"
#include "MeshParticle.h"
#include "EndingRocket.h"
#include "UI_Generator.h"

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

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int C3DText::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pTransformCom->Go_Down(dTimeDelta * 3.0);

	_float fMyPosY	= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float fCodyY	= XMVectorGetY(m_pCodyTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fMyPosY > fCodyY)
	{
		((CCody*)(DATABASE->GetCody()))->Start_RadiarBlur_FullScreen(2.f);
		((CEndingRocket*)(DATABASE->Get_EndingRocket()))->Set_Boost();
		ENDINGCREDIT->Set_RandomModel();
		Create_Particle();
		Set_Dead();

		if (0 == m_iIndex)
			ENDINGCREDIT->Set_2DMeshStart();

		/* 엔딩크래딧 끝 */
		if (23 == m_iIndex)
			ENDINGCREDIT->End_EndingCredit();

		return EVENT_DEAD;
	}

	return NO_EVENT;
}

_int C3DText::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_PRE_CUSTOM_BLUR, this);

	return NO_EVENT;
}

HRESULT C3DText::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(28, 0);

	return S_OK;
}

HRESULT C3DText::Ready_Component(void * pArg)
{
	NULL_CHECK_RETURN(pArg, E_FAIL);

	ARG_DESC tArg;
	memcpy(&tArg, pArg, sizeof(ARG_DESC));

	wstring strModelTag = TEXT("Component_Model_3DText");
	strModelTag += to_wstring(tArg.iIndex);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, strModelTag.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pCodyTransformCom = ((CCody*)(DATABASE->GetCody()))->Get_Transform();
	Safe_AddRef(m_pCodyTransformCom);

	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (0 == tArg.iIndex || 10 == tArg.iIndex || 16 == tArg.iIndex || 23 == tArg.iIndex)
	{
		m_pTransformCom->Set_Scale(XMVectorSet(3.f, 1.f, 3.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, tArg.fPosY, 0.f, 1.f));
	}
	else if (11 == tArg.iIndex || 12 == tArg.iIndex || 13 == tArg.iIndex || 14 == tArg.iIndex || 15 == tArg.iIndex)
	{
		m_pTransformCom->Set_Scale(XMVectorSet(2.f, 1.f, 2.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, tArg.fPosY, -2.f, 1.f));
	}
	else
	{
		m_pTransformCom->Set_Scale(XMVectorSet(2.f, 1.f, 2.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 11 - 5), tArg.fPosY, _float(rand() % 3 - 1), 1.f));
	}

	m_iIndex = tArg.iIndex;

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
		_vector vRandomPos = XMVectorSet((rand() % 7 - 3.f), -20.f, (rand() % 3 - 1.f), 0.f);
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
	Safe_Release(m_pCodyTransformCom);

	CGameObject::Free();
}
