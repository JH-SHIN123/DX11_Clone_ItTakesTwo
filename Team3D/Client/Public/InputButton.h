#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CInputButton final : public CUIObject
 {
private:
	explicit CInputButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInputButton(const CUIObject& rhs);
	virtual ~CInputButton() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_uint							m_iShaderPassNum = 0;
	_float2							m_vStartScale;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	void SetUp_Option();
	void ScaleEffect(_double TimeDelta);

public:
	static CInputButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END