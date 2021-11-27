#include "stdafx.h"
#include "..\Public\Effect_Cody_Size.h"

CEffect_Cody_Size::CEffect_Cody_Size(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Cody_Size::CEffect_Cody_Size(const CEffect_Cody_Size & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Cody_Size::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };
	m_EffectDesc_Prototype.iInstanceCount = m_iInstance_Large;
	m_EffectDesc_Prototype.fLifeTime = 0.f;

	return S_OK;
}

HRESULT CEffect_Cody_Size::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);

	return S_OK;
}

_int CEffect_Cody_Size::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return NO_EVENT;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	Check_Size(TimeDelta);

	return NO_EVENT;
}

_int CEffect_Cody_Size::Late_Tick(_double TimeDelta)
{
	if(0.f >= m_EffectDesc_Prototype.fLifeTime)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Cody_Size::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_int iInstanceCount = Get_InstanceCount();
	_float4 vColor = { 0.930784376f, 0.930784376f, 0.930784376f, 1.000000000f };//= { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, iInstanceCount);

	return S_OK;
}

void CEffect_Cody_Size::Set_Model(CModel * pModel)
{
	m_pTargetModel = pModel;
	Safe_AddRef(m_pTargetModel);

	Ready_Instance();
}

void CEffect_Cody_Size::Update_Matrix(_fmatrix WorldMatrix)
{
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CEffect_Cody_Size::Change_Size(SIZE_TYPE eChangeSize)
{
	if (m_eChangeSize_Now == eChangeSize)
		return;

	m_IsSetDir = true;
	m_IsReSizing = true;
	m_IsNextSizing = false;
	m_dParabolaTime = 0.0;
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_eChangeSize_Now = eChangeSize;

	Reset_Instance();
}

void CEffect_Cody_Size::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Size::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Size::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_Cody_Size::Check_Size(_double TimeDelta)
{
	switch (m_eChangeSize_Now)
	{
	case Client::CEffect_Cody_Size::TYPE_SMALL_MIDDLE:
		Resizing_SM(TimeDelta);
		Resizing_SM_End(TimeDelta);
		break;
	case Client::CEffect_Cody_Size::TYPE_MIDDLE_SMALL:
		Resizing_MS(TimeDelta);
		Resizing_MS_End(TimeDelta);
		break;
	case Client::CEffect_Cody_Size::TYPE_MIDDLE_LARGE:
		Resizing_ML(TimeDelta);
		Resizing_ML_End(TimeDelta);
		break;
	case Client::CEffect_Cody_Size::TYPE_LARGE_MIDDLE:
		Resizing_LM(TimeDelta);
		Resizing_LM_End(TimeDelta);
		break;
	default:
		m_IsReSizing = false;
		break;
	}

	if(false == m_IsReSizing)
		m_eChangeSize_Now = CEffect_Cody_Size::TYPE_END;
}

void CEffect_Cody_Size::Resizing_MS(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (0.201f >= fSize)
	{
		m_IsNextSizing = true;
		return;
	}

	for (_int iIndex = 0; iIndex < m_iInstance_Small; ++iIndex)
	{
		_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos_Origin[iIndex]);
		vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);

		_float fPointSize = m_pInstance_SizeType[iIndex] * fSize;
		m_pInstanceBuffer[iIndex].vSize = _float2(fPointSize, fPointSize);		
	}
}
void CEffect_Cody_Size::Resizing_LM(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (1.005f >= fSize)
	{
		m_IsNextSizing = true;
		return;
	}

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
		vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);
		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);
		//m_pParabola_WorldPos_Y[iIndex] = m_pInstanceBuffer[iIndex].vPosition.y;

		_float fPointSize = m_pInstance_SizeType[iIndex] * fSize;
		m_pInstanceBuffer[iIndex].vSize = _float2(fPointSize, fPointSize);
	}
}
void CEffect_Cody_Size::Resizing_SM(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (0.9f <= fSize)
	{
		m_IsNextSizing = true;
		return;
	}

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
		vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);

		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);
		m_pParabola_WorldPos_Y[iIndex] = m_pInstanceBuffer[iIndex].vPosition.y;

		_float fPointSize = m_pInstance_SizeType[iIndex] * fSize;
		m_pInstanceBuffer[iIndex].vSize = _float2(fPointSize, fPointSize);
	}
}
void CEffect_Cody_Size::Resizing_ML(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (4.5f <= fSize)
	{
		m_IsNextSizing = true;
		return;
	}

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
		vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);

		XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);
		m_pParabola_WorldPos_Y[iIndex] = m_pInstanceBuffer[iIndex].vPosition.y;

		_float fPointSize = m_pInstance_SizeType[iIndex] * fSize;
		m_pInstanceBuffer[iIndex].vSize = _float2(fPointSize, fPointSize);
	}
}

