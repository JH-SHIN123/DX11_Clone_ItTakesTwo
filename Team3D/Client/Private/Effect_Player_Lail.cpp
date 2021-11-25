#include "stdafx.h"
#include "..\Public\Effect_Player_Lail.h"
#include "GameInstance.h"

CEffect_Player_Lail::CEffect_Player_Lail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Lail::CEffect_Player_Lail(const CEffect_Player_Lail & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Player_Lail::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Player_Lail::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);
	FAILED_CHECK_RETURN(Ready_InstanceBuffer_Spark(), E_FAIL);


	return S_OK;
}

_int CEffect_Player_Lail::Tick(_double TimeDelta)
{
	Check_Update_Loop(TimeDelta);
	Check_Spark(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Player_Lail::Late_Tick(_double TimeDelta)
{
	m_IsActivate = false;

	return NO_EVENT;
}

HRESULT CEffect_Player_Lail::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

void CEffect_Player_Lail::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Player_Lail::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Player_Lail::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Player_Lail::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE,	TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Texture_Diff"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Texture_Mask"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	// 스모크


	return S_OK;
}

HRESULT CEffect_Player_Lail::Ready_InstanceBuffer_Spark()
{
	m_pInstanceBuffer_Spark = new VTXMATRIX_CUSTOM_STT[m_iInstanceCount_Spark];
	m_pSpeedPerSec_Spark	= new _float[m_iInstanceCount_Spark];
	m_pMoveDir_Spark		= new _float3[m_iInstanceCount_Spark];

	_float4 vPos;
	//XMStoreFloat4(&vPos, ParentMatrix.r[3]);

	_int iInstanceCount_LeftToe = m_iInstanceCount_Spark >> 1;
	for (_int iIndex = 0; iIndex < iInstanceCount_LeftToe; ++iIndex)
	{
		m_pInstanceBuffer_Spark[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vPosition	= vPos;
		m_pInstanceBuffer_Spark[iIndex].vSize		= { 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vTextureUV	= { 0.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].fTime		= m_fSpark_RespawnTime * (_float)iInstanceCount_LeftToe * iIndex;
		m_pSpeedPerSec_Spark[iIndex]				= m_fSpeedPerSec_Spark;
		m_pMoveDir_Spark[iIndex]					= Get_Dir_Spark(_int3(100, 100, 100));
	}

	for (_int iIndex = iInstanceCount_LeftToe; iIndex < m_iInstanceCount_Spark; ++iIndex)
	{
		m_pInstanceBuffer_Spark[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vPosition	= vPos;
		m_pInstanceBuffer_Spark[iIndex].vSize		= { 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].vTextureUV	= { 0.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Spark[iIndex].fTime		= m_fSpark_RespawnTime * (_float)iInstanceCount_LeftToe * iIndex;
		m_pSpeedPerSec_Spark[iIndex]				= m_fSpeedPerSec_Spark;
		m_pMoveDir_Spark[iIndex]					= Get_Dir_Spark(_int3(100, 100, 100));
	}

	return S_OK;
}

void CEffect_Player_Lail::Check_Update_Loop(_double TimeDelta)
{
}

void CEffect_Player_Lail::Check_Spark(_double TimeDelta)
{
	_int iInstanceCount_Left = m_iInstanceCount_Spark >> 1;

	_matrix WorldMatrix = XMLoadFloat4x4(&m_BoneMatrix_Left) * m_pTransformCom->Get_WorldMatrix();
	Update_Spark(TimeDelta, 0, iInstanceCount_Left, WorldMatrix);

	WorldMatrix = XMLoadFloat4x4(&m_BoneMatrix_Right) * m_pTransformCom->Get_WorldMatrix();
	Update_Spark(TimeDelta, iInstanceCount_Left, m_iInstanceCount_Spark, WorldMatrix);
}

void CEffect_Player_Lail::Update_Spark(_double TimeDelta, _int iStartIndex, _int iEndIndex, _fmatrix ParentMatrix)
{
	//ParentMatrix는 Reset할 때만 Pos만 똑 떼오셈
	for (_int iIndex = iStartIndex; iIndex < iEndIndex; ++iIndex)
	{
		m_pInstanceBuffer_Spark[iIndex].fTime -= TimeDelta;
		if (0.f >= m_pInstanceBuffer_Spark[iIndex].fTime)
			Reset_Spark(iIndex, ParentMatrix);

		m_pSpeedPerSec_Spark[iIndex] -= (_float)TimeDelta * 0.02f;
		if (0.1f >= m_pSpeedPerSec_Spark[iIndex])
			m_pSpeedPerSec_Spark[iIndex] = m_fSpeedPerSec_Spark;

		_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_Spark[iIndex].vPosition);
		_vector vDir = XMLoadFloat3(&m_pMoveDir_Spark[iIndex]);

		vPos += vDir * (_float)TimeDelta * m_pSpeedPerSec_Spark[iIndex];
		XMStoreFloat4(&m_pInstanceBuffer_Spark[iIndex].vPosition, vPos);
	}
}

void CEffect_Player_Lail::Reset_Spark(_int iIndex, _fmatrix ParentMatrix)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, ParentMatrix.r[3]); 

	m_pInstanceBuffer_Spark[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer_Spark[iIndex].vUp			= Get_UpVector_Rand(_int3(20, 20, 20));
	m_pInstanceBuffer_Spark[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer_Spark[iIndex].vPosition	= vPos;
	m_pInstanceBuffer_Spark[iIndex].vSize		= { 0.f, 0.f };
	m_pInstanceBuffer_Spark[iIndex].vTextureUV	= { 0.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer_Spark[iIndex].fTime		= m_fSpark_RespawnTime * ((_float)m_iInstanceCount_Spark >> 1) * iIndex;
	m_pSpeedPerSec_Spark[iIndex]				= m_fSpeedPerSec_Spark;
	m_pMoveDir_Spark[iIndex]					= Get_Dir_Spark(_int3(100, 100, 100));
}

_float3 CEffect_Player_Lail::Get_Dir_Spark(_int3 vRandPower)
{
	_vector vAddDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f;
	_float3 vRandDir = Get_Dir_Rand(vRandPower);

	if (0.f >= vRandDir.y)
		vRandDir.y *= -1.f;
	if (0.f <= vRandDir.z)
		vRandDir.z *= -1.f;

	_vector vDir = XMLoadFloat3(&vRandDir) + (vAddDir * 1.5f);
	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&vRandDir, vDir);

	return vRandDir;
}

CEffect_Player_Lail * CEffect_Player_Lail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	return nullptr;
}

CGameObject * CEffect_Player_Lail::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CEffect_Player_Lail::Free()
{
}
