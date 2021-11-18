#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual HRESULT	Render(_uint iPassIndex);
	/* For.Shader */
	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView);
	HRESULT	Set_DefaultVariables_Perspective(_fmatrix WorldMatrix);
	HRESULT	Set_DefaultVariables_Shadow();
	HRESULT	Set_DefaultVariables_ShadowDepth();

protected:
	/* For.Vertices */
	ID3D11Buffer*				m_pVB = nullptr;
	_uint						m_iVertexCount = 0;
	_uint						m_iVertexStride = 0;
	_uint						m_iVertexBufferCount = 0;
	/* For.Indices */
	ID3D11Buffer*				m_pIB = nullptr;
	_uint						m_iFaceCount = 0;
	_uint						m_iFaceStride = 0;
	_uint						m_iIndexCount = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	/* For.Shader */
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts;
protected:
	HRESULT	Create_Buffer(ID3D11Buffer** ppBuffer, _uint iByteWidth, D3D11_USAGE Usage, _uint iBindFlags, _uint iCPUAccessFlags, _uint iMiscFlags, _uint iStructureByteStride, void* pSysMem, _bool bDelete = true, _uint iSysMemPitch = 0, _uint iSysMemSlicePitch = 0);
	void	Store_VertexBufferInfo(_uint iVertexBufferCount, _uint iVertexCount_Main, _uint iVertexStride_Main);
	void	Store_IndexBufferInfo(_uint iFaceCount, DXGI_FORMAT eFormat, D3D11_PRIMITIVE_TOPOLOGY eTopology);
	HRESULT SetUp_InputLayouts(D3D11_INPUT_ELEMENT_DESC* pInputElementDesc, _uint iElementCount, const _tchar* pShaderFilePath, const char* pTechniqueName);

public:
	virtual CComponent* Clone_Component(void* pArg) PURE;
	virtual void Free() PURE;
};

END

/*
	Set_DefaultVariables_Perspective 함수
	> 원근 투영 단계까지 필요한 변수들 기본 세팅
	> 월드/뷰/투영/카메라정보

	Create_Buffer 함수
	> 버퍼 생성 함수, bDelete를 false로 주면 pSysMem으로 넘긴 데이터를 지우지 않는다.
*/