#include "stdafx.h"
#include "..\Public\InputButton.h"

#include "GameInstance.h"
#include "Pipeline.h"

CInputButton::CInputButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CInputButton::CInputButton(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CInputButton::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInputButton::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	m_UIDesc.iRenderGroup *= -1;

	return S_OK;
}

HRESULT CInputButton::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	SetUp_Option();

	return S_OK;
}

_int CInputButton::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CInputButton::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	if (false == m_bActive)
		return 0;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CInputButton::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (0 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}
	else if(1 == m_iOption)
	{
		if (FAILED(CUIObject::Set_InterActiveVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;
	}

	m_pVIBuffer_RectCom->Render(m_iShaderPassNum);

	return S_OK;
}

void CInputButton::SetUp_Option()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Dot")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_PS_Triangle")) ||
		!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_PS_R1")))
	{
		m_iOption = 1;
		m_iShaderPassNum = 0;
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Right_TriAngle")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Left_TriAngle")))
	{
		m_iOption = 2;
		m_iShaderPassNum = 11;
	}
	else
	{
		m_iOption = 0;
		m_iShaderPassNum = 0;
	}
}

HRESULT CInputButton::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CInputButton * CInputButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CInputButton* pInstance = new CInputButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create InputButton Prototype, Error to CInputButton::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInputButton::Clone_GameObject(void * pArg)
{
	CInputButton* pClone = new CInputButton(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CInputButton, Error to CInputButton::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CInputButton::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
