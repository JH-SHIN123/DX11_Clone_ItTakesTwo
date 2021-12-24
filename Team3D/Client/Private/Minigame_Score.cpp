#include "stdafx.h"
#include "..\Public\Minigame_Score.h"

CMinigame_Score::CMinigame_Score(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CMinigame_Score::CMinigame_Score(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CMinigame_Score::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinigame_Score::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CMinigame_Score::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	FAILED_CHECK_RETURN(Ready_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	if (0 == m_iOption)
		m_iShaderPassNum = 27;
	else
		m_iShaderPassNum = 28;

	return S_OK;
}

_int CMinigame_Score::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CMinigame_Score::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CMinigame_Score::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(m_iShaderPassNum);

	Render_Font();

	return S_OK;
}

HRESULT CMinigame_Score::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_FontCodyWinCount"), (CComponent**)&m_pFontCodyWinCount), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_FontMayWinCount"), (CComponent**)&m_pFontMayWinCount), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_FontCenter"), (CComponent**)&m_pFontCenter), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_FontCody"), (CComponent**)&m_pFontCody), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_FontMay"), (CComponent**)&m_pFontMay), E_FAIL);

	return S_OK;
}

void CMinigame_Score::Render_Font()
{
	if (0 == m_iOption)
	{
		_uint iCount;

		m_pFontCenter->Render_Font(TEXT("승리한 라운드"), _float2(m_UIDesc.vPos.x + 640.f, m_UIDesc.vPos.y - 175.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.28f);
		m_pFontMay->Render_Font(TEXT("메이"), _float2(m_UIDesc.vPos.x + 780.f, m_UIDesc.vPos.y - 175.f), XMVectorSet(0.258f, 0.721f, 0.941f, 1.f), 0.28f);
		m_pFontCody->Render_Font(TEXT("코디"), _float2(m_UIDesc.vPos.x + 500.f, m_UIDesc.vPos.y - 175.f), XMVectorSet(0.58f, 0.894f, 0.239f, 1.f), 0.28f);

		_tchar pText[_MAX_ITOSTR_BASE10_COUNT];
		iCount = DATABASE->Get_CodyMinigameWinCount();
		_itow_s(iCount, pText, 10);
		m_pFontCodyWinCount->Render_Font(pText, _float2(m_UIDesc.vPos.x + 430.f, m_UIDesc.vPos.y - 175.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.35f);

		iCount = DATABASE->Get_MayMinigameWinCount();
		_itow_s(iCount, pText, 10);
		m_pFontMayWinCount->Render_Font(pText, _float2(m_UIDesc.vPos.x + 850.f, m_UIDesc.vPos.y - 175.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.35f);
	}
	else
	{
		m_pFontCenter->Render_Font(TEXT("레이저 테니스"), _float2(m_UIDesc.vPos.x + 640.f, m_UIDesc.vPos.y - 290.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.35f);
	}
}

CMinigame_Score * CMinigame_Score::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CMinigame_Score* pInstance = new CMinigame_Score(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Minigame_Score Prototype, Error to CMinigame_Score::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMinigame_Score::Clone_GameObject(void * pArg)
{
	CMinigame_Score* pClone = new CMinigame_Score(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CMinigame_Score, Error to CMinigame_Score::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMinigame_Score::Free()
{
	Safe_Release(m_pFontMayWinCount);
	Safe_Release(m_pFontCodyWinCount);
	Safe_Release(m_pFontMay);
	Safe_Release(m_pFontCody);
	Safe_Release(m_pFontCenter);
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
