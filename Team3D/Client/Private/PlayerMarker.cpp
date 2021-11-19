#include "stdafx.h"
#include "..\Public\PlayerMarker.h"

#include "GameInstance.h"
#include "DataStorage.h"
#include "Cody.h"
#include "May.h"
#include "Pipeline.h"
#include "MainCamera.h"

CPlayerMarker::CPlayerMarker(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CPlayerMarker::CPlayerMarker(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CPlayerMarker::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPlayerMarker::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CPlayerMarker::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	return S_OK;
}

_int CPlayerMarker::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	return _int();
}

_int CPlayerMarker::Late_Tick(_double TimeDelta)
{
	CUIObject::Late_Tick(TimeDelta);
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayerMarker::Render()
{
	CUIObject::Render();

	if (FAILED(Set_PlayerMarkerVariables_Perspective()))
		return E_FAIL;

	m_pVIBuffer_RectCom->Render(0);

	return S_OK;
}

HRESULT CPlayerMarker::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

HRESULT CPlayerMarker::Set_PlayerMarkerVariables_Perspective()
{
	D3D11_VIEWPORT Viewport;

	_matrix WorldMatrix, ViewMatrix, ProjMatrix, SubViewMatrix, SubProjMatrix;
	_float3 vConvertPos, vCodyConvertPos, vCodyConvertRight;
	_float fRadian;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		ViewMatrix = XMMatrixIdentity();
		SubProjMatrix = XMMatrixIdentity();

		CCody* pCody = (CCody*)DATABASE->GetCody();
		_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vCodyLook = pCody->Get_Transform()->Get_State(CTransform::STATE_LOOK);
		_vector vCodyUp = pCody->Get_Transform()->Get_State(CTransform::STATE_UP);

		m_vTargetPos.y += 2.f;

		_matrix MainViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINVIEW);
		_matrix MainProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINPROJ);

		_matrix matMainCombine = MainViewMatrix * MainProjMatrix;

		_vector vTargetPos = XMLoadFloat4(&m_vTargetPos);
		vTargetPos = XMVector3TransformCoord(vTargetPos, matMainCombine);
		
		vCodyPos = XMVector3TransformCoord(vCodyPos, matMainCombine);

		XMStoreFloat3(&vConvertPos, vTargetPos);
		vConvertPos.x += 1.f;
		vConvertPos.y += 1.f;

		XMStoreFloat3(&vCodyConvertPos, vCodyPos);
		vCodyConvertPos.x += 1.f;
		vCodyConvertPos.y += 1.f;

		if (1.f <= vConvertPos.z)
		{
			vConvertPos.x *= -1.f;
			vConvertPos.y *= -1.f;
		}

		if (1.f <= vCodyConvertPos.z)
		{
			vCodyConvertPos.x *= -1.f;
			vCodyConvertPos.y *= -1.f;
		}

		vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f + Viewport.TopLeftX);
		vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f + Viewport.TopLeftY);

		vCodyConvertPos.x = ((Viewport.Width * (vCodyConvertPos.x)) / 2.f + Viewport.TopLeftX);
		vCodyConvertPos.y = (Viewport.Height * (2.f - vCodyConvertPos.y) / 2.f + Viewport.TopLeftY);

		_float3 vSavePos = vConvertPos;

		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = Viewport.Width - 15.f;
		else if (Viewport.TopLeftX > vConvertPos.x)
			vConvertPos.x = Viewport.TopLeftX + 15.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = Viewport.TopLeftY + 15.f;
		else if (Viewport.Height < vConvertPos.y)
			vConvertPos.y = Viewport.Height - 20.f;

		vConvertPos.x = -1.f * (Viewport.Width / 2) + vConvertPos.x;
		vConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vConvertPos.y);
		vConvertPos.z = 0.f;

		vSavePos.x = -1.f * (Viewport.Width / 2) + vSavePos.x;
		vSavePos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vSavePos.y);
		vSavePos.z = 0.f;

		vCodyConvertPos.x = -1.f * (Viewport.Width / 2) + vCodyConvertPos.x;
		vCodyConvertPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vCodyConvertPos.y);
		vCodyConvertPos.z = 0.f;

		//_vector vDir = XMLoadFloat3(&vCodyConvertPos) - XMLoadFloat3(&vSavePos);
		_vector vDir = XMLoadFloat3(&vSavePos) - XMLoadFloat3(&vCodyConvertPos);
		_vector vDot = XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vCodyUp));
		_float fRadian = acosf(XMVectorGetX(vDot));
		// 텍스쳐 자체가 90도 회전한 텍스쳐라서 뺴줌
		_float fAngle = 90.f - XMConvertToDegrees(fRadian);

		CMainCamera* pMainCamera = (CMainCamera*)DATABASE->Get_MainCam();
		_vector vCamUp = pMainCamera->Get_Transform()->Get_State(CTransform::STATE_UP);
		_vector vCamRight = pMainCamera->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
		_vector vCamLook = pMainCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);


		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;

		_float fCCW = XMVectorGetZ(XMVector3Cross(XMVector3Normalize(vCamLook), XMVector3Normalize(vDir)));

		if (0.f > fCCW)
		{
			fAngle *= -1.f;
			fAngle -= 180.f;
		}

		//if (0.f < vCamLook.m128_f32[2])
		//{
		//	fAngle *= 1.f;
		//}

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));

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

CPlayerMarker * CPlayerMarker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPlayerMarker* pInstance = new CPlayerMarker(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create PlayerMarker Prototype, Error to CPlayerMarker::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerMarker::Clone_GameObject(void * pArg)
{
	CPlayerMarker* pClone = new CPlayerMarker(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPlayerMarker, Error to CPlayerMarker::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayerMarker::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);

	CUIObject::Free();
}
