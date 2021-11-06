#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public: /* Getter */
	LIGHT_DESC* Get_LightDescPtr(_uint iIndex);

public: /* Setter */
	HRESULT TurnOn_Light(_uint iIndex);
	HRESULT TurnOff_Light(_uint iIndex);

public:
	HRESULT Ready_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT	Reserve_Container(_uint iCount);
	HRESULT	Add_Light(const LIGHT_DESC& pLightDesc, _bool isActive);
	HRESULT Render_Lights();
	void	Clear_Lights();
	void	Clear_Buffer();

private: /* Typedef */
	typedef vector<class CLight*> LIGHTS;
private:
	class CVIBuffer_RectRHW*	m_pVIBuffer = nullptr;
	LIGHTS						m_Lights;

public:
	virtual void Free() override;
};

END

/*
	VIBuffer는 매니저에 하나만 세팅한다.
	각각의 Light를 렌더할 때는 매니저의 버퍼를 가져가서 그리게한다.
	모든 빛이 공통으로 가지는 값들이 세팅되어 있음.
	> 디퓨즈/노말 렌더타겟, 카메라관련
*/