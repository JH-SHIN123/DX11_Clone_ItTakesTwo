#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sprite final : public CVIBuffer
{
private:
	explicit CVIBuffer_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs);
	virtual ~CVIBuffer_Sprite() = default;

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual HRESULT	NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);

public:
	static CVIBuffer_Sprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END