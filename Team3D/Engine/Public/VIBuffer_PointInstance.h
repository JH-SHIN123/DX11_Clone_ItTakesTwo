#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer
{
private:
	explicit CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;
	HRESULT			Render(_uint iPassIndex, void* VertexMatrices, _uint iInstanceCount);

private:
	ID3D11Buffer*	m_pVBInstance = nullptr;
	VTXMATRIX*		m_pInstanceVertices = nullptr;
	_uint			m_iMaxInstanceCount = 0;

public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END