#include "stdafx.h"
#include "..\Public\Effect_Boss_BrokenLaser_Flow.h"
#include "DataStorage.h"
#include "UFO.h"

CEffect_Boss_BrokenLaser_Flow::CEffect_Boss_BrokenLaser_Flow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_BrokenLaser_Flow::CEffect_Boss_BrokenLaser_Flow(const CEffect_Boss_BrokenLaser_Flow & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_BrokenLaser_Flow::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 30;
	return S_OK;
}

HRESULT CEffect_Boss_BrokenLaser_Flow::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Spark_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	Check_TargetMatrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_BrokenLaser_Flow::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 5.0 < m_dActivateTime)
		return EVENT_DEAD;

	Check_TargetMatrix();

	m_dActivateTime += TimeDelta;
	if (2.0 < m_dActivateTime)
		m_IsActivate = false;

	if (true == m_IsActivate)
	{
		m_dAlphaTime += TimeDelta * 0.5f;
		if (1.0 <= m_dAlphaTime) m_dAlphaTime = 1.0;
	}
	else
	{
		m_dAlphaTime -= TimeDelta * 0.5f;
		if (0.0 >= m_dAlphaTime) m_dAlphaTime = 0.0;
	}

	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Boss_BrokenLaser_Flow::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_BrokenLaser_Flow::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dAlphaTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(2));

	m_pPointInstanceCom_STT->Render(11, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_BrokenLaser_Flow::Check_Instance(_double TimeDelta)
{
	_fmatrix ParentMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fAlphaTime = (_float)m_dInstance_Pos_Update_Time / 3.f;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.45f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
		{
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;
			continue;
		}

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_Pos((_float)TimeDelta, iIndex);
	}
}

void CEffect_Boss_BrokenLaser_Flow::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CUFO*>(DATABASE->Get_BossUFO())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CUFO*>(DATABASE->Get_BossUFO())->Get_Model()->Get_BoneMatrix("LaserPiston");
	WorldMatrix = BoneMatrix * WorldMatrix;
	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CEffect_Boss_BrokenLaser_Flow::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * 0.5f;
	m_pInstanceBuffer_STT[iIndex].vSize.y -= TimeDelta * 0.8f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;
}

void CEffect_Boss_BrokenLaser_Flow::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstanceBuffer_Dir[iIndex]);
	_float	fParabola_Weight = _float((m_pInstanceBuffer_Parabola_Power[iIndex]) * m_pInstanceBuffer_Parabola_Time[iIndex] * 2.f - 0.5f * (GRAVITY * m_pInstanceBuffer_Parabola_Time[iIndex] * m_pInstanceBuffer_Parabola_Time[iIndex]));

	m_pInstanceBuffer_Parabola_Time[iIndex] += 0.02f;

	vPos += vDir * TimeDelta * 2.f;
	vPos.m128_f32[1] = fParabola_Weight + m_pInstanceBuffer_Parabola_PosY[iIndex];

	_vector vUp = XMVector3Normalize(vPos - XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition));

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp, vUp);
	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Boss_BrokenLaser_Flow::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_BrokenLaser_Flow::Reset_Instance(_double TimeDelta, _fmatrix ParentMatrix, _int iIndex)
{
}

HRESULT CEffect_Boss_BrokenLaser_Flow::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];
	m_pInstanceBuffer_Parabola_PosY = new _float[iInstanceCount];
	m_pInstanceBuffer_Parabola_Time = new _float[iInstanceCount];
	m_pInstanceBuffer_Parabola_Power = new _float[iInstanceCount];
	m_pInstanceBuffer_Dir = new _float3[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_matrix ParentMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f, 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = { 0.3f, 0.6f };

		_vector vRandPos = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(2, 2, 100)));
		vRandPos.m128_f32[0] = 0.f;
		vRandPos.m128_f32[1] = 0.f;
		_float fRandPower = ((_float)(rand() % 10) + 0.5f) * 0.03f;
		vRandPos *= fRandPower;
		vRandPos.m128_f32[3] = 1.f;

		vRandPos = XMVector3Transform(vRandPos, ParentMatrix);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vRandPos);
		m_pInstanceBuffer_Parabola_PosY[iIndex] = m_pInstanceBuffer_STT[iIndex].vPosition.y;
		m_pInstanceBuffer_Parabola_Time[iIndex] = 1.f;
		m_pInstanceBuffer_Parabola_Power[iIndex] = (_float)((rand() % 20 + 1) / 20.f) + 1.5f;

		vRandPos = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 50, 100)));
		XMStoreFloat3(&m_pInstanceBuffer_Dir[iIndex], vRandPos);
	}

	return S_OK;
}

CEffect_Boss_BrokenLaser_Flow * CEffect_Boss_BrokenLaser_Flow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_BrokenLaser_Flow*	pInstance = new CEffect_Boss_BrokenLaser_Flow(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_BrokenLaser_Flow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_BrokenLaser_Flow::Clone_GameObject(void * pArg)
{
	CEffect_Boss_BrokenLaser_Flow* pInstance = new CEffect_Boss_BrokenLaser_Flow(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_BrokenLaser_Flow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_BrokenLaser_Flow::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Delete_Array(m_pInstanceBuffer_Parabola_PosY);
	Safe_Delete_Array(m_pInstanceBuffer_Parabola_Time);
	Safe_Delete_Array(m_pInstanceBuffer_Parabola_Power);
	Safe_Delete_Array(m_pInstanceBuffer_Dir);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