void CEffect_Cody_Size::Resizing_MS_End(_double TimeDelta)
{
	if (false == m_IsNextSizing)
		return;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

		if (0 != XMVector3Length(vDir).m128_f32[0])
		{
			_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos_Origin[iIndex]);
			vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);

			_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
			_vector vDir_D = vWorldPos - vPos;
			vPos += vDir_D * 5.f * (_float)TimeDelta;
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);

			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.03125f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}

		else
		{
			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.0625f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);

			_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
			vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);
		}
	}

	m_dParabolaTime += 0.02;
}
void CEffect_Cody_Size::Resizing_LM_End(_double TimeDelta)
{
	if (false == m_IsNextSizing)
		return;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize		= m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

		if (0 != XMVector3Length(vDir).m128_f32[0])
		{
			_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos_Origin[iIndex]);
			vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);
			
			_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
			_vector vDir_D = vWorldPos - vPos;
			vPos += vDir_D * 5.f * (_float)TimeDelta;
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);

			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.1f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}

		else
		{
			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.25f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);

			_vector vWorldPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
			vWorldPos = XMVector3Transform(vWorldPos, WorldMatrix);
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vWorldPos);
		}
	}

	m_dParabolaTime += 0.02;
}
void CEffect_Cody_Size::Resizing_SM_End(_double TimeDelta)
{
	if (false == m_IsNextSizing)
		return;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

		if (0 != XMVector3Length(vDir).m128_f32[0])
		{
			_float fParabola = m_pParabola_WorldPos_Y[iIndex];
			_float fPower = (vDir.m128_f32[1] + 0.125f);
			fParabola += _float((4.f * fPower) * m_dParabolaTime * 1.f - 0.5f * (GRAVITY * m_dParabolaTime * m_dParabolaTime));
			m_pInstanceBuffer[iIndex].vPosition.y = fParabola;

			_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
			vDir.m128_f32[1] = 0.f;
			XMVector3Normalize(vDir);
			vPos += vDir * 1.5f * (_float)TimeDelta;
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);

			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.0625f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}

		else
		{
			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.125f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}
	}

	m_dParabolaTime += 0.02;
}
void CEffect_Cody_Size::Resizing_ML_End(_double TimeDelta)
{
	if (false == m_IsNextSizing)
		return;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	for (_int iIndex = 0; iIndex < m_iInstance_Large; ++iIndex)
	{
		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);

		if (0 != XMVector3Length(vDir).m128_f32[0])
		{
			_float fParabola = m_pParabola_WorldPos_Y[iIndex];
			_float fPower = (vDir.m128_f32[1] + 0.5f);
			fParabola += _float((8.f * fPower) * m_dParabolaTime * 1.f - 0.5f * (GRAVITY * m_dParabolaTime * m_dParabolaTime));
			m_pInstanceBuffer[iIndex].vPosition.y = fParabola;

			_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
			vDir.m128_f32[1] = 0.f;
			XMVector3Normalize(vDir);
			vPos += vDir * 5.f * (_float)TimeDelta;
			XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);

			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.25f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}

		else
		{
			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.5f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}
	}

	m_dParabolaTime += 0.02;
}

_int CEffect_Cody_Size::Get_InstanceCount()
{
	switch (m_eChangeSize_Now)
	{
	case Client::CEffect_Cody_Size::TYPE_SMALL_MIDDLE:
		return m_iInstance_Middle;
	case Client::CEffect_Cody_Size::TYPE_MIDDLE_SMALL:
		return m_iInstance_Small;
	case Client::CEffect_Cody_Size::TYPE_MIDDLE_LARGE:
		return m_iInstance_Large;
	case Client::CEffect_Cody_Size::TYPE_LARGE_MIDDLE:
		return m_iInstance_Middle;
	default:
		break;
	}
	return 0;
}

