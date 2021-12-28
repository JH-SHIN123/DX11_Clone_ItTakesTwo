#include "stdafx.h"
#include "..\Public\Effect_WallLaser_Trap_Plane.h"

CEffect_WallLaser_Trap_Plane::CEffect_WallLaser_Trap_Plane(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_WallLaser_Trap_Plane::CEffect_WallLaser_Trap_Plane(const CEffect_WallLaser_Trap_Plane & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_WallLaser_Trap_Plane::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_WallLaser_Trap_Plane::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_ColorRamp"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += XMLoadFloat3(&m_vOffSet_Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	m_vWeight = m_pInstanceBuffer[0].vTextureUV;
	m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);

	m_IsBillBoard = false;

	m_pInstanceBuffer[0].vTextureUV.x = 0.f;
	m_pInstanceBuffer[0].vTextureUV.y = 0.f;
	m_pInstanceBuffer[0].vTextureUV.z = 1.f;
	m_pInstanceBuffer[0].vTextureUV.w = 1.f;

	m_pInstanceBuffer[0].vSize = { 3.6f, 10.f };


	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));// = { 10.f, 5.f, 10.f,1.f };


	return S_OK;
}

_int CEffect_WallLaser_Trap_Plane::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (true == m_IsActivate)
	{
		m_dActivateTime += TimeDelta * 1.5;
		if (0.25 <= m_dActivateTime)
			m_dActivateTime = 0.25;
	}
	else
	{
		m_dActivateTime -= TimeDelta * 4.0;
		if (0.0 >= m_dActivateTime)
			m_dActivateTime = 0.0;
	}

	_float Time = (_float)TimeDelta * 0.3f;

	m_vWeight.y += Time;
	m_vWeight.w += Time;

	dResetTime += Time;
	if (1.f <= dResetTime)
	{
		dResetTime = 0.f;
		m_vWeight = _float4(0.25f, 0.25f, 0.75f, 0.75f);
		m_pInstanceBuffer[0].vTextureUV = _float4(0.f, 0.f, 1.f, 1.f);
	}

	return _int();
}

_int CEffect_WallLaser_Trap_Plane::Late_Tick(_double TimeDelta)
{
	return  m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_WallLaser_Trap_Plane::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(m_EffectDesc_Prototype.iTextureNum_Second));

	_float4 vColorRamp = { 0.0f, 0.0f, 2.0f, 2.0f };
	_float fTime = (_float)m_dActivateTime;

	m_pPointInstanceCom->Set_Variable("g_vUV", &m_vWeight, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vColorRamp, sizeof(_float4));
	m_pPointInstanceCom->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(3));

	m_pPointInstanceCom->Render(26, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_WallLaser_Trap_Plane::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CEffect_WallLaser_Trap_Plane::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	_matrix Matrix = __super::Normalize_Matrix(WorldMatrix);
	Matrix.r[3] += Matrix.r[1] * 0.14f;

	XMStoreFloat4(&m_pInstanceBuffer[0].vRight, 	Matrix.r[0]);
	XMStoreFloat4(&m_pInstanceBuffer[0].vUp,		Matrix.r[2]);
	XMStoreFloat4(&m_pInstanceBuffer[0].vLook,		Matrix.r[1]);
	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition,	Matrix.r[3]);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CEffect_WallLaser_Trap_Plane::Update_Trigger_Position()
{
}

void CEffect_WallLaser_Trap_Plane::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_WallLaser_Trap_Plane::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_WallLaser_Trap_Plane::Instance_UV(_float TimeDelta, _int iIndex)
{
}

CEffect_WallLaser_Trap_Plane * CEffect_WallLaser_Trap_Plane::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_WallLaser_Trap_Plane*	pInstance = new CEffect_WallLaser_Trap_Plane(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_WallLaser_Trap_Plane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_WallLaser_Trap_Plane::Clone_GameObject(void * pArg)
{
	CEffect_WallLaser_Trap_Plane* pInstance = new CEffect_WallLaser_Trap_Plane(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_WallLaser_Trap_Plane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_WallLaser_Trap_Plane::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);

	__super::Free();
}
