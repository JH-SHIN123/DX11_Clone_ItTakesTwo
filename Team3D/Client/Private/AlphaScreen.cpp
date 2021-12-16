#include "stdafx.h"
#include "..\Public\AlphaScreen.h"

#include "GameInstance.h"
#include "UI_Generator.h"

CAlphaScreen::CAlphaScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CAlphaScreen::CAlphaScreen(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CAlphaScreen::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAlphaScreen::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	lstrcpy(m_UIDesc.szTextureTag, TEXT("AlphaScreen"));

	return S_OK;
}

HRESULT CAlphaScreen::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));


	Option_Setting();


	return S_OK;
}

_int CAlphaScreen::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CAlphaScreen::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	if (3 == m_iOption || 4 == m_iOption)
	{
		if (false == m_IsFadeOut)
		{
			m_fAlpha += (_float)TimeDelta * m_fFadeInSpeed;

			if (1.f <= m_fAlpha)
				m_fAlpha = 1.f;
		}
		else if (true == m_IsFadeOut)
		{
			m_fAlpha -= (_float)TimeDelta * m_fFadeOutSpeed;

			if (0.f >= m_fAlpha)
			{
				m_IsDead = true;

				if (m_ePlayerID == Player::Cody)
				{
					UI_Delete(Cody, WhiteScreenFadeInOut);
					UI_Delete(Cody, BlackScreenFadeInOut);
				}
				else if (m_ePlayerID == Player::May)
				{
					UI_Delete(May, WhiteScreenFadeInOut);
					UI_Delete(May, BlackScreenFadeInOut);
				}
			}
		}
	}

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CAlphaScreen::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (5 == m_iOption || 6 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_fAlpha = 1.f;
		m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

		m_pVIBuffer_RectCom->Render(m_iShaderPassNum);
	}
	else if (3 == m_iOption || 4 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

		m_pVIBuffer_RectCom->Render(m_iShaderPassNum);
	}
	else if (1 == m_iOption || 2 == m_iOption)
	{
		if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_pVIBuffer_RectCom->Set_Variable("g_iAlphaOption", &m_iOption, sizeof(_int));

		m_pVIBuffer_RectCom->Render(m_iShaderPassNum);
	}
	else
	{
		if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
			return E_FAIL;

		m_pVIBuffer_RectCom->Set_Variable("g_iAlphaOption", &m_iOption, sizeof(_int));

		m_pVIBuffer_RectCom->Render(m_iShaderPassNum);
	}

	return S_OK;
}


void CAlphaScreen::Option_Setting()
{
	if (6 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 320.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 150.f, 0.f, 0.f));
		m_fSortOrder = 0.f;
		m_iShaderPassNum = 17;
		m_fAlpha = 1.f;
	}
	else if (5 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -320.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 150.f, 0.f, 0.f));
		m_fSortOrder = 0.f;
		m_iShaderPassNum = 17;
		m_fAlpha = 1.f;
	}
	else if (4 == m_iOption) /* 검은색 스크린 FadeOut */
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 0.f));
		m_fSortOrder = 0.f;
		m_iShaderPassNum = 17;
		m_fAlpha = 0.f;
	}
	else if (3 == m_iOption) /* 하얀색 FadeInOut 스크린 */
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 0.f));
		m_fSortOrder = 0.f;
		m_iShaderPassNum = 16;
	}
	else if (2 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 50.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(400.f, 300.f, 0.f, 0.f));
		m_fSortOrder = -3.f;
		m_iShaderPassNum = 12;
	}
	else if (1 == m_iOption)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 720.f, 0.f, 0.f));
		m_fSortOrder = -2.f;
		m_iShaderPassNum = 12;
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->Set_Scale(XMVectorSet(640.f, 720.f, 0.f, 0.f));
		m_fSortOrder = 0.f;
		m_iShaderPassNum = 7;
	}

}

HRESULT CAlphaScreen::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CAlphaScreen * CAlphaScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CAlphaScreen* pInstance = new CAlphaScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create AlphaScreen Prototype, Error to CAlphaScreen::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlphaScreen::Clone_GameObject(void * pArg)
{
	CAlphaScreen* pClone = new CAlphaScreen(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CAlphaScreen, Error to CAlphaScreen::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAlphaScreen::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