void CEffect_Cody_Size::Reset_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return;

	_int iInstanceCount = Get_InstanceCount();

	VTXMESH*	pVtx		= m_pTargetModel->Get_Vertices();
	_uint		iVtxCount	= m_pTargetModel->Get_VertexCount();
	_uint		iRandVtx	= rand() % iInstanceCount;
	_uint		iAddVtx		= _int(iVtxCount / (_float)iInstanceCount);

	_float fIndecesRatio	= 0.6f * iInstanceCount;
	_float fIndecesRatio2	= 0.4f * iInstanceCount;

	_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	_double fRenderTerm = 0.f;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix	PivotMatrix = XMMatrixScaling(0.0105f, 0.0105f, 0.0105f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		_int iRandSize = rand() % 3;
		if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.04f, 0.04f);
		else if (1 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.0525f, 0.0525f);
		else
			m_pInstanceBuffer[i].vSize = _float2(0.065f, 0.065f);

		m_pInstance_SizeType[i] = m_pInstanceBuffer[i].vSize.x;

		_vector vLocalPos = XMLoadFloat3(&pVtx[iRandVtx].vPosition);
		_vector vPivotPos = XMVector3Transform(vLocalPos, PivotMatrix);

		m_pInstance_Dir[i] = { 0.f, 0.f, 0.f };

		if (rand() % 5 == 0)
		{
			_vector vDir = XMLoadFloat3(&pVtx[iRandVtx].vNormal);
			vDir = XMVector3Transform(vDir, XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f)));
			vDir = XMVector3Normalize(vDir);
			vDir.m128_f32[3] = 0.f;

			if (TYPE_MIDDLE_LARGE == m_eChangeSize_Now)
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);			

			else if (TYPE_LARGE_MIDDLE == m_eChangeSize_Now)
			{
				XMStoreFloat4(&m_pInstance_LocalPos_Origin[i], vPivotPos);

				_float fPower = pVtx[iRandVtx].vNormal.y + 0.2f;
				if (0.f >= fPower)
					fPower *= -1.f;
				_float fHeight = XMVectorGetY(vPivotPos);

				_float fRandTime = _float(rand() % 3 + 1) * 0.1f;
				fHeight += _float((8.f * fPower * fRandTime) - 0.5f * (GRAVITY * fRandTime * fRandTime));
				vPivotPos = XMVectorSetY(vPivotPos, fHeight);

				_vector vDir_XZ = vDir;
				_float fRandPower = _float(rand() % 5 + 8) * 0.1f;
				vDir_XZ.m128_f32[1] = 0.f;
				if(rand() % 2 == 0)
					vPivotPos += vDir_XZ * fRandPower;
				else
					vPivotPos -= vDir_XZ * fRandPower;

				vDir *= -1.f;
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);
			}

			else if(TYPE_SMALL_MIDDLE == m_eChangeSize_Now)
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);

			else if (TYPE_MIDDLE_SMALL == m_eChangeSize_Now)
			{
				XMStoreFloat4(&m_pInstance_LocalPos_Origin[i], vPivotPos);

				_float fPower = pVtx[iRandVtx].vNormal.y + 0.2f;
				if (0.f >= fPower)
					fPower *= -1.f;
				_float fHeight = XMVectorGetY(vPivotPos);

				_float fRandTime = _float(rand() % 3 + 1) * 0.1f;
				fHeight += _float((8.f * fPower * fRandTime) - 0.5f * (GRAVITY * fRandTime * fRandTime));
				vPivotPos = XMVectorSetY(vPivotPos, fHeight);

				_vector vDir_XZ = vDir;
				_float fRandPower = _float(rand() % 5 + 8) * 0.1f;
				vDir_XZ.m128_f32[1] = 0.f;
				if (rand() % 2 == 0)
					vPivotPos += vDir_XZ * fRandPower;
				else
					vPivotPos -= vDir_XZ * fRandPower;

				vDir *= -1.f;
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);
			}

		}

		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		XMStoreFloat4(&m_pInstance_LocalPos[i], vPivotPos);

		iRandVtx += iAddVtx;
		if (iRandVtx >= iVtxCount)
			iRandVtx = rand() % iInstanceCount;
	}
}

HRESULT CEffect_Cody_Size::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer				= new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstance_Dir					= new _float3[iInstanceCount];
	m_pInstance_LocalPos			= new _float4[iInstanceCount];
	m_pInstance_LocalPos_Origin		= new _float4[iInstanceCount];
	m_pParabola_WorldPos_Y			= new _float[iInstanceCount];
	m_pInstance_SizeType			= new _float[iInstanceCount];

	VTXMESH* pVtx = m_pTargetModel->Get_Vertices();
	_uint iVtxCount = m_pTargetModel->Get_VertexCount();
	_uint iRandVtx = rand() % iInstanceCount;
	_uint iAddVtx = _int(iVtxCount / (_float)iInstanceCount);

	_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };
	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	_float fIndecesRatio = 0.6f * iInstanceCount;
	_float fIndecesRatio2 = 0.4f * iInstanceCount;

	_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fDis = 0.5f * fScaleX;

	_double fRenderTerm = 0.f;
	m_UVTime = m_EffectDesc_Clone.UVTime;

	_vector vPos = XMVectorZero();
	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));

	memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		_int iRandSize = rand() % 3;
		if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.05f, 0.05f);
		else if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.0625f, 0.0625f);
		else
			m_pInstanceBuffer[i].vSize = _float2(0.075f, 0.075f);

		m_pInstance_SizeType[i] = m_pInstanceBuffer[i].vSize.x;

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

// 		_vector vWorldDir = XMVector3Normalize((vLocalPos - vPos));
// 		_vector vDir = XMVector3Cross(vWorldDir, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		XMStoreFloat4(&m_pInstance_LocalPos[i], vLocalPos);
// 		XMStoreFloat3(&m_pInstance_Dir[i], vDir);

		iRandVtx += iAddVtx;
		if (iRandVtx >= iVtxCount)
			iRandVtx = rand() % iInstanceCount;
	}


	return S_OK;
}

CEffect_Cody_Size * CEffect_Cody_Size::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Cody_Size*	pInstance = new CEffect_Cody_Size(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Cody_Size");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Cody_Size::Clone_GameObject(void * pArg)
{
	CEffect_Cody_Size* pInstance = new CEffect_Cody_Size(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Cody_Size");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Cody_Size::Free()
{
	Safe_Release(m_pTargetModel);

	Safe_Delete_Array(m_pInstance_LocalPos);
	Safe_Delete_Array(m_pInstance_LocalPos_Origin);
	Safe_Delete_Array(m_pParabola_WorldPos_Y);
	Safe_Delete_Array(m_pInstance_SizeType);

	__super::Free();
}
