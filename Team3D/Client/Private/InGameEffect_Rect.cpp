#include "stdafx.h"
#include "..\Public\InGameEffect_Rect.h"
#include "GameInstance.h"

CInGameEffect_Rect::CInGameEffect_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CInGameEffect_Rect::CInGameEffect_Rect(const CInGameEffect_Rect & rhs)
	: CGameObject(rhs)
	, m_EffectDesc_Prototype(rhs.m_EffectDesc_Prototype)
{
}

HRESULT CInGameEffect_Rect::NativeConstruct_Prototype(void * pArg)
{
	memcpy(&m_EffectDesc_Prototype, pArg, sizeof(EFFECT_DESC_PROTO));

	return S_OK;
}

HRESULT CInGameEffect_Rect::NativeConstruct(void * pArg)
{

	
	Ready_Component(pArg);

	Ready_InstanceBuffer();



	return S_OK;
}

_int CInGameEffect_Rect::Tick(_double TimeDelta)
{
	return _int();
}

_int CInGameEffect_Rect::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CInGameEffect_Rect::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

HRESULT CInGameEffect_Rect::SetUp_InstanceCount(_uint iInstanceCount)
{
	return S_OK;
}

HRESULT CInGameEffect_Rect::SetUp_Instance_WorldMatrix(_uint iIndex, _float4x4 WolrdMatrix)
{
	return S_OK;
}

HRESULT CInGameEffect_Rect::SetUp_Instance_WorldMatrix(_uint iIndex, _fmatrix WolrdMatrix)
{
	return S_OK;
}

HRESULT CInGameEffect_Rect::SetUp_Instance_Position(_uint iIndex, _fvector vPosition, _fvector vOffSetPosition)
{
	return S_OK;
}

void CInGameEffect_Rect::Check_Color(_double TimeDelta)
{
}

void CInGameEffect_Rect::Check_Gravity(_double TimeDelta)
{
}

_float2 CInGameEffect_Rect::Check_Size(_double TimeDelta, _int iIndex)
{
	return _float2();
}

_float4 CInGameEffect_Rect::Check_Move(_double TimeDelta, _int iIndex)
{
	return _float4();
}

HRESULT CInGameEffect_Rect::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);

	if (1 < lstrlen(m_EffectDesc_Prototype.TextureName_Second))
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_RectInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pRectInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);

	for (_int i = 0; i < 3; ++i)
		WolrdMatrix.r[i] = XMVector3Normalize(WolrdMatrix.r[i]);

	WolrdMatrix.r[0] *= m_EffectDesc_Prototype.vSize.x;
	WolrdMatrix.r[1] *= m_EffectDesc_Prototype.vSize.y;
	WolrdMatrix.r[2] *= m_EffectDesc_Prototype.vSize.z;

	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	return S_OK;
}

HRESULT CInGameEffect_Rect::Ready_InstanceBuffer(_bool IsRenderTerm)
{
	m_pInstanceBuffer = new VTXMATRIX[m_EffectDesc_Prototype.iInstanceCount];
	m_pInstance_Dir = new _float3[m_EffectDesc_Prototype.iInstanceCount];

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y,  m_EffectDesc_Prototype.vSize.x};

	_vector vRight, vUp, vLook;
	vRight	= m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vUp		= m_pTransformCom->Get_State(CTransform::STATE_UP);
	vLook	= m_pTransformCom->Get_State(CTransform::STATE_LOOK);


	for (_int i = 0; i < m_EffectDesc_Prototype.iInstanceCount; ++i)
	{
		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		m_pInstanceBuffer[i].vPosition = { 5.f,0.f,5.f,1.f };
		XMStoreFloat4(&m_pInstanceBuffer[i].vRight , vRight);
		XMStoreFloat4(&m_pInstanceBuffer[i].vUp	, vUp);
		XMStoreFloat4(&m_pInstanceBuffer[i].vLook	, vLook);
	}

	return S_OK;
}

void CInGameEffect_Rect::SetUp_Shader_Data()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return;

	m_pRectInstanceCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pRectInstanceCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	m_pRectInstanceCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	m_pRectInstanceCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	m_pRectInstanceCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();

	m_pRectInstanceCom->Set_Variable("g_fMainCamFar",		&fMainCamFar,		sizeof(_float));
	m_pRectInstanceCom->Set_Variable("g_fSubCamFar",		&fSubCamFar,		sizeof(_float));
	m_pRectInstanceCom->Set_Variable("g_vMainCamPosition", &vMainCamPosition,	sizeof(_vector));
	m_pRectInstanceCom->Set_Variable("g_vSubCamPosition",	&vSubCamPosition,	sizeof(_vector));
}

CGameObject * CInGameEffect_Rect::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CInGameEffect_Rect::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pTexturesCom_Second);
	Safe_Release(m_pTexturesCom_Color);
	Safe_Release(m_pRectInstanceCom);

	Safe_Delete_Array(m_pInstanceBuffer);
	Safe_Delete_Array(m_pInstance_Dir);

	__super::Free();
}
