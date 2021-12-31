#include "stdafx.h"
#include "..\Public\Effect_Player_Dead_Particle.h"

CEffect_Player_Dead_Particle::CEffect_Player_Dead_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Dead_Particle::CEffect_Player_Dead_Particle(const CEffect_Player_Dead_Particle & rhs)
	: CInGameEffect(rhs), m_vTargetPos(rhs.m_vTargetPos)
{
}

HRESULT CEffect_Player_Dead_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_vTargetPos = { 0.f, 0.f, 0.f, 1.f };

	return S_OK;
}

HRESULT CEffect_Player_Dead_Particle::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 5.f;
	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };

	__super::Ready_Component(pArg);

	if (EFFECT_DESC_CLONE::PV_CODY >= m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 1000;
	else if (EFFECT_DESC_CLONE::PV_CODY_S == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 100;
	else if (EFFECT_DESC_CLONE::PV_CODY_L == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 5000;


	//Ready_Instance();
// 	memcpy(m_pModelCom, static_cast<CModel*>(m_EffectDesc_Clone.pArg), sizeof(CModel*));
	m_pModelCom = static_cast<CModel*>(m_EffectDesc_Clone.pArg);
	Safe_AddRef(m_pModelCom);

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody_Effect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	// 	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Dead_Cells"), TEXT("Com_Texture_Particle"), (CComponent**)&m_pTexturesCom_Particle), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"), TEXT("Com_Texture_Particle_Mask"), (CComponent**)&m_pTexturesCom_Particle_Mask), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Texture_Particle_Diss"), (CComponent**)&m_pTexturesCom_Particle_Diss), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Twirl"), TEXT("Com_Texture_Particle_Flow"), (CComponent**)&m_pTexturesCom_Particle_Flow), E_FAIL);

	//	m_pModelCom->Set_Animation(ANI_M_Death_Fall_MH);
	//	m_pModelCom->Set_NextAnimIndex(ANI_M_Death_Fall_MH);
	SetUp_Rand_Dir();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vPos = WorldMatrix.r[3];
	vPos += XMVector3Normalize(WorldMatrix.r[1]) * (XMVector3Length(WorldMatrix.r[1]));
	//vPos += XMVector3Normalize(WorldMatrix.r[2]) * (XMVector3Length(WorldMatrix.r[2]) * 1.5f);
	XMStoreFloat4(&m_vTargetPos, vPos);


// 	m_fSizePower = XMVector3Length(WorldMatrix.r[0]).m128_f32[0];
// 	m_vSize.x *= m_fSizePower;
// 	m_vSize.y *= m_fSizePower;

	return S_OK;
}

_int CEffect_Player_Dead_Particle::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	m_fMoveTime += (_float)TimeDelta * 0.75f;

// 	m_vSize.x -= (_float)TimeDelta * 0.01f * m_fSizePower;
// 	m_vSize.y -= (_float)TimeDelta * 0.01f * m_fSizePower;
// 	if (0.f >= m_vSize.x)
// 	{
// 		m_vSize.x = 0.f;
// 		m_vSize.y = 0.f;
// 	}

	return _int();
}

_int CEffect_Player_Dead_Particle::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}

HRESULT CEffect_Player_Dead_Particle::Render(RENDER_GROUP::Enum eGroup)
{
	/*
	g_DissolveTexture
	g_FlowTexture
	g_fTime
	g_fDissolveTime
	g_fRadius
	g_vTextureSize
	*/
	XMINT2 vTexSize = {1024, 1024};
	_float fFlowPower = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
	_float fRadius = 0.1f * fFlowPower;
	m_pModelCom->Set_Variable("g_vPos", &m_vTargetPos, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fMoveTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_fRadius", &fRadius, sizeof(_float));
	m_pModelCom->Set_Variable("g_fDissolveTime", &m_fMoveTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_vTextureSize", &vTexSize, sizeof(XMINT2));
	vTexSize = { 512, 512 };
	m_pModelCom->Set_Variable("g_vTextureSize_2", &vTexSize, sizeof(XMINT2));
	m_pModelCom->Set_Variable("g_fFlowPower", &fFlowPower, sizeof(_float));

	m_pModelCom->Set_ShaderResourceView("g_DissolveTexture", m_pTexturesCom_Particle_Diss->Get_ShaderResourceView(1));
	m_pModelCom->Set_ShaderResourceView("g_FlowTexture",	 m_pTexturesCom_Particle_Flow->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_MaskingTexture",  m_pTexturesCom_Particle_Mask->Get_ShaderResourceView(0));

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(11);

	return S_OK;
}
void CEffect_Player_Dead_Particle::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x -= TimeDelta * 0.05f;
	m_pInstanceBuffer[iIndex].vSize.y -= TimeDelta * 0.05f;

	if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
	{
		m_pInstanceBuffer[iIndex].vSize.x = 0.f;
		m_pInstanceBuffer[iIndex].vSize.y = 0.f;
	}
}

void CEffect_Player_Dead_Particle::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);

	vPos += vDir * TimeDelta * 0.125f;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Player_Dead_Particle::Instance_UV(_float TimeDelta, _int iIndex)
{

}

HRESULT CEffect_Player_Dead_Particle::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pModelCom = static_cast<CModel*>(m_EffectDesc_Clone.pArg);
	Safe_AddRef(m_pModelCom);
	VTXMESH* pVtx = m_pModelCom->Get_Vertices();
	_uint iVtxCount = m_pModelCom->Get_VertexCount();
	_uint iRandVtx = rand() % iInstanceCount;
	_uint iAddVtx = _int(iVtxCount / (_float)iInstanceCount);

	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));


	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];

	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_UVCount = new _float2[iInstanceCount];

	_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };

	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	_double fRenderTerm = 0.f;
	m_UVTime = m_EffectDesc_Clone.UVTime;

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_int iRandSize = rand() % 3;
		if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.05f, 0.05f);
		else if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.0625f, 0.0625f);
		else
			m_pInstanceBuffer[i].vSize = _float2(0.075f, 0.075f);

		_vector vRandDir = XMVectorZero();

		_int iRandPower[3] = { (_int)m_EffectDesc_Clone.vRandDirPower.x , (_int)m_EffectDesc_Clone.vRandDirPower.y, (_int)m_EffectDesc_Clone.vRandDirPower.z };
		_int iDir[3] = { 0, 0, 0 };

		for (_int j = 0; j < 3; ++j)
		{
			if (0 != iRandPower[j])
			{
				iDir[j] = rand() % (iRandPower[j] + 1);
				if (true == m_EffectDesc_Clone.IsRandDirDown[j] && rand() % 2 == 0)
					iDir[j] *= -1;
			}

			vRandDir.m128_f32[j] = (_float)iDir[j];
		}

		vRandDir = XMVector3Normalize(vRandDir);

		_vector vLocalPos = XMLoadFloat3(&pVtx[iRandVtx].vPosition);
		vLocalPos = XMVector3Transform(vLocalPos, PivotMatrix);
		vLocalPos = XMVector3Transform(vLocalPos, XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix));
		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vLocalPos);
		XMStoreFloat3(&m_pInstance_Dir[i], vRandDir);

		iRandVtx += iAddVtx;
		if (iRandVtx >= iVtxCount)
			iRandVtx = rand() % iInstanceCount;

		Set_VtxColor(i, iRandVtx);
	}

	Safe_Release(m_pModelCom);

	return S_OK;
}

