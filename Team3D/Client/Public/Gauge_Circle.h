#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CGauge_Circle final : public CUIObject
 {
private:
	explicit CGauge_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGauge_Circle(const CUIObject& rhs);
	virtual ~CGauge_Circle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();

public:
	static CGauge_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END