#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CUFO : public CGameObject
{
private:
	explicit CUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFO(const CUFO& rhs);
	virtual ~CUFO() = default;

public:
	enum UFO_STATE {
		UFO_END
	};

	enum UFO_TARGET { TARGET_CODY , TARGET_MAY, TARGET_END };
	enum UFO_PHASE { PHASE_1, PHASE_2, PHASE_3, PHASE_END };
	enum UFO_PATTERN { INTERACTION, LASER, GRAVITATIONALBOMB, GUIDEDMISSILE, PRESS, PATTERN_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;


public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() const { return m_pTransformCom; }
	CModel* Get_Model() const { return m_pModelCom; }
	_float4 Get_LaserStartPos() const { return m_vLaserGunPos; }
	_float4 Get_LaserDir() const { return m_vLaserDir; }

private:
	UFO_TARGET			m_eTarget = TARGET_END;
	UFO_PHASE			m_ePhase = PHASE_END;
	UFO_PATTERN			m_ePattern = PATTERN_END;

private:
	/* For.CutScene */
	_bool				m_IsCutScene = false;

	/* For.AllPHASE */
	_float				m_fWaitingTime = 0.f;
	_uint				m_iPatternChangeCount = 0;
	_uint				m_iPhaseChangeCount = 0;
	_bool				m_IsInterActive = false;
	_float4				m_vStartUFOPos;

	/* For.PHASE_1 Pattern */
	_bool				m_IsCoreExplosion = false;
	_bool				m_IsStartingPointMove = false;
	_bool				m_IsLaserCreate = true;
	_float				m_fGravitationalBombLanchTime = 0.f;
	_uint				m_iGravitationalBombCount = 0;

	/* For.PHASE_2 Pattern */
	_float				m_fRevAngle = 0.f;
	_float				m_fRotAngle = 0.f;
	_float				m_fGuidedMissileTime = 0.f;
	_float4				m_vTranslationPos;
	_bool				m_IsGuidedMissileCreate = true;
	

	/* For.PHASE_3 Pattern */


	/* For. Laser_TypeA */
	_float4				m_vLaserGunPos;
	_float4				m_vLaserDir;


private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pCodyTransform = nullptr;
	CTransform*			m_pMayTransform = nullptr;

private:
	/* For.AllPHASE */
	

	/* For.PHASE_1 Pattern */
	void Phase1_Pattern(_double dTimeDelta);
	void Phase1_InterAction(_double dTimeDelta);
	void Laser_Pattern(_double dTimeDelta);
	void MoveStartingPoint(_double dTimeDelta);
	void GravitationalBomb_Pattern(_double dTimeDelta);

	/* For.PHASE_2 Pattern */
	void Phase2_Pattern(_double dTimeDelta);
	void Phase2_InterAction(_double dTimeDelta);
	void OrbitalMovementCenter(_double dTimeDelta);
	void GuidedMissile_Pattern(_double dTimeDelta);

	/* For.PHASE_3 Pattern */
	void Phase3_Pattern(_double dTimeDelta);

private:
	/* For.PHASE_1 End */
	void Phase1_End(_double dTimeDelta);
	HRESULT Ready_Actor_Component();

	/* For.PHASE_2 End */
	void Phase2_End(_double dTimeDelta);

	/* For.PHASE_3 End */
	void Phase3_End(_double dTimeDelta);

private:
	/* For.NativeConstruct */
	void Add_LerpInfo_To_Model();
	HRESULT Ready_Component();

public:
	static CUFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END

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
