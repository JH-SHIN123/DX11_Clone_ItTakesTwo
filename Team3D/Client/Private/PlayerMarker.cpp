#include "stdafx.h"
#include "..\Public\PlayerMarker.h"

#include "GameInstance.h"
#include "DataStorage.h"
#include "Cody.h"
#include "May.h"
#include "Pipeline.h"
#include "MainCamera.h"
#include "SubCamera.h"

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
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x - 10.f, m_UIDesc.vScale.y - 10.f, 0.f, 0.f));

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
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;
	
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CPlayerMarker::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(Set_PlayerMarkerVariables_Perspective()))
		return E_FAIL;

	if(true == m_IsRender)
		m_pVIBuffer_RectCom->Render(4);

	return S_OK;
}

void CPlayerMarker::Set_TargetPos(_vector vPos)
{
	XMStoreFloat4(&m_vMarkerTargetPos, vPos);
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
	_float3 vConvertPos, vCodyConvertPos, vCodyConvertRight, vMayConvertPos, vMayConvertRight;
	_int iGsOption;

	if (m_ePlayerID == Player::Cody)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(1);

		ViewMatrix = XMMatrixIdentity();
		SubProjMatrix = XMMatrixIdentity();

		// m_vMarkerTargetPos가 May포스임 
		CCody* pCody = (CCody*)DATABASE->GetCody();
		_vector vCodyPos = pCody->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vCodyUp = pCody->Get_Transform()->Get_State(CTransform::STATE_UP);

		m_vMarkerTargetPos.y += 1.f;

		_vector vTargetPos = XMLoadFloat4(&m_vMarkerTargetPos);

		vConvertPos = ConvertToWindowPos(Viewport, vTargetPos);
		vCodyConvertPos = ConvertToWindowPos(Viewport, vCodyPos);

		// 뷰포트안에 있으면 회전 180도로 고정시키고 알파값 줄여주고 랜더 안타게 해주는 함수
		if (false == IsCheckCharacterInViewport(Viewport, vConvertPos))
			return S_OK;

		// 고정 시키기전에 내적해서 각도구해야해서 저장해둠
		_float3 vSavePos = vConvertPos;

		// 뷰포트 밖으로 나가면 화면 고정
		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = Viewport.Width - 20.f;
		else if (Viewport.TopLeftX > vConvertPos.x)
			vConvertPos.x = Viewport.TopLeftX + 20.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = Viewport.TopLeftY + 20.f;
		else if (Viewport.Height < vConvertPos.y)
			vConvertPos.y = Viewport.Height - 20.f;

		// 뷰스페이스로 변환
		// 뷰 행렬에 좌표를 바로 줘서 직교하던 월드 스페이스에 주던 상관 없음 월드에 줬다고 하면 뷰 행렬은 항등으로 주고 직교 투영하기 때문임 ㅇㅇ
		// 고정시킨 좌표는 직교투영 시킬 매트릭스에 던져주자
		vConvertPos = ConvertToViewPos(Viewport, vConvertPos);
		// 얘는 내적하기 위해 고정시키키 전에 저장해놨던 좌표임
		vSavePos = ConvertToViewPos(Viewport, vSavePos);
		vCodyConvertPos = ConvertToViewPos(Viewport, vCodyConvertPos);

		_vector vDir = XMLoadFloat3(&vSavePos) - XMLoadFloat3(&vCodyConvertPos);
		_vector vDot = XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vCodyUp));
		_float fRadian = acosf(XMVectorGetX(vDot));
		// 텍스쳐 자체가 90도 회전한 텍스쳐라서 뺴줌
		_float fAngle = 90.f - XMConvertToDegrees(fRadian);

		CMainCamera* pMainCamera = (CMainCamera*)DATABASE->Get_MainCam();
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

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));

		if (0.f < Viewport.Width)
			ProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_iColorOption = 0;
		iGsOption = 0;
	}
	else if (m_ePlayerID == Player::May)
	{
		Viewport = m_pGameInstance->Get_ViewportInfo(2);

		ViewMatrix = XMMatrixIdentity();
		ProjMatrix = XMMatrixIdentity();

		// m_vTargetPos가 Cody포스임 
		CMay* pMay = (CMay*)DATABASE->GetMay();
		_vector vMayPos = pMay->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vMayUp = pMay->Get_Transform()->Get_State(CTransform::STATE_UP);

		m_vMarkerTargetPos.y += 1.f;

		_vector vTargetPos = XMLoadFloat4(&m_vMarkerTargetPos);

		vConvertPos = ConvertToWindowPos(Viewport, vTargetPos);
		vMayConvertPos = ConvertToWindowPos(Viewport, vMayPos);

		if (false == IsCheckCharacterInViewport(Viewport, vConvertPos))
			return S_OK;

		_float3 vSavePos = vConvertPos;

		if (Viewport.Width < vConvertPos.x)
			vConvertPos.x = Viewport.Width - 20.f;
		else if (0.f > vConvertPos.x)
			vConvertPos.x = 20.f;

		if (0.f > vConvertPos.y)
			vConvertPos.y = Viewport.TopLeftY + 20.f;
		else if (Viewport.Height < vConvertPos.y)
			vConvertPos.y = Viewport.Height - 20.f;

		vConvertPos = ConvertToViewPos(Viewport, vConvertPos);
		vSavePos = ConvertToViewPos(Viewport, vSavePos);
		vMayConvertPos = ConvertToViewPos(Viewport, vMayConvertPos);

		_vector vDir = XMLoadFloat3(&vSavePos) - XMLoadFloat3(&vMayConvertPos);
		_vector vDot = XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vMayUp));
		_float fRadian = acosf(XMVectorGetX(vDot));
		// 텍스쳐 자체가 90도 회전한 텍스쳐라서 뺴줌
		_float fAngle = 90.f - XMConvertToDegrees(fRadian);

		CSubCamera* pSubCamera = (CSubCamera*)DATABASE->Get_SubCam();
		_vector vCamLook = pSubCamera->Get_Transform()->Get_State(CTransform::STATE_LOOK);

		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		WorldMatrix.r[3].m128_f32[0] = vConvertPos.x;
		WorldMatrix.r[3].m128_f32[1] = vConvertPos.y;

		_float fCCW = XMVectorGetZ(XMVector3Cross(XMVector3Normalize(vCamLook), XMVector3Normalize(vDir)));

		if (0.f > fCCW)
		{
			fAngle *= -1.f;
			fAngle -= 180.f;
		}

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));

		if (0.f < Viewport.Width)
			SubProjMatrix = XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f);

		m_iColorOption = 1;
		iGsOption = 1;
	}


	if (0.f > vConvertPos.y)
		vConvertPos.y = Viewport.TopLeftY + 20.f;
	else if (Viewport.Height < vConvertPos.y)
		vConvertPos.y = Viewport.Height - 20.f;

	// 이거 안하면 반대쪽 뷰포트 반짝거리는 오류 발생합니다.
	m_pVIBuffer_RectCom->Set_Variable("g_iGSOption", &iGsOption, sizeof(_int));
	m_pVIBuffer_RectCom->Set_Variable("g_iColorOption", &m_iColorOption, sizeof(_int));
	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pVIBuffer_RectCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_matrix));
	m_pVIBuffer_RectCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(SubProjMatrix), sizeof(_matrix));

	m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(m_UIDesc.iTextureRenderIndex));

	return S_OK;
}

