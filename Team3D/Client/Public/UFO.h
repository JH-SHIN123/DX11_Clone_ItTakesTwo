#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CControllableActor;
END

BEGIN(Client)

class CUFO : public CGameObject
{
protected:
	explicit CUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFO(const CUFO& rhs);
	virtual ~CUFO() = default;

public:
	enum UFO_STATE {
		UFO_ENTIRE_ANIMATION, UFO_END

	};
	enum UFO_TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;
	virtual HRESULT Set_ShaderConstant_Default() override;
	virtual HRESULT Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix) override;


	CTransform* Get_Transform() { return m_pTransformCom; }

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CControllableActor* m_pActorCom = nullptr;

	CTransform*			m_pCodyTransform = nullptr;
	CTransform*			m_pMayTransform = nullptr;

public:
	CUFO::UFO_STATE Check_State(_double TimeDelta);
	void Change_State(_double TimeDelta);
	void During_Animation_Behavior(_double TimeDelta);


private:
	UFO_STATE m_eCurState = UFO_ENTIRE_ANIMATION;
	UFO_STATE m_eNextState = UFO_ENTIRE_ANIMATION;

	UFO_TARGET m_eTarget = TARGET_END;




public:
	static CUFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END