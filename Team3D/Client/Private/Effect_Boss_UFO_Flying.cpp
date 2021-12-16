#include "stdafx.h"
#include "..\Public\Effect_Boss_UFO_Flying.h"
#include "DataStorage.h"
#include "UFO.h"

CEffect_Boss_UFO_Flying::CEffect_Boss_UFO_Flying(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_UFO_Flying::CEffect_Boss_UFO_Flying(const CEffect_Boss_UFO_Flying & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_UFO_Flying::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_UFO_Flying::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pBufferRectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Tex1"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ring"), TEXT("Com_Tex2"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	m_vOffSet_Pos.y = m_EffectDesc_Clone.fSizePower;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(65.f, 0.3f, 30.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(13.8f, 1.f, 13.8f, 0.f));

	m_pUFO = static_cast<CUFO*>(DATABASE->Get_BossUFO());
	Safe_AddRef(m_pUFO);



	return S_OK;
}

_int CEffect_Boss_UFO_Flying::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.f >= m_fTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	if (true == m_IsActivate)
	{
		m_fTime += (_float)TimeDelta * 0.25f;
		if (1.f < m_fTime)
			m_fTime = 1.f;
	}
	else
	{
		m_fTime -= (_float)TimeDelta * 0.25f;
		if (0.0f > m_fTime)
			m_fTime = 0.0f;
	}

	m_dRotateTime += TimeDelta * 10;
	if (360.0 < m_dRotateTime)
		m_dRotateTime = 0.0;

	Check_ParentMatrix();

	return _int();
}

_int CEffect_Boss_UFO_Flying::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_UFO_Flying::Render(RENDER_GROUP::Enum eGroup)
{
	_int i = 0;
	_float fRadian = XMConvertToRadians((_float)m_dRotateTime);
	m_pBufferRectCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	m_pBufferRectCom->Set_Variable("g_fRadianAngle", &fRadian, sizeof(_float));
	m_pBufferRectCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pBufferRectCom->Set_Variable("g_fAlpha", &m_fTime, sizeof(_float));

	m_pBufferRectCom->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom->Get_ShaderResourceView(2));
	m_pBufferRectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(1));
	m_pBufferRectCom->Render(3);

	return S_OK;
}

void CEffect_Boss_UFO_Flying::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_UFO_Flying::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_UFO_Flying::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Boss_UFO_Flying::Ready_Instance()
{
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST;

	m_pInstanceBuffer->vRight = { 1.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vUp = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vLook = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vSize = m_vSize;
	m_pInstanceBuffer->vTextureUV = { 0.f, -1.f, 1.f, 3.f };
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_pInstanceBuffer->vPosition, vPos);

	return S_OK;
}

void CEffect_Boss_UFO_Flying::Check_ParentMatrix()
{
	_matrix ParentMatrix = m_pUFO->Get_Model()->Get_BoneMatrix("Base") * m_pUFO->Get_Transform()->Get_WorldMatrix();
	for (_int i = 0; i < 3; ++i)
		ParentMatrix.r[i] = XMVector3Normalize(ParentMatrix.r[i]) * 5.75f;
	m_pTransformCom->Set_WorldMatrix(ParentMatrix);

	_vector vOffSet = XMLoadFloat3(&m_vOffSet_Pos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vOffSet);

	m_pTransformCom->RotatePitch_Angle(90.f);
}

void CEffect_Boss_UFO_Flying::Check_RotateUV(_double TimeDelta)
{
	m_dRotateTime += TimeDelta * 30;
	if (360.0 < m_dRotateTime)
		m_dRotateTime = 0.0;

	_float fRadian = XMConvertToRadians((_float)m_dRotateTime);
}

CEffect_Boss_UFO_Flying * CEffect_Boss_UFO_Flying::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_UFO_Flying*	pInstance = new CEffect_Boss_UFO_Flying(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_UFO_Flying");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_UFO_Flying::Clone_GameObject(void * pArg)
{
	CEffect_Boss_UFO_Flying* pInstance = new CEffect_Boss_UFO_Flying(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_UFO_Flying");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_UFO_Flying::Free()
{
	Safe_Release(m_pUFO);
	Safe_Release(m_pBufferRectCom);

	__super::Free();
}
