#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(_uint iVertexCountX, _uint iVertexCountZ, _float fInterval, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT	NativeConstruct(void* pArg) override;

private:
	_uint m_iVertexCountX = 0;
	_uint m_iVertexCountZ = 0;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iVertexCountX, _uint iVertexCountZ, _float fInterval, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual	void Free() override;
};

END