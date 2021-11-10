#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "GameInstance.h"
#include"DataBase.h"
#include"Player.h"
CMainCamera::CMainCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CMainCamera::CMainCamera(const CMainCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMainCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	
	XMStoreFloat4x4(&m_matPreRev, XMMatrixIdentity());

	
	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	
	if (m_pTargetObj == nullptr)
	{
		m_pTargetObj = CDataBase::GetInstance()->GetPlayer();
		if (m_pTargetObj)
			Safe_AddRef(m_pTargetObj);
	}
	/*POINT pt{ g_iWinCX >> 1 , g_iWinCY >> 1 };
	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
*/
	CTransform* pPlayerTransform = dynamic_cast<CPlayer*>(m_pTargetObj)->Get_Transform();
	
	/*_matrix matTarget;
	Get_TargetMatrix(&matTarget, dTimeDelta);
	*/
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	matWorld *= XMMatrixInverse(nullptr , XMLoadFloat4x4(&m_matPreRev));
	m_pTransformCom->Set_WorldMatrix(matWorld);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);



	_long MouseMove = 0;
	
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	{
		m_fMouseRev[Rev_Holizontal] += MouseMove;
	/*	if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
			m_fMouseRev[Rev_Holizontal] = 0.f;*/
	}
	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	{
		m_fMouseRev[Rev_Prependicul] += MouseMove;
		//if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
		//	m_fMouseRev[Rev_Prependicul] = 0.f;
		//if (m_fMouseRev[Rev_Prependicul] < -60.f)
		//	m_fMouseRev[Rev_Prependicul] = -60.f;
	}
	
	if (m_pGameInstance->Key_Pressing(DIK_Z))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_X))
		m_pTransformCom->Go_Backward(dTimeDelta);
	
	_matrix matRevX = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta* XMConvertToRadians(m_fMouseRev[Rev_Prependicul])* m_fMouseRevSpeed[Rev_Prependicul]);
	_matrix matRevY= XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f),dTimeDelta* XMConvertToRadians(m_fMouseRev[Rev_Holizontal]) * m_fMouseRevSpeed[Rev_Holizontal]);
	_matrix matTrans = XMMatrixTranslation(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));
	_matrix matRev = matRevX * matRevY* matTrans;
	XMStoreFloat4x4(&m_matPreRev, matRev);
	
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matRev);

	return CCamera::Tick(dTimeDelta);
}

_int CMainCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);



	return NO_EVENT;
}

CMainCamera * CMainCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCamera::Clone_GameObject(void * pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainCamera::Free()
{
	CCamera::Free();
	Safe_Release(m_pTargetObj);
}

void CMainCamera::Get_TargetMatrix(_matrix * pOut, _double dTimeDelta)
{
	//CTransform* pPlayerTransform = dynamic_cast<CPlayer*>(m_pTargetObj)->Get_Transform();
	////_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	//POINT pt{ g_iWinCX >> 1 , g_iWinCY >> 1 };
	//ClientToScreen(g_hWnd, &pt);
	//SetCursorPos(pt.x, pt.y);

	//_float2 vMouseMove = { (_float)m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X),(_float)m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y) };
	//m_fMouseRev[Rev_Holizontal] += vMouseMove.x / 15.f;
	//m_fMouseRev[Rev_Prependicul] += vMouseMove.y / 15.f;

	//if (m_fMouseRev[Rev_Holizontal] > 360.f || m_fMouseRev[Rev_Holizontal] < -360.f)
	//	m_fMouseRev[Rev_Holizontal] = 0.f;
	//if (m_fMouseRev[Rev_Prependicul] > 360.f || m_fMouseRev[Rev_Prependicul] < -360.f)
	//	m_fMouseRev[Rev_Prependicul] = 0.f;
	//if (m_fMouseRev[Rev_Prependicul] < -60.f)
	//	m_fMouseRev[Rev_Prependicul] = -60.f;

	//_float3 vPlayerPos, vPlayerLook;
	//XMStoreFloat3(&vPlayerPos, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	//XMStoreFloat3(&vPlayerLook, pPlayerTransform->Get_State(CTransform::STATE_LOOK));


	//_vector vNextAt = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//_vector vDir = vNextAt - XMLoadFloat3(&m_vCurAt);

	//XMStoreFloat3(&m_vCurAt, XMLoadFloat3(&m_vCurAt) + vDir * dTimeDelta);
	//_float3 vRealAt;
	//XMStoreFloat3(&vRealAt, XMLoadFloat3(&m_vCurAt));

	//_matrix matRot = XMMatrixIdentity(), matTrans = XMMatrixTranslation(0.2f, 3.5f, -3.5f), matRotX = XMMatrixIdentity(),
	//	matRotY = XMMatrixIdentity(), matPar = XMMatrixIdentity();

	//matRot = XMMatrixRotationX(XMConvertToRadians(45.f));
	//matRotX = XMMatrixRotationX(XMConvertToRadians(m_fMouseRev[Rev_Prependicul]));
	//matRotY = XMMatrixRotationY(XMConvertToRadians(m_fMouseRev[Rev_Holizontal]));

	//matPar = XMMatrixTranslation(vRealAt.x, vRealAt.y
	//	, vRealAt.z);

	// *pOut = matRot* matTrans * matRotX * matRotY* matPar;

}
