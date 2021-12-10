#include "..\public\VIBuffer.h"
#include "Pipeline.h"
#include "Shadow_Manager.h"
#include "RenderTarget_Manager.h"
#include "Graphic_Device.h"
#include "ShaderCompiler.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent			(rhs)
	, m_pVB					(rhs.m_pVB)
	, m_iVertexCount		(rhs.m_iVertexCount)
	, m_iVertexStride		(rhs.m_iVertexStride)
	, m_iVertexBufferCount	(rhs.m_iVertexBufferCount)
	, m_pIB					(rhs.m_pIB)
	, m_iFaceCount			(rhs.m_iFaceCount)
	, m_iFaceStride			(rhs.m_iFaceStride)
	, m_iIndexCount			(rhs.m_iIndexCount)
	, m_eIndexFormat		(rhs.m_eIndexFormat)
	, m_eTopology			(rhs.m_eTopology)
	, m_pEffect				(rhs.m_pEffect)
	, m_InputLayouts		(rhs.m_InputLayouts)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pEffect);

	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_AddRef(InputLayout.pLayout);
		Safe_AddRef(InputLayout.pPass);
	}
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	CComponent::NativeConstruct_Prototype();

	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CVIBuffer::Render(_uint iPassIndex)
{
	NULL_CHECK_RETURN(m_pDeviceContext, E_FAIL);

	_uint	iOffSet = 0;

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, &m_pVB, &m_iVertexStride, &iOffSet);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->IASetInputLayout(m_InputLayouts[iPassIndex].pLayout);
	m_InputLayouts[iPassIndex].pPass->Apply(0, m_pDeviceContext);
	m_pDeviceContext->DrawIndexed(m_iIndexCount, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Set_Variable(const char * pConstantName, void * pData, _uint iByteSize)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iByteSize);
}

HRESULT CVIBuffer::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView * pResourceView)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetResource(pResourceView);
}

HRESULT CVIBuffer::Set_DefaultVariables_Perspective(_fmatrix WorldMatrix)
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	NULL_CHECK_RETURN(pPipeline, E_FAIL);

	Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	
	Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar			= pPipeline->Get_MainCamFar();
	_float	fSubCamFar			= pPipeline->Get_SubCamFar();
	_vector vMainCamPosition	= pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition		= pPipeline->Get_SubCamPosition();

	Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));
	Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));

	return S_OK;
}

HRESULT CVIBuffer::Set_DefaultVariables_Shadow()
{
	CGraphic_Device* pGraphicDevice = CGraphic_Device::GetInstance();
	_float4	vViewportUVInfo;
	vViewportUVInfo = pGraphicDevice->Get_ViewportRadioInfo(CGraphic_Device::VP_MAIN);
	Set_Variable("g_vMainViewportUVInfo", &vViewportUVInfo, sizeof(_float4));
	vViewportUVInfo = pGraphicDevice->Get_ViewportRadioInfo(CGraphic_Device::VP_SUB);
	Set_Variable("g_vSubViewportUVInfo", &vViewportUVInfo, sizeof(_float4));

	CShadow_Manager* pShadowManager = CShadow_Manager::GetInstance();
	_matrix ShadowTransform[MAX_CASCADES]; /* Shadow View * Shadow Proj * NDC */
	pShadowManager->Get_CascadeShadowTransformsTranspose(CShadow_Manager::SHADOW_MAIN, ShadowTransform);
	Set_Variable("g_ShadowTransforms_Main", ShadowTransform, sizeof(_matrix) * MAX_CASCADES);
	pShadowManager->Get_CascadeShadowTransformsTranspose(CShadow_Manager::SHADOW_SUB, ShadowTransform);
	Set_Variable("g_ShadowTransforms_Sub", ShadowTransform, sizeof(_matrix) * MAX_CASCADES);
	Set_Variable("g_CascadeEnds", (void*)pShadowManager->Get_CascadedEnds(), sizeof(_float) * (MAX_CASCADES + 1));

	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::GetInstance();
	Set_ShaderResourceView("g_CascadedShadowDepthTexture", pRenderTargetManager->Get_ShaderResourceView(TEXT("Target_CascadedShadow_Depth")));

	return S_OK;
}

