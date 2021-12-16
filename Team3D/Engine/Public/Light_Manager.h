#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)
class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

private:
	explicit CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public: /* Getter */
	CLight* Get_Light(LightStatus::Enum eState, const _tchar* pLightTag, _bool bAddRef = true); /* Careful to use func, too heavy */

public:
	HRESULT Ready_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	_int	Tick_LightManager(_double dTimeDelta); /* Static & Directional is not operate Tick */
	HRESULT Render_LightManager();

public:
	HRESULT	Add_Light(LightStatus::Enum eState, CLight* pLight);
	HRESULT Remove_Light(const _tchar* pLightTag); /* Can remove only dyamic lights and, careful to use func, too heavy */
	void	Clear_Lights(LightStatus::Enum eState);

private:
	typedef vector<class CLight*>	STATIC_LIGHTS;
	typedef list<class CLight*>		DYNAMIC_LIGHTS;

private:
	class CVIBuffer_RectRHW*	m_pVIBuffer = nullptr;

private:
	CLight*						m_pDirectionalLight = nullptr; /* must be only one in game */
	STATIC_LIGHTS				m_StaticLights;
	DYNAMIC_LIGHTS				m_DynamicLights;

public:
	virtual void	Free() override;
	void			Clear_All();

};
END

/*
	VIBuffer는 매니저에 하나만 세팅한다.
	각각의 Light를 렌더할 때는 매니저의 버퍼를 가져가서 그리게한다.
	모든 빛이 공통으로 가지는 값들이 세팅되어 있음.
	> 디퓨즈/노말 렌더타겟, 카메라관련
*/