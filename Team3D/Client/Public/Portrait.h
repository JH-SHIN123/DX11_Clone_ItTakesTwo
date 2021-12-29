#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPortrait final : public CUIObject
{
private:
	explicit CPortrait(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPortrait(const CUIObject& rhs);
	virtual ~CPortrait() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Active(_bool IsActive);
	void Set_Position(_float fTime);

private:
	_uint							m_iOption = 0;
	_uint							m_iShaderPassNum = 0;
	_bool							m_IsActive = false;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();

public:
	static CPortrait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END