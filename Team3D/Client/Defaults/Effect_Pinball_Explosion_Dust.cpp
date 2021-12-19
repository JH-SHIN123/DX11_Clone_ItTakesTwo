#include "stdafx.h"
#include "Effect_Pinball_Explosion_Dust.h"

CEffect_Pinball_Explosion_Dust::CEffect_Pinball_Explosion_Dust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Pinball_Explosion_Dust::CEffect_Pinball_Explosion_Dust(const CEffect_Pinball_Explosion_Dust & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Pinball_Explosion_Dust::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = 17;

	return S_OK;
}

HRESULT CEffect_Pinball_Explosion_Dust::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix WorldMatrix = __super::Normalize_Matrix(XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix));
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Pinball_Explosion_Dust::Tick(_double TimeDelta)
{
	if (3.0 < m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;

	if (true == m_IsActivate)
	{
		m_dControlTime += TimeDelta;
		if (1.0 < m_dControlTime) m_dControlTime = 1.0;
	}
	else
	{
		m_dControlTime -= TimeDelta;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	m_fMoveUp_SpeedPerSec += (_float)TimeDelta;

	Check_Target_Matrix();
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Pinball_Explosion_Dust::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Pinball_Explosion_Dust::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	_float4 vColor = { 0.3f, 0.3f, 0.3f, 0.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_Variable("g_vColor", &vColor, sizeof(_float4));

	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));

	m_pPointInstanceCom_STT->Render(17, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Pinball_Explosion_Dust::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * 0.45f;
		if (0.f >= m_pInstanceBuffer_STT[iIndex].fTime)
			m_pInstanceBuffer_STT[iIndex].fTime = 0.f;

		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_Pinball_Explosion_Dust::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * 0.25f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
	m_pInstanceBuffer_STT[iIndex].vSize.y = m_pInstanceBuffer_STT[iIndex].vSize.x;
}

void CEffect_Pinball_Explosion_Dust::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition) + vDir * TimeDelta * 5.f * (m_pInstanceBuffer_STT[iIndex].fTime * m_pInstanceBuffer_STT[iIndex].fTime);

	vPos.m128_f32[1] += m_fMoveUp_SpeedPerSec * TimeDelta * 5.f;

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_Pinball_Explosion_Dust::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Pinball_Explosion_Dust::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;
	m_pInstanceBuffer_STT[iIndex].vSize = Get_RandSize();
	m_pInstanceBuffer_STT[iIndex].fTime = 1.02f;

	m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

	XMStoreFloat3(&m_pInstance_Dir[iIndex], vRandDir);
}

HRESULT CEffect_Pinball_Explosion_Dust::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime = new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = Get_RandSize();

		m_pInstance_Pos_UpdateTime[iIndex] = m_dInstance_Pos_Update_Time;

		_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));

		XMStoreFloat3(&m_pInstance_Dir[iIndex], vRandDir);
	}

	return S_OK;
}

_float3 CEffect_Pinball_Explosion_Dust::Get_Particle_Rand_Dir(_fvector vDefaultPos)
{
	_vector vRandDir = XMLoadFloat3(&__super::Get_Dir_Rand(_int3(100, 100, 100)));
	_vector vPos = vDefaultPos - vRandDir;



	return _float3();
}

void CEffect_Pinball_Explosion_Dust::Check_Target_Matrix()
{
}

_float2 CEffect_Pinball_Explosion_Dust::Get_RandSize()
{
	_int iRand = rand() % 3;
	_float2 vSize = m_vDefaultSize;

	switch (iRand)
	{
	case 0:
		vSize.x -= 0.05f;
		vSize.y -= 0.05f;
		break;
	case 1:
		vSize.x += 0.05f;
		vSize.y += 0.05f;
		break;
	default:
		break;
	}

	return vSize;
}

CEffect_Pinball_Explosion_Dust * CEffect_Pinball_Explosion_Dust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Pinball_Explosion_Dust*	pInstance = new CEffect_Pinball_Explosion_Dust(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Pinball_Explosion_Dust");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Pinball_Explosion_Dust::Clone_GameObject(void * pArg)
{
	CEffect_Pinball_Explosion_Dust* pInstance = new CEffect_Pinball_Explosion_Dust(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Pinball_Explosion_Dust");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Pinball_Explosion_Dust::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
