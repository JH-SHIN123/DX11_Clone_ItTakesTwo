#include "stdafx.h"
#include "..\Public\Effect_Boss_Missile_Particle.h"
#include "DataStorage.h"
#include "Cody.h"

CEffect_Boss_Missile_Particle::CEffect_Boss_Missile_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Missile_Particle::CEffect_Boss_Missile_Particle(const CEffect_Boss_Missile_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Missile_Particle::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 80;
	return S_OK;
}

HRESULT CEffect_Boss_Missile_Particle::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);


	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();
	Set_Parabola();

	return S_OK;
}

_int CEffect_Boss_Missile_Particle::Tick(_double TimeDelta)
{
	if (2.5 < m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;
	m_dControlTime += TimeDelta;

	if (true == m_IsActivate)
	{
		if (1.0 <= m_dControlTime)
			m_dControlTime = 1.0;
	}

	Check_Instance(TimeDelta);
	Check_Parabola(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_Missile_Particle::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CEffect_Boss_Missile_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fAlpha", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(1));
	m_pPointInstanceCom_STT->Render(9, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Missile_Particle::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_Boss_Missile_Particle::Set_Parabola()
{
	m_vDir = __super::Get_Dir_Rand(_int3(200, 10, 200));
	m_vDir.y = 0.f;

	m_fJumpPower += ((_float)(rand() % 5 + 1)) * 0.5f;
	m_fMovePower = (_float)(rand() % 5 + 3) * 1.5f;
	m_fJumpStartPos_Y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
}

void CEffect_Boss_Missile_Particle::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.8f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
		{
			Reset_Instance(TimeDelta, vMyPos, iIndex);
			continue;
		}

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_UV((_float)TimeDelta, iIndex);
	}
}

void CEffect_Boss_Missile_Particle::Check_Parabola(_double TimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMLoadFloat3(&m_vDir);

	m_fJumpTime += 0.02f ;
	_float fUp = _float((m_fJumpPower)* m_fJumpTime * 1.f - 0.5f * (GRAVITY * m_fJumpTime * m_fJumpTime));
	vPos.m128_f32[1] = m_fJumpStartPos_Y + fUp;

	vPos += vDir * (_float)TimeDelta * m_fMovePower;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_Boss_Missile_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * m_fSize_Power * (m_pInstanceBuffer_STT[iIndex].vSize.x * 10.f);
	if (0.f >= m_pInstanceBuffer_STT[iIndex].vSize.x)
		m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;

	m_pInstanceBuffer_STT[iIndex].vSize.y = m_pInstanceBuffer_STT[iIndex].vSize.x;
}

void CEffect_Boss_Missile_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition.y += TimeDelta * 0.1f;
}

void CEffect_Boss_Missile_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_Update_TextureUV_Time[iIndex] -= TimeDelta;

	if (0 >= m_pInstance_Update_TextureUV_Time[iIndex])
	{
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;

		m_pInstanceBuffer_STT[iIndex].vTextureUV.x += m_fNextUV;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.y += m_fNextUV;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.z += m_fNextUV;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.w += m_fNextUV;

		if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.y)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.y = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.w = m_fNextUV;
			if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
			{
				m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 1.f - m_fNextUV;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.y = 1.f - m_fNextUV;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.z = 1.f;
				m_pInstanceBuffer_STT[iIndex].vTextureUV.w = 1.f;
			}
		}

		if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.z = m_fNextUV;
		}
	}
}

void CEffect_Boss_Missile_Particle::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_Boss_Missile_Particle::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_Update_TextureUV_Time = new _double[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(m_vTexUV.x, m_vTexUV.y, true).z;

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV_Rand(m_vTexUV.x, m_vTexUV.y);
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time  * (_double(iIndex) / iInstanceCount);
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
	}
	return S_OK;
}

CEffect_Boss_Missile_Particle * CEffect_Boss_Missile_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Missile_Particle*	pInstance = new CEffect_Boss_Missile_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Missile_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Missile_Particle::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Missile_Particle* pInstance = new CEffect_Boss_Missile_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Missile_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Missile_Particle::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
