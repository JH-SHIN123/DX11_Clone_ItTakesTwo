#include "..\public\Shadow_Manager.h"
#include "GameInstance.h"
#include "Frustum.h"
#include "PipeLine.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CShadow_Manager);

void CShadow_Manager::Get_CascadeShadowLightViewProjTranspose(_matrix* OutMatrix) const
{
	for (_uint i = 0; i < MAX_CASCADES; ++i)
		OutMatrix[i] = XMMatrixTranspose(XMLoadFloat4x4(&m_CascadeViews[i]) * XMLoadFloat4x4(&m_CascadeProjs[i]));
}

void CShadow_Manager::Get_CascadeShadowTransformsTranspose(_matrix* OutMatrix) const
{
	for (_uint i = 0; i < MAX_CASCADES; ++i)
		OutMatrix[i] = XMMatrixTranspose(XMLoadFloat4x4(&m_CascadedShadowTransforms[i]));
}

HRESULT CShadow_Manager::Ready_ShadowManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context)
{
	m_pDevice = pDevice;
	m_pDevice_Context = pDevice_Context;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDevice_Context);

	FAILED_CHECK_RETURN(Set_CascadeViewportsInfo(), E_FAIL);

	m_pVIBuffer = CVIBuffer_RectRHW::Create(m_pDevice, m_pDevice_Context, 0.f, 0.f, 1000, 1000, TEXT("../Bin/ShaderFiles/Shader_LightAcc.hlsl"), "DefaultTechnique");
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);

	return S_OK;
}

void CShadow_Manager::Clear_Buffer()
{
	Safe_Release(m_pVIBuffer);
}

HRESULT CShadow_Manager::RSSet_CascadedViewports()
{
	m_pDevice_Context->RSSetViewports(MAX_CASCADES, m_CascadeViewport);

	return S_OK;
}

HRESULT CShadow_Manager::Set_CascadeViewportsInfo()
{
	for (_uint i = 0; i < MAX_CASCADES; ++i)
	{
		// width / height 해상도 ( LOD X )
		m_CascadeViewport[i].TopLeftX = 0.f;
		m_CascadeViewport[i].TopLeftY = (_float)(i * SHADOWMAP_SIZE);
		m_CascadeViewport[i].Width = SHADOWMAP_SIZE;
		m_CascadeViewport[i].Height = SHADOWMAP_SIZE;
		m_CascadeViewport[i].MinDepth = 0.f;
		m_CascadeViewport[i].MaxDepth = 1.f;
	}

	return S_OK;
}

