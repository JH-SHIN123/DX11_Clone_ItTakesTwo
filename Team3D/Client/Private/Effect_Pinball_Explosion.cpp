#include "stdafx.h"
#include "..\Public\Effect_Pinball_Explosion.h"

#include "DataStorage.h"
#include "Pinball.h"

CEffect_Pinball_Explosion::CEffect_Pinball_Explosion(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Pinball_Explosion::CEffect_Pinball_Explosion(const CEffect_Pinball_Explosion & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Pinball_Explosion::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Pinball_Explosion::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion7x7"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	m_EffectDesc_Prototype.fLifeTime = 3.f;
	m_EffectDesc_Prototype.iTextureCount_U = 7;
	m_EffectDesc_Prototype.iTextureCount_V = 7;

	m_EffectDesc_Clone.UVTime = 0.01f;

	Check_Target_Matrix();
	Ready_Smoke_Effect();

	return S_OK;
}

_int CEffect_Pinball_Explosion::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	m_pInstanceBuffer[0].vTextureUV  = Check_UV_Smoke(TimeDelta);
	m_pInstanceBuffer[0].vSize.x	 += (_float)TimeDelta * 2.75f;
	m_pInstanceBuffer[0].vSize.y	 = m_pInstanceBuffer[0].vSize.x;
	m_pInstanceBuffer[0].vPosition.y += (_float)TimeDelta * 0.5f;

	m_fAlphaTime -= (_float)TimeDelta * 0.75f;
	if (0.f >= m_fAlphaTime) m_fAlphaTime = 0.f;

	return NO_EVENT;
}

_int CEffect_Pinball_Explosion::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Pinball_Explosion::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_int i = 1;
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));
	m_pPointInstanceCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fAlphaTime, sizeof(_float));
	m_pPointInstanceCom->Render(21, m_pInstanceBuffer, 1);

	return S_OK;
}

void CEffect_Pinball_Explosion::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Explosion::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Explosion::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Pinball_Explosion::Ready_Smoke_Effect()
{
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[1];

	m_pInstanceBuffer[0].vRight = { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer[0].vUp = { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer[0].vLook = { 0.f, 0.f, 1.f, 0.f };
	XMStoreFloat4(&m_pInstanceBuffer[0].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pInstanceBuffer[0].vTextureUV = __super::Get_TexUV(m_EffectDesc_Prototype.iTextureCount_U, m_EffectDesc_Prototype.iTextureCount_V, true);
	m_pInstanceBuffer[0].vSize = { 2.f, 2.f };
	return S_OK;
}

_float4 CEffect_Pinball_Explosion::Check_UV_Smoke(_double TimeDelta)
{
	_float4 vUV = m_pInstanceBuffer[0].vTextureUV;

	if (-1.f == m_EffectDesc_Clone.UVTime)
		return vUV;

	m_UVTime -= TimeDelta;

	if (m_UVTime <= 0.f)
	{

		if (m_vTexUV_Smoke.y == m_EffectDesc_Prototype.iTextureCount_V - 1)
		{
			if (m_vTexUV_Smoke.x == m_EffectDesc_Prototype.iTextureCount_U - 1)
				return vUV = { 1.f, 1.f, 1.f, 1.f };
		}


		m_UVTime = m_EffectDesc_Clone.UVTime;

		if (m_vTexUV_Smoke.x >= m_EffectDesc_Prototype.iTextureCount_U - 1)
		{
			m_vTexUV_Smoke.x = 0;
			m_vTexUV_Smoke.y += 1.f;
		}
		else
			m_vTexUV_Smoke.x += 1.f;

		if (m_vTexUV_Smoke.y >= m_EffectDesc_Prototype.iTextureCount_V)
			m_vTexUV_Smoke.y = 0;

		_float fLeft = (1.f / m_EffectDesc_Prototype.iTextureCount_U) *  m_vTexUV_Smoke.x;
		_float fTop = (1.f / m_EffectDesc_Prototype.iTextureCount_V) *  m_vTexUV_Smoke.y;
		_float fRight = (1.f / m_EffectDesc_Prototype.iTextureCount_U) * (m_vTexUV_Smoke.x + 1.f);
		_float fBottom = (1.f / m_EffectDesc_Prototype.iTextureCount_V) * (m_vTexUV_Smoke.y + 1.f);

		vUV = { fLeft, fTop, fRight, fBottom };
	}

	return vUV;
}

void CEffect_Pinball_Explosion::Check_Target_Matrix()
{

}

CEffect_Pinball_Explosion * CEffect_Pinball_Explosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Pinball_Explosion*	pInstance = new CEffect_Pinball_Explosion(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Pinball_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Pinball_Explosion::Clone_GameObject(void * pArg)
{
	CEffect_Pinball_Explosion* pInstance = new CEffect_Pinball_Explosion(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Pinball_Explosion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Pinball_Explosion::Free()
{
	__super::Free();
}
