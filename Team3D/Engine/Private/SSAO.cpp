#include "SSAO.h"

IMPLEMENT_SINGLETON(CSSAO)

HRESULT CSSAO::Ready_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight)
{
	return S_OK;
}

void CSSAO::Free()
{
}