HRESULT CShadow_Manager::Update_CascadeShadowTransform()
{
	// Get Directional Light - if light is not exit, return 0
	CLight_Manager* pLightManager = CLight_Manager::GetInstance();
	if (nullptr == pLightManager) return E_FAIL;
	CLight* pLight = pLightManager->Get_DirectionalLight();
	if (nullptr == pLight) return 0;

	LIGHT_DESC* lightDesc = pLight->Get_LightDescPtr();
	if (nullptr == lightDesc) return E_FAIL;
	_vector vDirectionalLightDir = XMLoadFloat3(&lightDesc->vDirection);

	CFrustum* pFrustum = CFrustum::GetInstance();
	if (nullptr == pFrustum) return E_FAIL;

	const _float3* pFrustumCornersW = pFrustum->Get_FrustumPointsInWorld(CFrustum::FRUSTUM_SHADOW);
	if (nullptr == pFrustumCornersW) return E_FAIL;

	_vector cascadeFrustumCornersW[8];
	ZeroMemory(cascadeFrustumCornersW, sizeof(_vector) * 8);

	//// Get Frustum Transform Offset
	//CPipeline* pPipeLine = CPipeline::GetInstance();
	//if (nullptr == pPipeLine) return E_FAIL;

	//_matrix CamWorldMat = XMMatrixIdentity();

	//if (0 == iViewportIndex)  /* Main View */
	//	CamWorldMat = pPipeLine->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	//else if (1 == iViewportIndex)
	//	CamWorldMat = pPipeLine->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);

	//_vector vCamLook = XMVector3Normalize(CamWorldMat.r[2]);

	//_float fFrustumTransformOffset = pPipeLine->Get_MainCamFar(); --> Main & Sub
	//fFrustumTransformOffset *= m_fCascadedEnds[0] * 0.5f;

	//_vector vCascadeFrustumOffset = -vCamLook * fFrustumTransformOffset;

	for (_uint currentCascade = 0; currentCascade < MAX_CASCADES; ++currentCascade)
	{
		// Set Cam ViewFrustum Corners in World
		for (_uint i = 0; i < 8; ++i)
		{
			// Move Offset Frustm Center Forward -z
			// camera look 방향으로 -z 만큼 이동
			cascadeFrustumCornersW[i] = XMLoadFloat3(&pFrustumCornersW[i]);
			//cascadeFrustumCornersW[i] = XMLoadFloat3(&pFrustumCornersW[i]) + vCascadeFrustumOffset;
		}

		// Slice Cascade ViewFrustum
		_vector vCornerRay, vNearCornerRay, vFarCornerRay;
		for (_uint i = 0; i < 4; ++i)
		{
			vCornerRay = cascadeFrustumCornersW[i + 4] - cascadeFrustumCornersW[i];
			vNearCornerRay = vCornerRay * m_fCascadedEnds[currentCascade];
			vFarCornerRay = vCornerRay * m_fCascadedEnds[currentCascade + 1];
			cascadeFrustumCornersW[i + 4] = cascadeFrustumCornersW[i] + vFarCornerRay;
			cascadeFrustumCornersW[i] = cascadeFrustumCornersW[i] + vNearCornerRay;
		}

		// Get Cadcade Frustum Center
		_vector cascadeFrustumCenter = XMVectorZero();
		for (_uint i = 0; i < 8; ++i) {
			cascadeFrustumCenter += cascadeFrustumCornersW[i];
		}
		cascadeFrustumCenter /= 8.0f;

		// Caculate AABB around the frustum conrners
		_float sphereRadius = 0.f;
		for (_uint i = 0; i < 8; ++i)
		{
			float dist = XMVectorGetX(XMVector3Length(cascadeFrustumCornersW[i] - cascadeFrustumCenter));
			sphereRadius = max(sphereRadius, dist);
		}
		sphereRadius = std::ceil(sphereRadius * 16.0f) / 16.0f;

		//// Set CascadeEnd World pos
		//m_fCascadedEndsW[currentCascade] = XMVectorGetZ(cascadeFrustumCenter) + sphereRadius;

		_vector mins = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0.f);
		_vector maxes = XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.f);

		maxes = XMVectorSet(sphereRadius, sphereRadius, sphereRadius, 0.f);
		mins = -maxes;

		_vector cascadeExtents = maxes - mins;

		// Get Position of the Shadow Camera
		_vector shadowCameraPos = cascadeFrustumCenter - XMVector3Normalize(vDirectionalLightDir) * fabs(XMVectorGetZ(mins));
		shadowCameraPos = XMVectorSetW(shadowCameraPos, 1.f);
		cascadeFrustumCenter = XMVectorSetW(cascadeFrustumCenter, 1.f);

		// Build Light View Proj
		_matrix ShadowView, ShadowProj;
		_vector vTarget = cascadeFrustumCenter;
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		ShadowView = XMMatrixLookAtLH(shadowCameraPos, vTarget, vUp);
		// aabb의 길이 w / 높이 h / 기준점 zn / 깊이 zf 
		ShadowProj = XMMatrixOrthographicLH(XMVectorGetX(cascadeExtents), XMVectorGetY(cascadeExtents), 0.f, XMVectorGetZ(cascadeExtents));


		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// CASE 2
		//// Ortho frustum in light space encloses scene.
		//shadowCameraPos = XMVector3Normalize(vDirectionalLightDir) * fabs(XMVectorGetZ(mins)) * -2.f;
		//shadowCameraPos = XMVectorSetW(shadowCameraPos, 1.f);
		//ShadowView = XMMatrixLookAtLH(shadowCameraPos, vTarget, vUp);

		//cascadeFrustumCenter = XMVector3TransformCoord(vTarget, ShadowView);

		//float l = XMVectorGetX(cascadeFrustumCenter) - sphereRadius;
		//float b =  XMVectorGetY(cascadeFrustumCenter) - sphereRadius;
		//float n =  XMVectorGetZ(cascadeFrustumCenter) - sphereRadius;
		//float r =  XMVectorGetX(cascadeFrustumCenter) + sphereRadius;
		//float t =  XMVectorGetY(cascadeFrustumCenter) + sphereRadius;
		//float f =  XMVectorGetZ(cascadeFrustumCenter) + sphereRadius;
		//ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		_matrix T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		_matrix S = ShadowView * ShadowProj * T;

		XMStoreFloat4x4(&m_CascadeViews[currentCascade], ShadowView);
		XMStoreFloat4x4(&m_CascadeProjs[currentCascade], ShadowProj);
		XMStoreFloat4x4(&m_CascadedShadowTransforms[currentCascade], S);

	}

	return S_OK;
}


void CShadow_Manager::Free()
{
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDevice_Context);
}
