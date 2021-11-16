#include "stdafx.h"
#include "..\Public\Effect_Player_Dead_Particle.h"
#include "GameInstance.h"

CEffect_Player_Dead_Particle::CEffect_Player_Dead_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Dead_Particle::CEffect_Player_Dead_Particle(const CEffect_Player_Dead_Particle & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Player_Dead_Particle::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);


	return S_OK;
}

HRESULT CEffect_Player_Dead_Particle::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };

	__super::Ready_Component(pArg);

	if(EFFECT_DESC_CLONE::PV_CODY >= m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 1000;
	else if(EFFECT_DESC_CLONE::PV_CODY_S == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 100;
	else if (EFFECT_DESC_CLONE::PV_CODY_L == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 5000;

	m_EffectDesc_Clone.UVTime = 0.01;
	m_EffectDesc_Clone.vRandDirPower = { 10.f,10.f,10.f };

	Ready_Instance();

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Dead_Cells"), TEXT("Com_Textrue_Particle"), (CComponent**)&m_pTexturesCom_Particle), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"), TEXT("Com_Textrue_Particle_Mask"), (CComponent**)&m_pTexturesCom_Particle_Mask), E_FAIL);

	return S_OK;
}

_int CEffect_Player_Dead_Particle::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	m_pInstanceBuffer[0].vTextureUV = Check_UV((_float)TimeDelta, 0, false);

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}

	return _int();
}

_int CEffect_Player_Dead_Particle::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_Player_Dead_Particle::Render()
{
	SetUp_Shader_Data();

	_float4 vUV = { 0.f,0.f,1.f,1.f };
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vUV, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Particle_Mask->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Particle->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(4, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);


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

	m_pTargetModel = static_cast<CModel*>(m_EffectDesc_Clone.pArg);
	Safe_AddRef(m_pTargetModel);
	VTXMESH* pVtx = m_pTargetModel->Get_Vertices();
	_uint iVtxCount = m_pTargetModel->Get_VertexCount();
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


	Safe_Release(m_pTargetModel);

	return S_OK;
}

_float4 CEffect_Player_Dead_Particle::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft	= (1.f / U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop		= (1.f / V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight	= (1.f / U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom	= (1.f / V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

void CEffect_Player_Dead_Particle::Set_VtxColor(_int iIndex, _uint iVtxIndex)
{
	if (EFFECT_DESC_CLONE::PV_MAY == m_EffectDesc_Clone.iPlayerValue)
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
	else
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

	m_pInstanceBuffer[iIndex].vTextureUV = Set_particleUV(iIndex, 4, 4);
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
	Safe_Release(m_pTexturesCom_Particle);
	Safe_Release(m_pTexturesCom_Particle_Mask);

	__super::Free();
}