_bool CPlayerMarker::IsCheckCharacterInViewport(D3D11_VIEWPORT Viewport, _float3 vConvertPos)
{
	if (Viewport.Width > vConvertPos.x && 0.f < vConvertPos.x &&
		Viewport.Height > vConvertPos.y && 0.f < vConvertPos.y)
	{
		//m_fAlpha -= m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));
		m_fAlpha -= 0.0016f * 5.f;

		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));
		m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

		if (m_fAlpha <= 0.f)
		{
			m_IsRender = false;
			return m_IsRender;
		}
	}
	else
	{
		m_fAlpha = 1.f;
		m_IsRender = true;
	}

	return m_IsRender;
}

_float3 CPlayerMarker::ConvertToWindowPos(D3D11_VIEWPORT Viewprot, _vector vPos)
{
	_matrix CombineViewMatrix, CombineProjMatrix;

	if (m_ePlayerID == Player::Cody)
	{
		CombineViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINVIEW);
		CombineProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINPROJ);
	}
	else
	{
		CombineViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBVIEW);
		CombineProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_SUBPROJ);
	}

	_matrix matCombineMatrix = CombineViewMatrix * CombineProjMatrix;

	vPos = XMVector3TransformCoord(vPos, matCombineMatrix);

	_float3 vConvertPos;

	// 보정
	XMStoreFloat3(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.y += 1.f;

	if (1.f <= vConvertPos.z)
	{
		vConvertPos.x *= -1.f;
		vConvertPos.y *= -1.f;
	}

	vConvertPos.x = ((Viewprot.Width * (vConvertPos.x)) / 2.f);
	vConvertPos.y = (Viewprot.Height * (2.f - vConvertPos.y) / 2.f);

	return vConvertPos;
}

_float3 CPlayerMarker::ConvertToViewPos(D3D11_VIEWPORT Viewport, _float3 vPos)
{
	vPos.x = -1.f * (Viewport.Width / 2) + vPos.x;
	vPos.y = Viewport.Height - (1.f * (Viewport.Height / 2) + vPos.y);
	vPos.z = 0.f;

	return vPos;
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
