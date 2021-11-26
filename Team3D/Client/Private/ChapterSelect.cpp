#include "stdafx.h"
#include "..\Public\ChapterSelect.h"

#include "Pipeline.h"

CChapterSelect::CChapterSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CChapterSelect::CChapterSelect(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CChapterSelect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CChapterSelect::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CChapterSelect::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	//m_pSubTextureCom = (CTextures*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_LOGO, TEXT("ChapterSelectAlpha"));

	return S_OK;
}

_int CChapterSelect::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CChapterSelect::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CChapterSelect::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIDefaultVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

    m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseAlphaTexture", m_pTextureCom->Get_ShaderResourceView(0));

	m_pVIBuffer_RectCom->Render(14);

	return S_OK;
}

HRESULT CChapterSelect::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CChapterSelect * CChapterSelect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CChapterSelect* pInstance = new CChapterSelect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create ChapterSelect Prototype, Error to CChapterSelect::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChapterSelect::Clone_GameObject(void * pArg)
{
	CChapterSelect* pClone = new CChapterSelect(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CChapterSelect, Error to CChapterSelect::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CChapterSelect::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);
	Safe_Release(m_pSubTextureCom);

	CUIObject::Free();
}
