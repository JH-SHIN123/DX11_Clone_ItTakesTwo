#include "stdafx.h"
#include "..\Public\Effect_CS_CodySize.h"
#include "DataStorage.h"
#include "Cody.h"

CEffect_CS_CodySize::CEffect_CS_CodySize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_CS_CodySize::CEffect_CS_CodySize(const CEffect_CS_CodySize & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_CS_CodySize::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.vSize = { 0.09f, 0.09f,0.f };
	m_EffectDesc_Prototype.iInstanceCount = m_iInstance_Large;

	lstrcpy(m_EffectDesc_Prototype.TextureName, TEXT("Component_Texture_Circle_Alpha"));

	return S_OK;
}

HRESULT CEffect_CS_CodySize::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);

	Check_Size();
	Set_Model();
	Update_Matrix();
	Ready_Instance();
	Reset_Instance();
	
	return S_OK;
}

_int CEffect_CS_CodySize::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	Update_Matrix();
	Check_Size(TimeDelta);

	return NO_EVENT;
}

_int CEffect_CS_CodySize::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_CS_CodySize::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_int iInstanceCount = Get_InstanceCount();
	_float4 vColor = { 0.930784376f, 0.930784376f, 0.930784376f, 1.000000000f };//= { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
	m_pPointInstanceCom->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pPointInstanceCom->Render(1, m_pInstanceBuffer, iInstanceCount);

	return S_OK;
}

void CEffect_CS_CodySize::Set_Model()
{
	m_pTargetModel = static_cast<CCody*>(DATABASE->GetCody())->Get_Model();
	Safe_AddRef(m_pTargetModel);
}

void CEffect_CS_CodySize::Update_Matrix()
{
	_matrix WorldMatrix = static_cast<CCody*>(DATABASE->GetCody())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CCody*>(DATABASE->GetCody())->Get_Model()->Get_BoneMatrix("Hips");

	_vector vPos = WorldMatrix.r[3];

	BoneMatrix = BoneMatrix * WorldMatrix;

	for (_int i = 0; i < 3; ++i)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]) * XMVector3Length(WorldMatrix.r[i]);

	if (1 == m_EffectDesc_Clone.iCutSceneTake)
		BoneMatrix.r[3].m128_f32[1] = 0.7f;

	if (3 == m_EffectDesc_Clone.iCutSceneTake)
	{
		BoneMatrix.r[3].m128_f32[1] = 0.7f;
		BoneMatrix.r[3].m128_f32[0] -= 2.f;
		BoneMatrix.r[3].m128_f32[2] += 1.f;
	}

	m_pTransformCom->Set_WorldMatrix(BoneMatrix);
}

void CEffect_CS_CodySize::Check_Size()
{
	if(1 == m_EffectDesc_Clone.iCutSceneTake)
		m_eChangeSize_Now = CEffect_CS_CodySize::TYPE_MIDDLE_LARGE;
	else if (2 == m_EffectDesc_Clone.iCutSceneTake)
		m_eChangeSize_Now = CEffect_CS_CodySize::TYPE_LARGE_SMALL;
	else if (3 == m_EffectDesc_Clone.iCutSceneTake)
		m_eChangeSize_Now = CEffect_CS_CodySize::TYPE_SMALL_LARGE;
	else if (4 == m_EffectDesc_Clone.iCutSceneTake)
		m_eChangeSize_Now = CEffect_CS_CodySize::TYPE_SMALL_MIDDLE;


	m_IsSetDir = true;
	m_IsReSizing = true;
	m_IsNextSizing = false;
	m_dParabolaTime = 0.0;
	m_EffectDesc_Prototype.fLifeTime = 2.f;
}

void CEffect_CS_CodySize::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_CodySize::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_CodySize::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_CodySize::Check_Size(_double TimeDelta)
{
	switch (m_eChangeSize_Now)
	{
	case Client::CEffect_CS_CodySize::TYPE_SMALL_MIDDLE:
		Resizing_SM(TimeDelta);
		Resizing_SM_End(TimeDelta);
		break;
	case Client::CEffect_CS_CodySize::TYPE_LARGE_SMALL:
		Resizing_LS(TimeDelta);
		Resizing_LS_End(TimeDelta);
		break;
	case Client::CEffect_CS_CodySize::TYPE_SMALL_LARGE:
		Resizing_SL(TimeDelta);
		Resizing_SL_End(TimeDelta);
		break;
	case Client::CEffect_CS_CodySize::TYPE_MIDDLE_LARGE:
		Resizing_ML(TimeDelta);
		Resizing_ML_End(TimeDelta);
		break;
	default:
		m_IsReSizing = false;
		break;
	}

	if (false == m_IsReSizing)
		m_eChangeSize_Now = CEffect_CS_CodySize::TYPE_END;
}

