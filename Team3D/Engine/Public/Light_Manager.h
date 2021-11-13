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
	LIGHT_DESC*		Get_LightDescPtr(const _tchar* pLightTag);
	class CLight*	Get_DirectionalLight() const { return m_pDirectionalLight; }

public: /* Setter */
	HRESULT TurnOn_Light(const _tchar* pLightTag);
	HRESULT TurnOff_Light(const _tchar* pLightTag);

public:
	HRESULT Ready_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT	Reserve_Container(_uint iCount);
	HRESULT	Add_Light(const _tchar* pLightTag, const LIGHT_DESC& pLightDesc, _bool isActive);
	HRESULT Render_Lights();
	void	Clear_Lights();
	void	Clear_Buffer();

private: /* Typedef */
	typedef unordered_map<const _tchar* ,class CLight*> LIGHTS;
	// typedef vector<class CLight*> LIGHTS; // dynamic light (effect light)

private:
	class CVIBuffer_RectRHW*	m_pVIBuffer = nullptr;
	LIGHTS						m_Lights;
	class CLight*				m_pDirectionalLight = nullptr; /* must be only one in game */

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