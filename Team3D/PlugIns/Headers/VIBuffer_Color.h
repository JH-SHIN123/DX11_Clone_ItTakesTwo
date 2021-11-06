#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Color final : public CVIBuffer
{
private:
	explicit CVIBuffer_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Color(const CVIBuffer_Color& rhs);
	virtual ~CVIBuffer_Color() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(_fvector vColor, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT	NativeConstruct(void* pArg) override;

public:
	static CVIBuffer_Color* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _fvector vColor, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END