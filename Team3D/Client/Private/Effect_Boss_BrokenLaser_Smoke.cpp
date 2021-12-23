#include "stdafx.h"
#include "Effect_Boss_BrokenLaser_Smoke.h"
#include "UFO.h"
#include "DataStorage.h"
#include "May.h"

CEffect_Boss_BrokenLaser_Smoke::CEffect_Boss_BrokenLaser_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_BrokenLaser_Smoke::CEffect_Boss_BrokenLaser_Smoke(const CEffect_Boss_BrokenLaser_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_BrokenLaser_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_BrokenLaser_Smoke::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	m_EffectDesc_Prototype.fLifeTime = 5.f;
	m_EffectDesc_Prototype.iTextureCount_U = 8;
	m_EffectDesc_Prototype.iTextureCount_V = 8;

	m_EffectDesc_Clone.UVTime = 0.01f;

	m_EffectDesc_Prototype.iInstanceCount = 10;

	Check_Target_Matrix();
	Ready_Smoke_Effect();

	return S_OK;
}

_int CEffect_Boss_BrokenLaser_Smoke::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	for (_int i = 0; i < m_EffectDesc_Prototype.iInstanceCount; ++i)
	{
		//m_pInstanceBuffer[i].vTextureUV = Check_UV_Smoke(TimeDelta, i);
		m_pInstanceBuffer[i].vSize.x += (_float)TimeDelta * 3.75f;
		m_pInstanceBuffer[i].vSize.y = m_pInstanceBuffer[i].vSize.x;

		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[i]);
		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[i].vPosition);

		vPos += vDir * (_float)TimeDelta * m_EffectDesc_Prototype.fLifeTime;

		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
	}

	m_fAlphaTime -= (_float)TimeDelta * 0.25f;
	if (0.f >= m_fAlphaTime) m_fAlphaTime = 0.f;

	return NO_EVENT;
}

_int CEffect_Boss_BrokenLaser_Smoke::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Boss_BrokenLaser_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_int i = 1;
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(1));
	m_pPointInstanceCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fAlphaTime, sizeof(_float));
	m_pPointInstanceCom->Render(22, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_BrokenLaser_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_BrokenLaser_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_BrokenLaser_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Boss_BrokenLaser_Smoke::Ready_Smoke_Effect()
{
	m_pInstanceBuffer	= new VTXMATRIX_CUSTOM_ST[m_EffectDesc_Prototype.iInstanceCount];
	m_pInstance_Dir		= new _float3[m_EffectDesc_Prototype.iInstanceCount];

	for (_int i = 0; i < m_EffectDesc_Prototype.iInstanceCount; ++i)
	{
		m_pInstanceBuffer[i].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[i].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer[i].vLook = { 0.f, 0.f, 1.f, 0.f };
		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pInstanceBuffer[i].vTextureUV = __super::Get_TexUV(m_EffectDesc_Prototype.iTextureCount_U, m_EffectDesc_Prototype.iTextureCount_V, true);
		m_pInstanceBuffer[i].vSize = { 2.75f, 2.75f };

		m_pInstance_Dir[i] = __super::Get_Dir_Rand(_int3(100, 100, 100));
		m_pInstance_Dir[i].y *= 0.5f;
	}

	return S_OK;
}

_float4 CEffect_Boss_BrokenLaser_Smoke::Check_UV_Smoke(_double TimeDelta, _int iIndex)
{
	_float4 vUV = m_pInstanceBuffer[iIndex].vTextureUV;

	if (-1.f == m_EffectDesc_Clone.UVTime)
		return vUV;

	m_UVTime -= TimeDelta;

	if (m_UVTime <= 0.f)
	{

		if (m_vTexUV_Smoke.y == m_EffectDesc_Prototype.iTextureCount_V - 1)
		{
			m_vTexUV_Smoke.y = 0;
			if (m_vTexUV_Smoke.x == m_EffectDesc_Prototype.iTextureCount_U - 1)
				m_vTexUV_Smoke.x = 0;
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

void CEffect_Boss_BrokenLaser_Smoke::Check_Target_Matrix()
{
	_vector vPos_May = DATABASE->GetMay()->Get_Position();

	_matrix WorldMatrix = static_cast<CUFO*>(DATABASE->Get_BossUFO())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CUFO*>(DATABASE->Get_BossUFO())->Get_Model()->Get_BoneMatrix("LaserPiston");
	WorldMatrix = BoneMatrix * WorldMatrix;

	_vector vPos_LaserPiston = WorldMatrix.r[3];

	_vector vDir = (vPos_LaserPiston - vPos_May) * 0.5f;

	vPos_May += vDir;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos_May);
}

CEffect_Boss_BrokenLaser_Smoke * CEffect_Boss_BrokenLaser_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_BrokenLaser_Smoke*	pInstance = new CEffect_Boss_BrokenLaser_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_BrokenLaser_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_BrokenLaser_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_Boss_BrokenLaser_Smoke* pInstance = new CEffect_Boss_BrokenLaser_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_BrokenLaser_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_BrokenLaser_Smoke::Free()
{
	__super::Free();
}
