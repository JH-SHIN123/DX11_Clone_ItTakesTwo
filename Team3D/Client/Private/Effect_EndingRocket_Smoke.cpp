#include "stdafx.h"
#include "..\Public\Effect_EndingRocket_Smoke.h"
#include "DataStorage.h"
#include "EndingRocket.h"

CEffect_EndingRocket_Smoke::CEffect_EndingRocket_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_EndingRocket_Smoke::CEffect_EndingRocket_Smoke(const CEffect_EndingRocket_Smoke & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_EndingRocket_Smoke::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 8;
	return S_OK;
}

HRESULT CEffect_EndingRocket_Smoke::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Fire_Loop"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Cloud"), TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	_matrix WorldMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	WorldMatrix = __super::Normalize_Matrix(WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_EndingRocket_Smoke::Tick(_double TimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	if (true == static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Boost())
		m_IsBoosting = true;


	if (true == m_IsActivate)
	{
		m_IsActivate = true;
		m_dControlTime += TimeDelta;
		if (1.0 <= m_dControlTime) m_dControlTime = 1.0;
	}
	else
	{
		m_IsActivate = false;
		m_dControlTime -= TimeDelta * 0.1;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}

	if (true == m_IsBoosting)
	{
		m_fBoostTime += (_float)TimeDelta;
		if (2.f < m_fBoostTime)
		{
			m_fBoostTime = 0.f;
			m_IsBoosting = false;
		}
	}


	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_EndingRocket_Smoke::Late_Tick(_double TimeDelta)
{
	//Check_TargetMatrix();
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_EndingRocket_Smoke::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(3));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(23, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_EndingRocket_Smoke::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_EndingRocket_Smoke::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	_matrix World = WorldMatrix;

	for (_int i = 0; i < 3; ++i)
		World.r[i] = XMVector3Normalize(World.r[i]);

	World.r[3] += World.r[2] * 0.4f;

	m_pTransformCom->Set_WorldMatrix(World);
}

void CEffect_EndingRocket_Smoke::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//Instance_Size((_float)TimeDelta, 0);

	_int iInstanceCount_Max = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount_Max; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vPosition = vMyPos;
		Instance_UV((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_EndingRocket_Smoke::Instance_Size(_float TimeDelta, _int iIndex)
{
	if (false == m_IsBoosting)
	{
		m_pInstanceBuffer_STT[iIndex].vSize.x -= (_float)TimeDelta * 2.5f;
		m_pInstanceBuffer_STT[iIndex].vSize.y -= (_float)TimeDelta * 3.f;
		if (m_vDefaultSize.x > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = m_vDefaultSize.x;
		if (m_vDefaultSize.y > m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = m_vDefaultSize.y;
	}
	else
	{		
		m_pInstanceBuffer_STT[iIndex].vSize.x += (_float)TimeDelta * 2.5f;
		m_pInstanceBuffer_STT[iIndex].vSize.y += (_float)TimeDelta * 3.f;
		if (m_vBoostingSize.x < m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = m_vBoostingSize.x;
		if (m_vBoostingSize.y < m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = m_vBoostingSize.y;	
	}
}

void CEffect_EndingRocket_Smoke::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_EndingRocket_Smoke::Instance_UV(_float TimeDelta, _int iIndex)
{
	m_pInstance_Update_TextureUV_Time[iIndex] -= TimeDelta;

	if (true == m_IsBoosting)
		m_pInstance_Update_TextureUV_Time[iIndex] -= TimeDelta;

	if (0 >= m_pInstance_Update_TextureUV_Time[iIndex])
	{
		m_pInstance_Update_TextureUV_Time[iIndex] = 0.01;

		m_pInstanceBuffer_STT[iIndex].vTextureUV.x += m_fNextUV;
		m_pInstanceBuffer_STT[iIndex].vTextureUV.z += m_fNextUV;

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

		else if (1.f <= m_pInstanceBuffer_STT[iIndex].vTextureUV.x)
		{
			m_pInstanceBuffer_STT[iIndex].vTextureUV.x = 0.f;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.z = m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.y += m_fNextUV;
			m_pInstanceBuffer_STT[iIndex].vTextureUV.w += m_fNextUV;
		}
	}
}

void CEffect_EndingRocket_Smoke::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].vTextureUV = __super::Get_TexUV(7, 7, true);
	m_pInstanceBuffer_STT[iIndex].fTime = 1.00f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

	m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;
}

HRESULT CEffect_EndingRocket_Smoke::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Update_TextureUV_Time = new _double[iInstanceCount];

	m_fNextUV = __super::Get_TexUV(8, 8, true).z;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, m_fNextUV , m_fNextUV };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		m_pInstance_Update_TextureUV_Time[iIndex] = 0.05;

		_float fDegree = 180.f * (_float)iIndex / iInstanceCount;
		_matrix RotateMatrix = Rotate_WorldMatrix(WorldMatrix, fDegree, CTransform::STATE_LOOK);

		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vRight,	RotateMatrix.r[0]);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vUp,		RotateMatrix.r[1]);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vLook,		RotateMatrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, WorldMatrix.r[3]);
	}

	return S_OK;
}

void CEffect_EndingRocket_Smoke::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Transform()->Get_WorldMatrix();

	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);
	WorldMatrix.r[3] += WorldMatrix.r[2] * 0.2f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

_matrix CEffect_EndingRocket_Smoke::Rotate_WorldMatrix(_fmatrix WorldMatrix, _float fDegree, CTransform::STATE eState)
{
	return XMMatrixRotationAxis(WorldMatrix.r[eState], XMConvertToRadians(fDegree));
}

CEffect_EndingRocket_Smoke * CEffect_EndingRocket_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_EndingRocket_Smoke*	pInstance = new CEffect_EndingRocket_Smoke(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_EndingRocket_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_EndingRocket_Smoke::Clone_GameObject(void * pArg)
{
	CEffect_EndingRocket_Smoke* pInstance = new CEffect_EndingRocket_Smoke(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_EndingRocket_Smoke");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_EndingRocket_Smoke::Free()
{
	Safe_Delete_Array(m_pInstance_Update_TextureUV_Time);
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
