#include "stdafx.h"
#include "..\Public\UISprite.h"

#include "GameInstance.h"

CUISprite::CUISprite(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CUISprite::CUISprite(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CUISprite::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUISprite::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CUISprite::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
	
	SetUp_SpriteInfo();

	return S_OK;
}

_int CUISprite::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CUISprite::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	Update_Frame(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUISprite::Render()
{
	CUIObject::Render();

	if (FAILED(Set_UIVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_SpriteCom->Render(m_iShaderPassNum);

	return S_OK;
}

HRESULT CUISprite::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_SpriteCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	_float fSpriteWidth = (_float)m_iTextureWidth / m_iWidthMaxCount;
	_float fSpriteHeight = (_float)m_iTextureHeight / m_iHeightMaxCount;

	_float2 vLeftTop = { (_float)(m_iWidthIndex * fSpriteWidth / m_iTextureWidth), (_float)(m_iHeightIndex * fSpriteHeight / m_iTextureHeight) };
	_float2 vRightTop = { (_float)((m_iWidthIndex + 1) * fSpriteWidth / m_iTextureWidth), (_float)(m_iHeightIndex * fSpriteHeight / m_iTextureHeight) };
	_float2 vRightBottom = { (_float)((m_iWidthIndex + 1) * fSpriteWidth / m_iTextureWidth), (_float)((m_iHeightIndex + 1) * fSpriteHeight / m_iTextureHeight) };
	_float2 vLeftBottom = { (_float)(m_iWidthIndex * fSpriteWidth / m_iTextureWidth), (_float)((m_iHeightIndex + 1) * fSpriteHeight / m_iTextureHeight) };

	m_pVIBuffer_SpriteCom->Set_Variable("vLeftTopUV", &vLeftTop, sizeof(_float2));
	m_pVIBuffer_SpriteCom->Set_Variable("vRightTopUV", &vRightTop, sizeof(_float2));
	m_pVIBuffer_SpriteCom->Set_Variable("vLeftBottomUV", &vLeftBottom, sizeof(_float2));
	m_pVIBuffer_SpriteCom->Set_Variable("vRightBottomUV", &vRightBottom, sizeof(_float2));

	if (m_ePlayerID == Player::Player_Cody)
	{
		_int iGsOption = 0;
		m_pVIBuffer_SpriteCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

		D3D11_VIEWPORT Viewport = m_pGameInstance->Get_ViewportInfo(1);

		SubProjMatrix = XMMatrixIdentity();

		// 뷰포트 하나로 합쳐질 때 Width가 0.f가 되버리면 직교할 때 0.f / 0.f 나누기 연산이 일어나서 XMSclarNearEqul 오류 발생 그거 임시방편 예외처리 입니다.
		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_pVIBuffer_SpriteCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));
	}         
	else if (m_ePlayerID == Player::Player_May)
	{
		_int iGsOption = 1;
		m_pVIBuffer_SpriteCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

		D3D11_VIEWPORT Viewport = m_pGameInstance->Get_ViewportInfo(2);

		ProjMatrix = XMMatrixIdentity();

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_pVIBuffer_SpriteCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));
	}
	else if (m_ePlayerID == Player::Default)
	{
		ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

		m_pVIBuffer_SpriteCom->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
		m_pVIBuffer_SpriteCom->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	}

	m_pVIBuffer_SpriteCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

void CUISprite::Update_Frame(_double TimeDelta)
{
	m_FrameTime += TimeDelta;

	if (m_FrameControl <= m_FrameTime)
	{
		++m_iWidthIndex;
		m_FrameTime = 0;
	}

	if (m_iWidthIndex >= m_iWidthMaxCount)
	{
		m_iWidthIndex = 0;

		if(1 != m_iStickOption)
			++m_iHeightIndex;
	}

	if (m_iHeightIndex >= m_iHeightMaxCount)
	{
		m_iWidthIndex = 0;
		m_iHeightIndex = 0;
	}
}

void CUISprite::SetUp_SpriteInfo()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("StickIcon")))
	{
		m_iTextureWidth = 2048;
		m_iTextureHeight = 256;
		m_iWidthMaxCount = 16;
		m_iHeightMaxCount = 2;
		m_iShaderPassNum = 0;
		m_iStickOption = 1;
		m_FrameControl = 0.03;
	}
	else if (!lstrcmp(m_UIDesc.szUITag, TEXT("LoadingBook")))
	{
		m_iTextureWidth = 1024;
		m_iTextureHeight = 1024;
		m_iWidthMaxCount = 8;
		m_iHeightMaxCount = 8;
		m_iShaderPassNum = 1;
		m_FrameControl = 0.01;
	}
}

HRESULT CUISprite::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Sprite"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_SpriteCom), E_FAIL);

	return S_OK;
}

CUISprite * CUISprite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CUISprite* pInstance = new CUISprite(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create UISprite Prototype, Error to CUISprite::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUISprite::Clone_GameObject(void * pArg)
{
	CUISprite* pClone = new CUISprite(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CUISprite, Error to CUISprite::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUISprite::Free()
{
	Safe_Release(m_pVIBuffer_SpriteCom);

	CUIObject::Free();
}
