#pragma once


#include "Base.h"

BEGIN(Engine)
class CSSAO final : public CBase
{
	DECLARE_SINGLETON(CSSAO)

public:
	explicit CSSAO() = default;
	virtual ~CSSAO() = default;

public:
	HRESULT Ready_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	_uint	m_iWinSize[2] = { 0,0 };
	_uint	m_iDownScaleGroups = 0;

private: /* For.CS - Shader */
	ID3DX11Effect* m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private: /* For. CS - First Pass */
	ID3D11Buffer* m_pSSAOBuffer = nullptr;

private: /* For. Blur */


public:
	virtual void	Free() override;
};
END
