#include "stdafx.h"
#include "..\public\UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUIObject::CUIObject(const CUIObject & rhs)
	: CGameObject(rhs),
	m_UIDesc(rhs.m_UIDesc)
{
}

HRESULT CUIObject::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUIObject::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fSortOrder = (_float)m_UIDesc.iRenderGroup * -1.f;

	return S_OK;
}

_int CUIObject::Tick(_double TimeDelta)
{

	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CUIObject::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	return NO_EVENT;
}

HRESULT CUIObject::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	return S_OK;
}

_float CUIObject::Get_DistanceFromCamera()
{
	return m_fSortOrder;
}

void CUIObject::Set_Dead()
{
	m_IsDead = true;
}

void CUIObject::Set_PlayerID(Player::ID ePlayerID)
{
	m_ePlayerID = ePlayerID;
}

void CUIObject::Set_PosX(_float fPosX)
{
	m_UIDesc.vPos.x = fPosX;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
}

void CUIObject::Set_FadeInSpeed(_float fSpeed)
{
	m_fFadeInSpeed = fSpeed;
}

void CUIObject::Set_FadeOutSpeed(_float fSpeed)
{
	m_fFadeOutSpeed = fSpeed;
}

void CUIObject::Set_FadeOut()
{
	m_IsFadeOut = true;
}

void CUIObject::Set_TargetPos(_vector vPos)
{
	XMStoreFloat4(&m_vTargetPos, vPos);
}

void CUIObject::Set_ScaleEffect()
{
}

HRESULT CUIObject::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (1 <= m_UIDesc.iSubTextureNum)
		FAILED_CHECK_RETURN(CGameObject::Add_Component(m_UIDesc.iTextureLevelIndex, m_UIDesc.szSubTextureTag, TEXT("Com_SubTexture"), (CComponent**)&m_pSubTexturCom), E_FAIL);

	return S_OK;
}

HRESULT CUIObject::Set_UIDefaultVariables_Perspective(CVIBuffer * pVIBuffer)
{
	_matrix WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();
	ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	pVIBuffer->Set_Variable("g_UIWorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_UIViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_UIProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));

	pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

HRESULT CUIObject::Set_UIVariables_Perspective(CVIBuffer* pVIBuffer)
{
	if (nullptr == pVIBuffer || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;
	_int iGSOption;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	D3D11_VIEWPORT Viewport;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		iGSOption = 0;

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		iGSOption = 1;

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
	}

	pVIBuffer->Set_Variable("g_iGSOption", &iGSOption, sizeof(_int));

	pVIBuffer->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

HRESULT CUIObject::Set_InterActiveVariables_Perspective(CVIBuffer* pVIBuffer)
{
	D3D11_VIEWPORT Viewport;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;
	_float3 vConvertPos;
	_int iGsOption;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		ViewMatrix = XMMatrixIdentity();
		SubProjMatrix = XMMatrixIdentity();

		_matrix MainViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINVIEW);
		_matrix MainProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINPROJ);

		_matrix matMainCombine = MainViewMatrix * MainProjMatrix;

		_vector vTargetPos = XMLoadFloat4(&m_vTargetPos);
		vTargetPos = XMVector3TransformCoord(vTargetPos, matMainCombine);

		XMStoreFloat3(&vConvertPos, vTargetPos);
		vConvertPos.x += 1.f;
		vConvertPos.y += 1.f;

		if (1.f <= vConvertPos.z)
		{
			vConvertPos.y *= -1.f;
			vConvertPos.x *= -1.f;
		}

		vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f + Viewport.TopLeftX);
		vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f + Viewport.TopLeftY);

		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = Viewport.Width - 10.f;
		else if (Viewport.TopLeftX > vConvertPos.x)
			vConvertPos.x = Viewport.TopLeftX + 10.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = Viewport.TopLeftY + 10.f;
		else if (Viewport.Height < vConvertPos.y)
			vConvertPos.y = Viewport.Height - 20.f;

		vConvertPos.x = -1.f * (Viewport.Width / 2) + vConvertPos.x;
		vConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vConvertPos.y);
		vConvertPos.z = 0.f;

		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;
		m_UIDesc.vPos.x = vConvertPos.x;
		m_UIDesc.vPos.y = vConvertPos.y;

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		iGsOption = 0;
	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		ViewMatrix = XMMatrixIdentity();
		ProjMatrix = XMMatrixIdentity();

		_matrix SubCombineViewMatirx = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBVIEW);
		_matrix SubCombineProjMatirx = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBPROJ);

		_matrix matSubCombine = SubCombineViewMatirx * SubCombineProjMatirx;

		_vector vTargetPos = XMLoadFloat4(&m_vTargetPos);
		vTargetPos = XMVector3TransformCoord(vTargetPos, matSubCombine);

		XMStoreFloat3(&vConvertPos, vTargetPos);
		vConvertPos.x += 1.f;
		vConvertPos.y += 1.f;

		if (1.f <= vConvertPos.z)
		{
			vConvertPos.y *= -1.f;
			vConvertPos.x *= -1.f;
		}

		vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f);
		vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f);

		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = Viewport.Width - 10.f;
		else if (0.f > vConvertPos.x)
			vConvertPos.x = 10.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = Viewport.TopLeftY + 10.f;
		else if (Viewport.Height < vConvertPos.y)
			vConvertPos.y = Viewport.Height - 20.f;

		vConvertPos.x = -1.f * (Viewport.Width / 2) + vConvertPos.x;
		vConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vConvertPos.y);
		vConvertPos.z = 0.f;

		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;
		m_UIDesc.vPos.x = vConvertPos.x;
		m_UIDesc.vPos.y = vConvertPos.y;

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		iGsOption = 1;
	}

	pVIBuffer->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));

	pVIBuffer->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	pVIBuffer->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

void CUIObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	if (1 <= m_UIDesc.iSubTextureNum)
		Safe_Release(m_pSubTexturCom);

	CGameObject::Free();
}