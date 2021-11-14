#include "stdafx.h"
#include "..\Public\Player_DeadEffect.h"
#include "GameInstance.h"

CPlayer_DeadEffect::CPlayer_DeadEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CPlayer_DeadEffect::CPlayer_DeadEffect(const CPlayer_DeadEffect & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CPlayer_DeadEffect::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);


	return S_OK;
}

HRESULT CPlayer_DeadEffect::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.vSize = { 1.f,1.f,0.f };
	m_EffectDesc_Prototype.iInstanceCount = 2;

	__super::Ready_Component(pArg);
	Ready_Instance();

	m_pInstanceBuffer[0].vSize = { 7.f, 7.f };
	m_pInstanceBuffer[0].vPosition.y -= 0.5f;
	m_EffectDesc_Clone.UVTime = 0.01;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Dead_Cells"), TEXT("Com_Textrue_Particle"), (CComponent**)&m_pTexturesCom_Particle), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"), TEXT("Com_Textrue_Particle_Mask"), (CComponent**)&m_pTexturesCom_Particle_Mask), E_FAIL);
	
	return S_OK;
}

_int CPlayer_DeadEffect::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	m_pInstanceBuffer[0].vTextureUV = Check_UV((_float)TimeDelta, 0, false);

	for (_int iIndex = 1; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
		Instance_Pos((_float)TimeDelta, iIndex);

	return _int();
}

_int CPlayer_DeadEffect::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CPlayer_DeadEffect::Render()
{
	SetUp_Shader_Data();

	// 연기
	_float4 vColor = { 0.6f, 0.6f, 0.6f, (_float)m_dAlphaTime };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, 1);

	// 나머지 파티클
	//g_vColorRamp_UV
	_float4 vUV = { 0.f,0.f,1.f,1.f };
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vUV, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Particle_Mask->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Particle->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(4, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount - 1, 1);


	return S_OK;
}

void CPlayer_DeadEffect::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);

	vPos += vDir * TimeDelta * 2.f;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

HRESULT CPlayer_DeadEffect::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer	= new VTXMATRIX_CUSTOM_ST[iInstanceCount];

	m_pInstance_Dir		= new _float3[iInstanceCount];
	m_pInstance_UVCount = new _float2[iInstanceCount];

	_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };

	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	_double fRenderTerm = 0.f;
	m_UVTime = m_EffectDesc_Clone.UVTime;

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		
		m_pInstanceBuffer[i].vSize = vSize;

		if (0 < i)
		{
			m_pInstance_UVCount[i] = { _float(rand() % 4), _float(rand() % 2) };
			if (true == m_EffectDesc_Clone.IsCody)
				m_pInstance_UVCount[i].y += 2.f;
		}
		else
			m_pInstance_UVCount[i] = { 0.f, 0.f };
		m_pInstanceBuffer[i].vTextureUV = Set_particleUV(i, 4,4);

		m_pInstance_Dir[i] = _float3(0.f, 0.f, 0.f);
		_vector vRandDir = XMVectorZero();

		_int iRandPower[3] = { (_int)m_EffectDesc_Clone.vRandDirPower.x , (_int)m_EffectDesc_Clone.vRandDirPower.y, (_int)m_EffectDesc_Clone.vRandDirPower.z };
		_int iDir[3] = { 0, 0, 0 };

		for (_int j = 0; j < 3; ++j)
		{
			if (0 != iRandPower[j])
			{
				iDir[j] = rand() % iRandPower[j] + 1;
				if (true == m_EffectDesc_Clone.IsRandDirDown[j] && rand() % 2 == 0)
					iDir[j] *= -1;
			}

			vRandDir.m128_f32[j] = (_float)iDir[j];
		}

		vRandDir = XMVector3Normalize(vRandDir);
		if (0 < i)
		{
			// Pos Setting
			vPos += vRandDir * _float(rand() % 5);
			XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		}


		XMStoreFloat3(&m_pInstance_Dir[i], vRandDir);
		
	}

	m_pInstance_Dir[0] = m_EffectDesc_Clone.vDir;

	return S_OK;
}

_float4 CPlayer_DeadEffect::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft		= (1.f	/ U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop			= (1.f	/ V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight		= (1.f	/ U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom		= (1.f	/ V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

CPlayer_DeadEffect * CPlayer_DeadEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPlayer_DeadEffect*	pInstance = new CPlayer_DeadEffect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CPlayer_DeadEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_DeadEffect::Clone_GameObject(void * pArg)
{
	CPlayer_DeadEffect* pInstance = new CPlayer_DeadEffect(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlayer_DeadEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_DeadEffect::Free()
{
	Safe_Release(m_pTexturesCom_Particle);
	Safe_Release(m_pTexturesCom_Particle_Mask);

	__super::Free();
}
