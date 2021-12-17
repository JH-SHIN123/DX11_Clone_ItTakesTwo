#include "stdafx.h"
#include "..\Public\Effect_Boss_GroundPound_Smoke.h"

CEffect_Boss_GroundPound_Smoke::CEffect_Boss_GroundPound_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_GroundPound_Smoke::CEffect_Boss_GroundPound_Smoke(const CEffect_Boss_GroundPound_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_GroundPound_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 36;

	return S_OK;
}

HRESULT CEffect_Boss_GroundPound_Smoke::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Smoke_Puff_01"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_GroundPound_Smoke::Tick(_double TimeDelta)
{
	if (5.0 <= m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;

	m_dControlTime -= TimeDelta * 0.75;
	if (0.0 >= m_dControlTime)m_dControlTime = 0.0;

	m_fInstance_SpeedPerSec -= (_float)TimeDelta * (m_fInstance_SpeedPerSec * 0.5f);
	if (0.f >= m_fInstance_SpeedPerSec) m_fInstance_SpeedPerSec = 0.f;

	Check_InstanceBuffer(TimeDelta);

	return _int();
}

_int CEffect_Boss_GroundPound_Smoke::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_GroundPound_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.98f, 0.98f, 1.f, 1.f };
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(16, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_GroundPound_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_GroundPound_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstanceBuffer_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);

	vPos += vDir * TimeDelta * m_fInstance_SpeedPerSec;
	vPos.m128_f32[1] += TimeDelta * m_fInstance_SpeedPerSec * 0.35f;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Boss_GroundPound_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
	if (m_vUVCount_Max.x == m_pInstanceBuffer_UVCount[iIndex].x + 1
		&& m_vUVCount_Max.y == m_pInstanceBuffer_UVCount[iIndex].y + 1)
	{
		_float fLeft	= (1.f / 8.f) *  7.f;

		m_pInstanceBuffer[iIndex].vTextureUV = { fLeft , fLeft , 1.f, 1.f };
		return;
	}


	m_pInstance_UVTime[iIndex] -= (_double)TimeDelta;
	if (0.0 >= m_pInstance_UVTime[iIndex])
	{
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;
		m_pInstanceBuffer[iIndex].vTextureUV = __super::Check_UV(m_vUVCount_Max.x, m_vUVCount_Max.y, &m_pInstanceBuffer_UVCount[iIndex].x, &m_pInstanceBuffer_UVCount[iIndex].y);
	}

}

HRESULT CEffect_Boss_GroundPound_Smoke::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstanceBuffer_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstance_UVTime = new _double[iInstanceCount];
	m_pInstanceBuffer_UVCount = new XMINT2[iInstanceCount];

	_float4 vMyPos;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fAngle = 0.f;

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer[iIndex].vTextureUV = __super::Get_TexUV(m_vUVCount_Max.x, m_vUVCount_Max.y, true);
		m_pInstanceBuffer[iIndex].vSize = Get_Rand_Size();
		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;
		m_pInstance_UVTime[iIndex] = m_dTextureUV_Time;

		m_pInstanceBuffer_UVCount[iIndex] = { 0, 0 };

		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);

		_vector vDir = XMMatrixRotationY(XMConvertToRadians(fAngle)).r[2];
		XMStoreFloat3(&m_pInstanceBuffer_Dir[iIndex], vDir);
		fAngle += 10.f;
	}

	return S_OK;
}

_float2 CEffect_Boss_GroundPound_Smoke::Get_Rand_Size()
{
	_float2 vSize = m_vDefaultSize;

	_int iRandNum = rand() % 3;
	switch (iRandNum)
	{
	case 0:
		vSize.x -= 0.5f;
		vSize.y -= 0.5f;
		break;
	case 1:
		vSize.x += 0.5f;
		vSize.y += 0.5f;
		break;
	}

	return vSize;
}

void CEffect_Boss_GroundPound_Smoke::Check_InstanceBuffer(_double TimeDelta)
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

		if (0.0 <= m_pInstance_Pos_UpdateTime[iIndex])
			Instance_UV((_float)TimeDelta, iIndex);

		Instance_Pos((_float)TimeDelta, iIndex);
	}
}

CEffect_Boss_GroundPound_Smoke * CEffect_Boss_GroundPound_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_GroundPound_Smoke*	pInstance = new CEffect_Boss_GroundPound_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_GroundPound_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_GroundPound_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_Boss_GroundPound_Smoke* pInstance = new CEffect_Boss_GroundPound_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_GroundPound_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_GroundPound_Smoke::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_UVCount);
	Safe_Delete_Array(m_pInstanceBuffer_Dir);
	Safe_Delete_Array(m_pInstance_UVTime);

	__super::Free();
}
