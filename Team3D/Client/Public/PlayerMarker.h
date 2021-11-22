#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerMarker final : public CUIObject
 {
private:
	explicit CPlayerMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerMarker(const CUIObject& rhs);
	virtual ~CPlayerMarker() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_bool							m_IsRender = false;
	_int							m_iColorOption = 0;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Set_PlayerMarkerVariables_Perspective();

private:
	_float3	ConvertToWindowPos(D3D11_VIEWPORT Viewprot, _vector vPos);
	_float3 ConvertToViewPos(D3D11_VIEWPORT Viewport, _float3 vPos);

public:
	static CPlayerMarker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END