HRESULT CVIBuffer::Set_DefaultVariables_ShadowDepth(_fmatrix WorldMatrix)
{
	Set_Variable("g_WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));

	_matrix ShadowViewProj[MAX_CASCADES];

	CShadow_Manager* pShadowManager = CShadow_Manager::GetInstance();

	pShadowManager->Get_CascadeShadowViewProjTranspose(CShadow_Manager::SHADOW_MAIN, ShadowViewProj);
	Set_Variable("g_ShadowTransforms_Main", ShadowViewProj, sizeof(_matrix) * MAX_CASCADES);

	pShadowManager->Get_CascadeShadowViewProjTranspose(CShadow_Manager::SHADOW_SUB, ShadowViewProj);
	Set_Variable("g_ShadowTransforms_Sub", ShadowViewProj, sizeof(_matrix) * MAX_CASCADES);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer ** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void * pSysMem, _bool bDelete, _uint iSysMemPitch, _uint iSysMemSlicePitch)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth				= iByteWidth;
	Desc.Usage					= Usage;
	Desc.BindFlags				= iBindFlags;
	Desc.CPUAccessFlags			= iCPUAccessFlags;
	Desc.MiscFlags				= iMiscFlags;
	Desc.StructureByteStride	= iStructureByteStride;

	D3D11_SUBRESOURCE_DATA SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem				= pSysMem;
	SubResourceData.SysMemPitch			= iSysMemPitch;
	SubResourceData.SysMemSlicePitch	= iSysMemSlicePitch;

	FAILED_CHECK_RETURN(m_pDevice->CreateBuffer(&Desc, &SubResourceData, ppBuffer), E_FAIL);

	if (bDelete)
		Safe_Delete_Array(pSysMem);

	return S_OK;
}

void CVIBuffer::Store_VertexBufferInfo(_uint iVertexBufferCount, _uint iVertexCount_Main, _uint iVertexStride_Main)
{
	m_iVertexBufferCount	= iVertexBufferCount;
	m_iVertexCount			= iVertexCount_Main;
	m_iVertexStride			= iVertexStride_Main;
}

void CVIBuffer::Store_IndexBufferInfo(_uint iFaceCount, DXGI_FORMAT eFormat, D3D11_PRIMITIVE_TOPOLOGY eTopology)
{
	m_iFaceCount	= iFaceCount;
	m_eIndexFormat	= eFormat;
	m_eTopology		= eTopology;

	if (DXGI_FORMAT_R16_UINT == eFormat)
		m_iFaceStride = sizeof(POLYGON_INDICES16);
	else if (DXGI_FORMAT_R32_UINT == eFormat)
		m_iFaceStride = sizeof(POLYGON_INDICES32);
	else
		MSG_BOX("Undefined IndexFormat Type.");

	if (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST == eTopology)
		m_iIndexCount = iFaceCount * 3;
	else if (D3D_PRIMITIVE_TOPOLOGY_LINELIST == eTopology)
		m_iIndexCount = iFaceCount * 2;
	else if (D3D_PRIMITIVE_TOPOLOGY_POINTLIST == eTopology)
		m_iIndexCount = iFaceCount * 1;
	else
		MSG_BOX("Undefined Topology Type.");
}

HRESULT CVIBuffer::SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC* pInputElementDesc, _uint iElementCount, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	ID3DBlob* pCompiledShaderCode = CShaderCompiler::GetInstance()->Get_CompiledCode(pShaderFilePath);
	FAILED_CHECK_RETURN(D3DX11CreateEffectFromMemory(pCompiledShaderCode->GetBufferPointer(), pCompiledShaderCode->GetBufferSize(), 0, m_pDevice, &m_pEffect), E_FAIL);

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByName(pTechniqueName);
	NULL_CHECK_RETURN(pTechnique, E_FAIL);

	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	FAILED_CHECK_RETURN(pTechnique->GetDesc(&TechniqueDesc), E_FAIL);

	m_InputLayouts.reserve(TechniqueDesc.Passes);

	for (_uint iPassIndex = 0; iPassIndex < TechniqueDesc.Passes; ++iPassIndex)
	{
		INPUT_LAYOUT_DESC	InputLayoutDesc;
		D3DX11_PASS_DESC	PassDesc;

		InputLayoutDesc.pPass = pTechnique->GetPassByIndex(iPassIndex);
		FAILED_CHECK_RETURN(InputLayoutDesc.pPass->GetDesc(&PassDesc), E_FAIL);
		FAILED_CHECK_RETURN(m_pDevice->CreateInputLayout(pInputElementDesc, iElementCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &InputLayoutDesc.pLayout), E_FAIL);

		m_InputLayouts.emplace_back(InputLayoutDesc);
	}

	Safe_Release(pTechnique);

	return S_OK;
}

void CVIBuffer::Free()
{
	CComponent::Free();

	for (auto& InputLayout : m_InputLayouts)
	{
		Safe_Release(InputLayout.pPass);
		Safe_Release(InputLayout.pLayout);
	}
	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
