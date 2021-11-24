#include "stdafx.h"
#include "..\Public\HeaderBox.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CHeaderBox::CHeaderBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CHeaderBox::CHeaderBox(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CHeaderBox::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHeaderBox::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CHeaderBox::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CHeaderBox::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CHeaderBox::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CHeaderBox::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(10);

	Render_Font();

	return S_OK;
}

void CHeaderBox::Set_ScaleEffect()
{
	//if (2 != m_iOption && 10.f >= m_UIDesc.vScale.x)
	//	return;

	//m_UIDesc.vScale.x -= m_fPower;
	//m_UIDesc.vScale.y -= m_fPower;
	//m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
}


void CHeaderBox::Render_Font()
{
	//CUI_Generator::FONTDESC		tFontDesc;

	//if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_F")))
	//{
	//	tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
	//	tFontDesc.vScale = { 40.f, 50.f };
	//	tFontDesc.fInterval = 0.f;

	//	UI_Generator->Render_Font(TEXT("F"), tFontDesc, m_ePlayerID);
	//}
	//else if (!lstrcmp(m_UIDesc.szUITag, TEXT("HeaderBox_E")))
	//{
	//	tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
	//	tFontDesc.vScale = { m_UIDesc.vScale.x / 2.f, m_UIDesc.vScale.y / 1.2f };
	//	tFontDesc.fInterval = 0.f;

	//	UI_Generator->Render_Font(TEXT("E"), tFontDesc, m_ePlayerID);
	//}
}

HRESULT CHeaderBox::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CHeaderBox * CHeaderBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHeaderBox* pInstance = new CHeaderBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create HeaderBox Prototype, Error to CHeaderBox::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHeaderBox::Clone_GameObject(void * pArg)
{
	CHeaderBox* pClone = new CHeaderBox(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CHeaderBox, Error to CHeaderBox::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CHeaderBox::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
