#include "stdafx.h"
#include "..\public\Character.h"
#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{
}

_float CCharacter::Compute_Distance(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vTargetPos - vPlayerPos;
	_float vDist = fabs(XMVectorGetX(XMVector3Length(vDir)));

	return vDist;
}

_float CCharacter::Compute_Degree(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = pDstTransform->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vPlayerPos - vTargetPos;
	vDir = XMVector3Normalize(vDir);
	_float Cos = acosf(XMVectorGetX(XMVector3Dot(vTargetLook, vDir)));
	_float fDegree = XMConvertToDegrees(Cos);

	//Cross 
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vOriginUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vUp = XMVector3Cross(vLook, vDir);
	_float CCW = XMVectorGetX(XMVector3Dot(vOriginUp, vUp));

	if (CCW < 0.f)
		m_fClockWise = -1.f;
	else
		m_fClockWise = 1.f;

	return fDegree;
}

_float CCharacter::Compute_Degree_On_Land(CTransform * pPlayerTransform, CTransform * pDstTransform)
{
	_vector vTargetPos = pDstTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTargetLook = pDstTransform->Get_State(CTransform::STATE_LOOK);
	vTargetLook = XMVectorSetY(vTargetLook, 0.f);

	_vector vDir = vPlayerPos - vTargetPos;
	vDir = XMVector3Normalize(vDir);
	vDir = XMVectorSetY(vDir, 0.f);

	_float Cos = acosf(XMVectorGetX(XMVector3Dot(vTargetLook, vDir)));
	_float fDegree = XMConvertToDegrees(Cos);

	//Cross 
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vOriginUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vUp = XMVector3Cross(vLook, vDir);
	_float CCW = XMVectorGetX(XMVector3Dot(vOriginUp, vUp));

	if (CCW < 0.f)
		m_fClockWise = -1.f;
	else
		m_fClockWise = 1.f;

	return fDegree;
}

HRESULT CCharacter::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCharacter::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	return S_OK;
}

_int CCharacter::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CCharacter::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CCharacter::Render()
{
	return CGameObject::Render();
}

HRESULT CCharacter::Set_ShaderConstant_Default()
{
	return CGameObject::Set_ShaderConstant_Default();
}

HRESULT CCharacter::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	return CGameObject::Set_ShaderConstant_Shadow(LightViewMatrix, LightProjMatrix);
}

void CCharacter::Character_Marker(CTransform* pTransformCom)
{
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vTargetPos = pTransformCom->Get_State(CTransform::STATE_POSITION);

	//D3D11_VIEWPORT Viewport;
	//_matrix ViewMatrix, ProjMatrix;
	//_float3 vConvertPos;

	//Viewport = m_pGameInstance->Get_ViewportInfo(1);

	//_matrix ViewMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINVIEW);
	//_matrix ProjMatrix = CPipeline::GetInstance()->Get_Transform(CPipeline::TS_MAINPROJ);

	//_matrix matMainCombine = ViewMatrix * ProjMatrix;

	//vTargetPos = XMVector3TransformCoord(vTargetPos, matMainCombine);

	//XMStoreFloat3(&vConvertPos, vTargetPos);
	//vConvertPos.x += 1.f;
	//vConvertPos.y += 1.f;

	//if (1.f <= vConvertPos.z)
	//{
	//	vConvertPos.y *= -1.f;
	//	vConvertPos.x *= -1.f;
	//}

	//vConvertPos.x = ((Viewport.Width * (vConvertPos.x)) / 2.f + Viewport.TopLeftX);
	//vConvertPos.y = (Viewport.Height * (2.f - vConvertPos.y) / 2.f + Viewport.TopLeftY);

	//if (Viewport.Width < vConvertPos.x)
	//	vConvertPos.x = Viewport.Width - 10.f;
	//else if (Viewport.TopLeftX > vConvertPos.x)
	//	vConvertPos.x = Viewport.TopLeftX + 10.f;

	//if (0.f > vConvertPos.y)
	//	vConvertPos.y = Viewport.TopLeftY + 10.f;
	//else if (Viewport.Height < vConvertPos.y)
	//	vConvertPos.y = Viewport.Height - 20.f;


}

void CCharacter::Free()
{

	CGameObject::Free();
}