void CEffect_CS_CodySize::Resizing_LS(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (0.105f >= fSize)
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
void CEffect_CS_CodySize::Resizing_SL(_double TimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fSize = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	if (4.8f <= fSize)
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
void CEffect_CS_CodySize::Resizing_SM(_double TimeDelta)
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
void CEffect_CS_CodySize::Resizing_ML(_double TimeDelta)
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

void CEffect_CS_CodySize::Resizing_LS_End(_double TimeDelta)
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

			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.0625f;
			m_pInstanceBuffer[iIndex].vSize.y = m_pInstanceBuffer[iIndex].vSize.x;
			if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
				m_pInstanceBuffer[iIndex].vSize = _float2(0.f, 0.f);
		}

		else
		{
			m_pInstanceBuffer[iIndex].vSize.x -= (_float)TimeDelta * 0.09f;
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
void CEffect_CS_CodySize::Resizing_SL_End(_double TimeDelta)
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
			fParabola += _float((10.f * fPower) * m_dParabolaTime * 1.f - 0.5f * (GRAVITY * m_dParabolaTime * m_dParabolaTime));
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
void CEffect_CS_CodySize::Resizing_SM_End(_double TimeDelta)
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
void CEffect_CS_CodySize::Resizing_ML_End(_double TimeDelta)
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

_int CEffect_CS_CodySize::Get_InstanceCount()
{
	switch (m_eChangeSize_Now)
	{
	case Client::CEffect_CS_CodySize::TYPE_SMALL_MIDDLE:
		return m_iInstance_Middle;
	case Client::CEffect_CS_CodySize::TYPE_LARGE_SMALL:
		return m_iInstance_Middle;
	case Client::CEffect_CS_CodySize::TYPE_MIDDLE_LARGE:
		return m_iInstance_Large;
	case Client::CEffect_CS_CodySize::TYPE_SMALL_LARGE:
		return m_iInstance_Large;
	default:
		break;
	}
	return 0;
}

void CEffect_CS_CodySize::Reset_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return;

	_int iInstanceCount = Get_InstanceCount();

	VTXMESH*	pVtx = m_pTargetModel->Get_Vertices();
	_uint		iVtxCount = m_pTargetModel->Get_VertexCount();
	_uint		iRandVtx = rand() % iInstanceCount;
	_uint		iAddVtx = _int(iVtxCount / (_float)iInstanceCount);

	_float fIndecesRatio = 0.6f * iInstanceCount;
	_float fIndecesRatio2 = 0.4f * iInstanceCount;

	_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	_double fRenderTerm = 0.f;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix	PivotMatrix = XMMatrixScaling(0.0105f, 0.0105f, 0.0105f);
	//PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));

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

			else if (TYPE_SMALL_MIDDLE == m_eChangeSize_Now)
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);

			else if (TYPE_SMALL_LARGE == m_eChangeSize_Now)
				XMStoreFloat3(&m_pInstance_Dir[i], vDir);

			else if (TYPE_LARGE_SMALL == m_eChangeSize_Now)
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

HRESULT CEffect_CS_CodySize::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstance_Dir = new _float3[iInstanceCount];
	m_pInstance_LocalPos = new _float4[iInstanceCount];
	m_pInstance_LocalPos_Origin = new _float4[iInstanceCount];
	m_pParabola_WorldPos_Y = new _float[iInstanceCount];
	m_pInstance_SizeType = new _float[iInstanceCount];

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

CEffect_CS_CodySize * CEffect_CS_CodySize::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_CS_CodySize*	pInstance = new CEffect_CS_CodySize(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_CS_CodySize");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_CS_CodySize::Clone_GameObject(void * pArg)
{
	CEffect_CS_CodySize* pInstance = new CEffect_CS_CodySize(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_CS_CodySize");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_CS_CodySize::Free()
{
	Safe_Release(m_pTargetModel);

	Safe_Delete_Array(m_pInstance_LocalPos);
	Safe_Delete_Array(m_pInstance_LocalPos_Origin);
	Safe_Delete_Array(m_pParabola_WorldPos_Y);
	Safe_Delete_Array(m_pInstance_SizeType);

	__super::Free();
}
