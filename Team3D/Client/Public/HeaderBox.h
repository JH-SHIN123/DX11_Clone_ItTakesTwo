#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CHeaderBox final : public CUIObject
{
private:
	explicit CHeaderBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHeaderBox(const CUIObject& rhs);
	virtual ~CHeaderBox() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	_bool Get_Render() { return m_IsRender;  }
	_bool Get_LocalPlayRender();
	_bool Get_LogoDisappear() { return m_IsLogoDisappear; }
	_bool Get_Ready() { return m_IsReady; }

public:
	virtual void Set_ScaleEffect() override;
	void Set_NextSelect();
	void Set_PreviousSelect();
	void Set_LogoDisappear();
	void Set_Ready(_bool IsCheck);
	void Set_ColorChange();


private:
	_bool							m_IsRender = false;
	_float2							m_vFontPos;
	_bool							m_IsMousePicking = false;
	_bool							m_IsLogoDisappear = false;
	_int							m_iShaderOption = 0;
	_bool							m_IsReady = false;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void Render_Font();
	HRESULT Ready_Component();
	void Mouse_Picking();
	void Set_Option();
	void Set_ShaderOption();


public:
	static CHeaderBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END