#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CReturn_Button : public CGameObject
{
protected:
	explicit CReturn_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CReturn_Button(const CReturn_Button& rhs);
	virtual ~CReturn_Button() = default;

public:
	_bool Get_ChangeScene() { return m_bChangeScene; }

public:
	void Set_Ready() { m_bReady = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	CTextures*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

	_bool				m_bReady = false;
	_bool				m_bChangeScene = false;
private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CReturn_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END