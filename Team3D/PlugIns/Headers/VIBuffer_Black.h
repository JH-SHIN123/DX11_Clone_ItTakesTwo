#ifndef VIBuffer_Black_h__
#define VIBuffer_Black_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Black final : public CVIBuffer
{
private:
	explicit CVIBuffer_Black(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Black(const CVIBuffer_Black& rhs);
	virtual ~CVIBuffer_Black() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype	(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT	NativeConstruct				(void* pArg) override;

public:
	static CVIBuffer_Black*	Create			(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent*		Clone_Component	(void* pArg) override;
	virtual void			Free			() override;
};

END

#endif // VIBuffer_Black_h__