#include "stdafx.h"
#include "..\Public\MenuScreen.h"

#include "GameInstance.h"
#include "UI_Generator.h"
#include "HeaderBox.h"

CMenuScreen::CMenuScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMenuScreen::CMenuScreen(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMenuScreen::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMenuScreen::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMenuScreen::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CMenuScreen::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CMenuScreen::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	Input_SelectButton();

	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		for (_uint i = 0; i < 6; ++i)
		{
			if (true == UI_Generator->Get_HeaderBox(i)->Get_Render() && true == UI_Generator->Get_HeaderBox(i)->Get_LocalPlayer())
			{
				m_IsChapterScreenCreate = true;
				break;
			}
		}

		if (true == m_IsChapterScreenCreate)
		{
			for (_uint i = 0; i < 6; ++i)
				UI_Generator->Get_HeaderBox(i)->Set_Dead();


		}
	}

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMenuScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(11);

	Render_Font();

	return S_OK;
}

void CMenuScreen::Render_Font()
{
	//CUI_Generator::FONTDESC		tFontDesc;

	//if (!lstrcmp(m_UIDesc.szUITag, TEXT("MenuScreen_F")))
	//{
	//	tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
	//	tFontDesc.vScale = { 40.f, 50.f };
	//	tFontDesc.fInterval = 0.f;

	//	UI_Generator->Render_Font(TEXT("F"), tFontDesc, m_ePlayerID);
	//}
	//else if (!lstrcmp(m_UIDesc.szUITag, TEXT("MenuScreen_E")))
	//{
	//	tFontDesc.vPosition = { m_UIDesc.vPos.x , m_UIDesc.vPos.y };
	//	tFontDesc.vScale = { m_UIDesc.vScale.x / 2.f, m_UIDesc.vScale.y / 1.2f };
	//	tFontDesc.fInterval = 0.f;

	//	UI_Generator->Render_Font(TEXT("E"), tFontDesc, m_ePlayerID);
	//}
}

HRESULT CMenuScreen::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

void CMenuScreen::Input_SelectButton()
{

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		--m_iHeaderIndex;
		m_IsHeaderBoxChange = true;

		if (0 > m_iHeaderIndex)
			m_iHeaderIndex = 0;
	}
	else if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		++m_iHeaderIndex;
		m_IsHeaderBoxChange = true;

		if (5 < m_iHeaderIndex)
			m_iHeaderIndex = 5;
	}

	if (true == m_IsHeaderBoxChange)
	{
		if (true == m_IsFirst)
		{
			m_pHeaderBox = UI_Generator->Get_HeaderBox(0);
			m_pHeaderBox->Set_PreviousSelect();

			m_IsFirst = false;
		}

		if (nullptr != m_pHeaderBox)
			m_pHeaderBox->Set_PreviousSelect();

		m_pHeaderBox = UI_Generator->Get_HeaderBox(m_iHeaderIndex);
		m_pHeaderBox->Set_NextSelect();

		m_IsHeaderBoxChange = false;
	}
}

CMenuScreen * CMenuScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMenuScreen* pInstance = new CMenuScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create MenuScreen Prototype, Error to CMenuScreen::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuScreen::Clone_GameObject(void * pArg)
{
	CMenuScreen* pClone = new CMenuScreen(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMenuScreen, Error to CMenuScreen::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMenuScreen::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
