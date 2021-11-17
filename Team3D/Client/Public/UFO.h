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
		UFO_ENTIRE_ANIMATION , UFO_REF, UFO_BACK,
		UFO_CODYHOLDING, UFO_CODYHOLDING_ENTER, UFO_CODYHOLDING_LOW, 
		UFO_CONTROLLABLE_ADDITIVE, UFO_CONTROLLABLE_ADDITIVE_BOOST, UFO_CONTROLLABLE_ADDITIVE_FLYING,
		UFO_CONTROLLABLE_POSE_BCK, UFO_CONTROLLABLE_POSE_FWD, UFO_CONTROLLABLE_POSE_LEFT, UFO_CONTROLLABLE_POSE_RIGHT, 
		UFO_FIREROCKET_ADDITIVE_LEFT, UFO_FIREROCKET_ADDITIVE_RIGHT, UFO_FWD, UFO_GROUNDPOUND, UFO_HITREACTION_BCK, UFO_HITREACTION_FWD, 
		UFO_HITREACTION_LEFT, UFO_HITREACTION_RIGHT, UFO_KNOCKDOWNMH, UFO_LASER_HITPOD, UFO_LASER_MH, UFO_LASERRIPPEDOFF, UFO_LEFT, UFO_MH, UFO_RIGHT, UFO_ROCKETKNOCKDOWN_MH, 
		CUTSCENE_EJECT_FLYINGSAUCER, CUTSCENE_ENTERUFO_FLYINGSAUCER, CUTSCENE_UFO_BOSS_INTRO, CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER, 
		CUTSCENE_UFO_OUTRO, CUTSCENE_POWERCORESDESTROYED_UFO, CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER
	};
	enum UFO_TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel* Get_Model() { return m_pModelCom; }

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CControllableActor* m_pActorCom = nullptr;

	CTransform*			m_pCodyTransform = nullptr;
	CTransform*			m_pMayTransform = nullptr;

public:
	CUFO::UFO_STATE Check_State(_double dTimeDelta);
	void Change_State(_double dTimeDelta);
	void During_Animation_Behavior(_double dTimeDelta);

	// Patterns 

	// Phase 1
	// 컷씬 이후는 레이저 패턴        
	// 밑에서 나오는 캡슐의 코어 터트리면 가운데에서 터렛 나오면서 레이저 회전 + 보스가 유탄 날림 
	// 이후 다시 레이저 패턴
	// 다시 레이저로 캡슐 코어 터트리기
	// 보스가 추락함. 
	// 코디가 커진후에 우주선 들어올리고 메이가 레이저 뽑아버림.
	
	// Phase 2
	// 우주선이 맵 외각 천천히 돌면서 로켓발사
	// 로켓은 유도! 일정 거리이상 움직이면 바닥으로 떨어짐.
	// 이 때, 플레이어는 이 로켓을 주워서 우주선에 박아야 함.
	// 체력이 한칸남을 때 까지 반복.
	// 우주선 추락.

	// Phase 3
	// 추락한 우주선에서 코디가 작아져서 우주선 타기 전까지는 
	// 원숭이가 우주선 고치는 애니메이션 무한 재생.
	// 코디가 우주선에 타면, 코디쪽은 우주선 무력화 시키기 시작.
	// 메이는 외부에서 레이저 피하기 시작. (중간8방향포탑, 구석에 4방향 레이저, 우주선 내려찍기, 바닥 전기장판)
	// 코디가 맵 다 뚫고 사출 버튼 누르면 우주선 해치열리고 바닥에 떨어짐. 원숭이는 달로 사출.
	// 메이가 우주선에 타는 컷씬. -> 코디가 우주선 타는 컷씬 -> 레버 발견

	// Phase 4
	// 우주선 타고 원숭이 추격
	// 메이는 우주선 조종, 코디는 레버로 원숭이 맞추기.
	// 한 2~3번 맞추면 Outro 애니메이션 재생.


	void Laser_Pattern(_double dTimeDelta); 


private:
	UFO_STATE m_eCurState = UFO_ENTIRE_ANIMATION;
	UFO_STATE m_eNextState = UFO_ENTIRE_ANIMATION;

	UFO_TARGET m_eTarget = TARGET_END;

	/* For.Battle */
	_bool m_IsCoreExplode = false;
	_bool m_IsCodyHold = false;
	_bool m_IsMayLaserRippedOff = false;




public:
	static CUFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END