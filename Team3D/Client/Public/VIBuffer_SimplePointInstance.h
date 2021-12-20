#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CVIBuffer_SimplePointInstance final : public CVIBuffer
{
private:
	explicit CVIBuffer_SimplePointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_SimplePointInstance(const CVIBuffer_SimplePointInstance& rhs);
	virtual ~CVIBuffer_SimplePointInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;
	HRESULT			Render(_uint iPassIndex, void* VertexMatrices, _uint iInstanceCount);

private:
	VTXMATRIX*	m_pInstanceVertices = nullptr;
	_uint		m_iMaxInstanceCount = 0;

public:
	static CVIBuffer_SimplePointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END