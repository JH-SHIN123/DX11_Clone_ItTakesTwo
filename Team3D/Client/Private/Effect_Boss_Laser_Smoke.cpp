#include "stdafx.h"
#include "..\Public\Effect_Boss_Laser_Smoke.h"
#include "DataStorage.h"

CEffect_Boss_Laser_Smoke::CEffect_Boss_Laser_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Laser_Smoke::CEffect_Boss_Laser_Smoke(const CEffect_Boss_Laser_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Laser_Smoke::NativeConstruct_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CEffect_Boss_Laser_Smoke::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_Boss_Laser_Smoke::Tick(_double TimeDelta)
{
	/*Gara*/ m_pTransformCom->Set_State(CTransform::STATE_POSITION, DATABASE->GetCody()->Get_Position());

	if (m_dInstance_Pos_Update_Time >= m_dControlTime)
		return EVENT_DEAD;
	if (false == m_IsActivate)
		m_dControlTime += TimeDelta;

	Check_Instance(TimeDelta);

	return _int();
}

_int CEffect_Boss_Laser_Smoke::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Laser_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(2, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_Boss_Laser_Smoke::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].fTime -= (_float)TimeDelta * m_fAlphaTime_Power;
		m_pInstance_Pos_UpdateTime[iIndex]	-= TimeDelta;

		if (0.0 >= m_pInstance_Pos_UpdateTime[iIndex] && true == m_IsActivate)
			Reset_Instance(TimeDelta, vMyPos, iIndex);

		Instance_Size((_float)TimeDelta, iIndex);
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_UV((_float)TimeDelta, iIndex);
	}
}

void CEffect_Boss_Laser_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x += TimeDelta * m_fSize_Power;
	m_pInstanceBuffer_STT[iIndex].vSize.y += TimeDelta * m_fSize_Power;
}

void CEffect_Boss_Laser_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition.y += TimeDelta * m_fInstance_SpeedPerSec;
}

void CEffect_Boss_Laser_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
	_float4 vTexUV, vTexUV_Next;
	vTexUV		= m_pInstanceBuffer_STT[iIndex].vTextureUV;
	vTexUV_Next = Get_TexUV_Next(7, 7);

	if (1.f <= vTexUV.x + vTexUV_Next.x)
		return;
	else
	{
		m_pInstanceBuffer_STT[iIndex].vTextureUV.x += vTexUV_Next.x;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.y += vTexUV_Next.y;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.z += vTexUV_Next.z;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.w += vTexUV_Next.w;
	}	
}

void CEffect_Boss_Laser_Smoke::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition		= vPos;

	m_pInstanceBuffer_STT[iIndex].vTextureUV	= __super::Get_TexUV(7, 7, true);
	m_pInstanceBuffer_STT[iIndex].fTime			= 0.f;
	m_pInstanceBuffer_STT[iIndex].vSize			= m_vDefaultSize;

	m_pInstance_Pos_UpdateTime[iIndex]			= m_dInstance_Pos_Update_Time;
}

HRESULT CEffect_Boss_Laser_Smoke::Ready_InstanceBuffer()
{
	_int iInstanceCount	= m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT				= new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Pos_UpdateTime			= new _double[iInstanceCount];
	m_dInstance_Update_TextureUV_Time	= new _double[iInstanceCount];

	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook			= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vPosition		= vMyPos;

		m_pInstanceBuffer_STT[iIndex].vTextureUV	= __super::Get_TexUV(7, 7, true);
		m_pInstanceBuffer_STT[iIndex].fTime			= 0.f;
		m_pInstanceBuffer_STT[iIndex].vSize			= m_vDefaultSize;

		m_pInstance_Pos_UpdateTime[iIndex]			= m_dInstance_Pos_Update_Time * 0.5 * _double(iIndex);
	}

	return S_OK;
}

CEffect_Boss_Laser_Smoke * CEffect_Boss_Laser_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Laser_Smoke*	pInstance = new CEffect_Boss_Laser_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Laser_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Laser_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Laser_Smoke* pInstance = new CEffect_Boss_Laser_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Laser_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Laser_Smoke::Free()
{
	Safe_Delete_Array(m_dInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
