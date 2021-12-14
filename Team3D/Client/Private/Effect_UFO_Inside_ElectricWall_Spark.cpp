#include "stdafx.h"
#include "..\Public\Effect_UFO_Inside_ElectricWall_Spark.h"

CEffect_UFO_Inside_ElectricWall_Spark::CEffect_UFO_Inside_ElectricWall_Spark(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_UFO_Inside_ElectricWall_Spark::CEffect_UFO_Inside_ElectricWall_Spark(const CEffect_UFO_Inside_ElectricWall_Spark & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_UFO_Inside_ElectricWall_Spark::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_UFO_Inside_ElectricWall_Spark::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_ligntning_01"), TEXT("Com_Texture"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Texture_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	for (_int i = 0; i < 3; ++i)
		WolrdMatrix.r[i] = XMVector3Normalize(WolrdMatrix.r[i]);
	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	m_EffectDesc_Prototype.iInstanceCount = 27;

	Ready_Instance();
	Shuffle_ZeroSize_Arr();

	return S_OK;
}

_int CEffect_UFO_Inside_ElectricWall_Spark::Tick(_double TimeDelta)
{
	if (-1 == m_iEffectCount)
		return EVENT_DEAD;

	m_IsNoRender = false;
	Check_Particle_UV(TimeDelta);

	return _int();
}

_int CEffect_UFO_Inside_ElectricWall_Spark::Late_Tick(_double TimeDelta)
{
	if (true == m_IsNoRender)
		return NO_EVENT;

	if (20 == m_iEffectCount)
	{
		m_iEffectCount = -1;
		return NO_EVENT;
	}

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_UFO_Inside_ElectricWall_Spark::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_float4 vColor = m_vColor;
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(8, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

HRESULT CEffect_UFO_Inside_ElectricWall_Spark::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount / 3;
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[m_EffectDesc_Prototype.iInstanceCount];

	_matrix Matrix = m_pTransformCom->Get_WorldMatrix();
	_vector vOffsetPos = Matrix.r[3];
	_vector vRight = Matrix.r[0];

	_float fOffSet = 0.3f;
	_float2 vSize = { 1.05f, 0.2f };
	vOffsetPos -= Matrix.r[2] * fOffSet;
	for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
	{
		m_pInstanceBuffer[iIndex].vSize = vSize;
		m_pInstanceBuffer[iIndex].vTextureUV = Get_TexUV_Rand(8, 8);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vLook, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vRight, Matrix.r[0]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vUp, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vOffsetPos);

		m_pInstanceBuffer[iIndex + 9].vSize = vSize;
		m_pInstanceBuffer[iIndex + 9].vTextureUV = Get_TexUV_Rand(8, 8);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 9].vLook, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 9].vRight, Matrix.r[0]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 9].vUp, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 9].vPosition, vOffsetPos);

		m_pInstanceBuffer[iIndex + 18].vSize = vSize;
		m_pInstanceBuffer[iIndex + 18].vTextureUV = Get_TexUV_Rand(8, 8);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 18].vLook, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 18].vRight, Matrix.r[0]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 18].vUp, Matrix.r[2]);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex + 18].vPosition, vOffsetPos);

		vOffsetPos += Matrix.r[2] * 0.05f;
	}

	return S_OK;
}

void CEffect_UFO_Inside_ElectricWall_Spark::Shuffle_ZeroSize_Arr()
{
	for (_int iIndex = 0; iIndex < 50; ++iIndex)
	{
		_int iDst_Num = rand() % 9;
		_int iSrc_Num = rand() % 9;

 		while (iDst_Num == iSrc_Num) iSrc_Num = rand() % 9;

		_int iDst = m_iZeroSize_Arr[iDst_Num];
		_int iSrc = m_iZeroSize_Arr[iSrc_Num];

		m_iZeroSize_Arr[iDst_Num] = iSrc;
		m_iZeroSize_Arr[iSrc_Num] = iDst;

		_int i = 0;
	}
}

void CEffect_UFO_Inside_ElectricWall_Spark::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_UFO_Inside_ElectricWall_Spark::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_UFO_Inside_ElectricWall_Spark::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_UFO_Inside_ElectricWall_Spark::Check_Particle_UV(_double TimeDelta)
{
	m_fResetTime -= (_float)TimeDelta;

	if (0.f >= m_fResetTime)
	{
		m_fResetTime = 0.05f;

		_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

		for (_int iIndex = 0; iIndex < iInstanceCount; ++iIndex)
			m_pInstanceBuffer[iIndex].vTextureUV = Get_TexUV_Rand(8, 8);

		if (10 < m_iEffectCount)
		{
			if (9 > m_iArrCount)
			{
				_int iZeroSize_Index = m_iZeroSize_Arr[m_iArrCount++];
				m_pInstanceBuffer[iZeroSize_Index].vSize = { 0.f, 0.f };
				m_pInstanceBuffer[iZeroSize_Index + 9].vSize = { 0.f, 0.f };
				m_pInstanceBuffer[iZeroSize_Index + 18].vSize = { 0.f, 0.f };
			}
		}

		++m_iEffectCount;
	}
}

_vector CEffect_UFO_Inside_ElectricWall_Spark::Get_RandDir(_int3 iRandPower, _vector vDir)
{
	_vector vRandDir = XMLoadFloat3(&Get_Dir_Rand(iRandPower));
	vRandDir += vDir;

	vRandDir = XMVector3Normalize(vRandDir);
	vRandDir.m128_f32[3] = 0.f;

	return vRandDir;
}

CEffect_UFO_Inside_ElectricWall_Spark * CEffect_UFO_Inside_ElectricWall_Spark::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_UFO_Inside_ElectricWall_Spark*	pInstance = new CEffect_UFO_Inside_ElectricWall_Spark(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_UFO_Inside_ElectricWall_Spark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_UFO_Inside_ElectricWall_Spark::Clone_GameObject(void * pArg)
{
	CEffect_UFO_Inside_ElectricWall_Spark* pInstance = new CEffect_UFO_Inside_ElectricWall_Spark(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_UFO_Inside_ElectricWall_Spark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_UFO_Inside_ElectricWall_Spark::Free()
{
	__super::Free();
}
