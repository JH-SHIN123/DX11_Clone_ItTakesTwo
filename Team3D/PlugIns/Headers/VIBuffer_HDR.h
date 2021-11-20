#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CVIBuffer_HDR final : public CVIBuffer /* For HDR Constant Buffer - Use it Compute Shader */
{
private:
	explicit CVIBuffer_HDR(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_HDR(const CVIBuffer_HDR& rhs);
	virtual ~CVIBuffer_HDR() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char* pTechniqueName);

public:
	static CVIBuffer_HDR* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override { return nullptr; }; /* »ç¿ë X */
	virtual void Free() override;
};
END