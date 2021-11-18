#include "stdafx.h"
#include "..\Public\InputButton_Frame.h"

#include "GameInstance.h"

CInputButton_Frame::CInputButton_Frame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CInputButton_Frame::CInputButton_Frame(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CInputButton_Frame::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	SetUp_Option();

	return S_OK;
}

_int CInputButton_Frame::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CInputButton_Frame::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInputButton_Frame::Render()
{
	CUIObject::Render();

	if (0 == m_iOption)
	{
		if (FAILED(Set_UIVariables_Perspective()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Set_InterActiveVariables_Perspective()))
			return E_FAIL;
	}

	m_pVIBuffer_RectCom->Render(1);

	return S_OK;
}

HRESULT CInputButton_Frame::Set_UIVariables_Perspective()
{
	if (nullptr == m_pVIBuffer_RectCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	ViewMatrix = XMMatrixIdentity();

	D3D11_VIEWPORT Viewport;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
	}

	m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

HRESULT CInputButton_Frame::Set_InterActiveVariables_Perspective()
{
	D3D11_VIEWPORT Viewport;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;
	_float3 vConvertPos;

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

		vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f + Viewport.TopLeftX);
		vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f + Viewport.TopLeftY);

		// ¿ÞÂÊ ¿À¸¥ÂÊ
		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = 620.f;
		else if (Viewport.TopLeftX > vConvertPos.x)
			vConvertPos.x = 10.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = 10.f;
		else if (vConvertPos.y > 720.f)
			vConvertPos.y = 700.f;


		vConvertPos.x = -1.f * (Viewport.Width / 2) + vConvertPos.x;
		vConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vConvertPos.y);
		vConvertPos.z = 0.f;

		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
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

		vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f);
		vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f);

		vConvertPos.x = -1.f * (Viewport.Width / 2) + vConvertPos.x;
		vConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vConvertPos.y);
		vConvertPos.z = 0.f;

		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);
	}

	m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));


	return S_OK;
}

void CInputButton_Frame::SetUp_Option()
{
	if (!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_F")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_Dot")) ||
		!lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_PS_Triangle")) || !lstrcmp(m_UIDesc.szUITag, TEXT("InputButton_Frame_PS_R1")))
		m_iOption = 1;
	else
		m_iOption = 0;
}

HRESULT CInputButton_Frame::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

CInputButton_Frame * CInputButton_Frame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CInputButton_Frame* pInstance = new CInputButton_Frame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create InputButton_Frame Prototype, Error to CInputButton_Frame::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInputButton_Frame::Clone_GameObject(void * pArg)
{
	CInputButton_Frame* pClone = new CInputButton_Frame(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CInputButton_Frame, Error to CInputButton_Frame::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CInputButton_Frame::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
