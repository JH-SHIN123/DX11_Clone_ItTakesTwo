#include "stdafx.h"
#include "..\Public\Effect_EndingRocket_Boost.h"

#include "stdafx.h"
#include "..\Public\Effect_EndingRocket_Smoke.h"
#include "DataStorage.h"
#include "EndingRocket.h"

CEffect_EndingRocket_Boost::CEffect_EndingRocket_Boost(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_EndingRocket_Boost::CEffect_EndingRocket_Boost(const CEffect_EndingRocket_Boost & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_EndingRocket_Boost::NativeConstruct_Prototype(void * pArg)
{
	m_EffectDesc_Prototype.iInstanceCount = 30;
	return S_OK;
}

HRESULT CEffect_EndingRocket_Boost::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);

	Check_TargetMatrix();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CEffect_EndingRocket_Boost::Tick(_double TimeDelta)
{
	if (5.f <= m_fMoveTimeStack)
		return EVENT_DEAD;

	if (true == m_IsActivate)
	{
		m_IsActivate = true;
		m_dControlTime += TimeDelta;
		if (0.5 <= m_dControlTime) m_dControlTime = 0.5;
	}
	else
	{
		m_IsActivate = false;
		m_dControlTime -= TimeDelta * 0.1;
		if (0.0 > m_dControlTime) m_dControlTime = 0.0;
	}


	
	Check_Instance(TimeDelta);

	return NO_EVENT;
}

_int CEffect_EndingRocket_Boost::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_EndingRocket_Boost::Render(RENDER_GROUP::Enum eGroup)
{
	_float fTime = (_float)m_dControlTime;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();
	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fTime, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(5));
	m_pPointInstanceCom_STT->Render(12, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

void CEffect_EndingRocket_Boost::Set_Pos(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_EndingRocket_Boost::Check_Instance(_double TimeDelta)
{
	_float4 vMyPos;
	XMStoreFloat4(&vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//Instance_Size((_float)TimeDelta, 0);

	_int iInstanceCount_Max = m_EffectDesc_Prototype.iInstanceCount;
	for (_int iIndex = 0; iIndex < iInstanceCount_Max; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}
}

void CEffect_EndingRocket_Boost::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vSize.x -= TimeDelta * 0.15f;
	m_pInstanceBuffer_STT[iIndex].vSize.y -= TimeDelta * 0.15f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.x) m_pInstanceBuffer_STT[iIndex].vSize.x = 0.f;
	if (0.f > m_pInstanceBuffer_STT[iIndex].vSize.y) m_pInstanceBuffer_STT[iIndex].vSize.y = 0.f;
}

void CEffect_EndingRocket_Boost::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition);
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

	vPos += vDir * (_float)TimeDelta * 5.5f;

	XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);
}

void CEffect_EndingRocket_Boost::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_EndingRocket_Boost::Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex)
{
	m_pInstanceBuffer_STT[iIndex].vPosition = vPos;

	m_pInstanceBuffer_STT[iIndex].fTime = 1.00f;
	m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;
}

HRESULT CEffect_EndingRocket_Boost::Ready_InstanceBuffer()
{
	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer_STT = new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];


	m_fNextUV = __super::Get_TexUV(8, 8, true).z;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer_STT[iIndex].vRight = { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_STT[iIndex].vTextureUV = { 0.f, 0.f, 1.f , 1.f };
		m_pInstanceBuffer_STT[iIndex].fTime = 1.f;
		m_pInstanceBuffer_STT[iIndex].vSize = m_vDefaultSize;

		_float fDegree = 360.f * (_float)iIndex / iInstanceCount;
		_matrix RotateMatrix = Rotate_WorldMatrix(WorldMatrix, fDegree, CTransform::STATE_LOOK);

		_vector vPos = WorldMatrix.r[3] + (RotateMatrix.r[0] * 0.5f) + WorldMatrix.r[2];
		XMStoreFloat4(&m_pInstanceBuffer_STT[iIndex].vPosition, vPos);

		_vector vDir = XMVector3Normalize(RotateMatrix.r[1] + (WorldMatrix.r[2] * 5.f));
		XMStoreFloat3(&m_pInstance_Dir[iIndex], vDir);
	}

	return S_OK;
}

void CEffect_EndingRocket_Boost::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CEndingRocket*>(DATABASE->Get_EndingRocket())->Get_Transform()->Get_WorldMatrix();

	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

}

_matrix CEffect_EndingRocket_Boost::Rotate_WorldMatrix(_fmatrix WorldMatrix, _float fDegree, CTransform::STATE eState)
{
	return XMMatrixRotationAxis(WorldMatrix.r[eState], XMConvertToRadians(fDegree));
}

CEffect_EndingRocket_Boost * CEffect_EndingRocket_Boost::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_EndingRocket_Boost*	pInstance = new CEffect_EndingRocket_Boost(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_EndingRocket_Boost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_EndingRocket_Boost::Clone_GameObject(void * pArg)
{
	CEffect_EndingRocket_Boost* pInstance = new CEffect_EndingRocket_Boost(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_EndingRocket_Boost");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_EndingRocket_Boost::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer_STT);

	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pPointInstanceCom_STT);

	__super::Free();
}