_float4 CEffect_Player_Dead_Particle::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft = (1.f / U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop = (1.f / V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight = (1.f / U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom = (1.f / V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

void CEffect_Player_Dead_Particle::Set_VtxColor(_int iIndex, _uint iVtxIndex)
{
	if (EFFECT_DESC_CLONE::PV_MAY == m_EffectDesc_Clone.iPlayerValue)
	{
		if (0 <= iVtxIndex && iVtxIndex <= 92670) // ¹åÁÙ, ÆÈ¶Ò
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (92670 <= iVtxIndex && iVtxIndex <= 135426) // ½º¿þÅÍ
			m_pInstance_UVCount[iIndex] = { 1.f, 0.f };
		else if (135426 <= iVtxIndex && iVtxIndex <= 140226) // ´«±ò
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (140226 <= iVtxIndex && iVtxIndex <= 177645) // ´ë°¥Åë
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (177645 <= iVtxIndex && iVtxIndex <= 291639) // ¸Ó¸®ÅÐ, Àå½Ä
			m_pInstance_UVCount[iIndex] = { 0.f, 0.f };
		else if (291639 <= iVtxIndex && iVtxIndex <= 347628) // ¹ÙÁö, ½Å¹ß
			m_pInstance_UVCount[iIndex] = { 2.f, 0.f };
		else
			m_pInstance_UVCount[iIndex] = { 1.f, 0.f };
	}
	else
	{
		if (0 <= iVtxIndex && iVtxIndex <= 41748) // ½º¿þÅÍ
			m_pInstance_UVCount[iIndex] = { 3.f, 2.f };
		else if (54924 <= iVtxIndex && iVtxIndex <= 96558) // ¸Ó¸®ÅÐ
			m_pInstance_UVCount[iIndex] = { 0.f, 2.f };
		else if (177171 <= iVtxIndex && iVtxIndex <= 209500) // ¹ÙÁö
			m_pInstance_UVCount[iIndex] = { 2.f, 2.f };
		else if (209500 <= iVtxIndex && iVtxIndex <= 259779) // ÆÈ¶Ò
			m_pInstance_UVCount[iIndex] = { 1.f, 2.f };
		else if (259779 <= iVtxIndex && iVtxIndex <= 301083) // ¶Ç´Ù¸¥ ¸Ó¸®ÅÐ
			m_pInstance_UVCount[iIndex] = { 0.f, 2.f };
		else
			m_pInstance_UVCount[iIndex] = { 1.f, 2.f };
	}

	m_pInstanceBuffer[iIndex].vTextureUV = Set_particleUV(iIndex, 4, 4);
}

void CEffect_Player_Dead_Particle::SetUp_Rand_Dir()
{
// 	for (_int i = 0; i < 256; ++i)
// 	{
// 		m_vDir_Array[i] = Get_Dir_Rand(_int3(100, 100, 100));
// 		_vector vDir = XMLoadFloat3(&m_vDir_Array[i]) /** (_float(rand() % 100) * 0.0025f)*/;
// 		XMStoreFloat3(&m_vDir_Array[i], vDir);
// 	}
}

CEffect_Player_Dead_Particle * CEffect_Player_Dead_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Player_Dead_Particle*	pInstance = new CEffect_Player_Dead_Particle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Player_Dead_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Player_Dead_Particle::Clone_GameObject(void * pArg)
{
	CEffect_Player_Dead_Particle* pInstance = new CEffect_Player_Dead_Particle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Player_Dead_Particle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Player_Dead_Particle::Free()
{
	Safe_Release(m_pTexturesCom_Particle_Mask);
	Safe_Release(m_pTexturesCom_Particle_Diss);
	Safe_Release(m_pTexturesCom_Particle_Flow);

	__super::Free();
}

