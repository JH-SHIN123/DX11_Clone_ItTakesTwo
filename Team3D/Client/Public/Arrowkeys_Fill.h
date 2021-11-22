#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CArrowkeys_Fill final : public CUIObject
 {
private:
	explicit CArrowkeys_Fill(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CArrowkeys_Fill(const CUIObject& rhs);
	virtual ~CArrowkeys_Fill() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	 _double						m_UpDownTime = 0;
	 _uint							m_iUpDownCount = 0;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void UpDown(_double TimeDelta);

private:
	HRESULT Ready_Component();

public:
	static CArrowkeys_Fill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END