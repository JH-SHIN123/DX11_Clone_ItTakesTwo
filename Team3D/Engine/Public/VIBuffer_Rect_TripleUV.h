#pragma once

#ifndef __VIBUFFER_RECT_TRIPLEUV_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_TripleUV final : public CVIBuffer
{
private:
	explicit CVIBuffer_Rect_TripleUV(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Rect_TripleUV(const CVIBuffer_Rect_TripleUV& rhs);
	virtual ~CVIBuffer_Rect_TripleUV() = default;

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual HRESULT	NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);

public:
	static CVIBuffer_Rect_TripleUV* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};


END

#define __VIBUFFER_RECT_TRIPLEUV_H__
#endif // !__VIBUFFER_RECT_TRIPLEUV_H__
