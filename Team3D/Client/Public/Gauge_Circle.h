#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

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

 public:
	 void Set_Active(_bool IsActive);
	 void Set_TargetPos(_vector vPos) override;
	 void Set_SwingPointPlayerID(Player::ID ePlayerID);
	 void Set_Range(_float fRange);
	 void Set_Position(_vector vPos);
	 void Set_DefaultSetting();

private:
	 _float							m_fDistance = 0.f;
	 _float							m_fRange = 0.f;
	 _float							m_fAlpha = 0.f;
	 _bool							m_IsActive = false;
	 _bool							m_IsUICreateOn = false;
	 _uint							m_iOption = 0;

 private:
	 class CContextIcon*			m_pContextIcon = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void FindDistanceRatio();

private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_SwingPoint(const _tchar * pLayerTag);

public:
	static CGauge_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END