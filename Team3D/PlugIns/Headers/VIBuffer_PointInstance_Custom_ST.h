#pragma once

#ifndef __VIBUFFER_POINTINSTANCE_CUSTOM_ST_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_PointInstance_Custom_ST final : public CVIBuffer
{
private:
	explicit CVIBuffer_PointInstance_Custom_ST(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance_Custom_ST(const CVIBuffer_PointInstance_Custom_ST& rhs);
	virtual ~CVIBuffer_PointInstance_Custom_ST() = default;

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
	static CVIBuffer_PointInstance_Custom_ST* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iMaxInstanceCount, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END
#define __VIBUFFER_POINTINSTANCE_CUSTOM_ST_H__
#endif // !__VIBUFFER_POINTINSTANCE_CUSTOM_ST_H__
