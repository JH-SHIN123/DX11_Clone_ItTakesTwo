#pragma once

#ifndef __VIBUFFER_POINTINSTANCE_CUSTOM_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_PointInstance_Custom final : public CVIBuffer
{
private:
	explicit CVIBuffer_PointInstance_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance_Custom(const CVIBuffer_PointInstance_Custom& rhs);
	virtual ~CVIBuffer_PointInstance_Custom() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;
	HRESULT			Render(_uint iPassIndex, void* VertexMatrices, _uint iInstanceCount, _uint iStartIndex = 0);
	VTXMATRIX_CUSTOM_ST*		Get_InstanceBuffer() { return m_pInstanceVertices; }

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	VTXMATRIX_CUSTOM_ST*	m_pInstanceVertices = nullptr;
	_uint					m_iMaxInstanceCount = 0;

public:
	static CVIBuffer_PointInstance_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END
#define __VIBUFFER_POINTINSTANCE_CUSTOM_H__
#endif // !__VIBUFFER_POINTINSTANCE_CUSTOM_H__
