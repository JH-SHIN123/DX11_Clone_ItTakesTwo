#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_FontInstance final : public CVIBuffer
{
private:
	explicit CVIBuffer_FontInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_FontInstance(const CVIBuffer_FontInstance& rhs);
	virtual ~CVIBuffer_FontInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;
	HRESULT			Render(_uint iPassIndex, void* VertexMatrices, _uint iInstanceCount);

private:
	ID3D11Buffer*	m_pVBInstance = nullptr;
	VTXFONT*			m_pInstanceVertices = nullptr;
	_uint			m_iMaxInstanceCount = 0;

public:
	static CVIBuffer_FontInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END