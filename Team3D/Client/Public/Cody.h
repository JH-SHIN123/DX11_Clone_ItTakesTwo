#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Client)
class CSpaceRail;
class CSpaceRail_Node;
class CCody final : public CCharacter
{
#pragma region Enum_STATE
public:
	enum CODY_STATE {
		/* For.Wall Actions */
		ENTIRE_ANIMATIONS, WALLSLIDE_ENTER, WALLSLIDE_ENTER_FROMDASH, WALLSLIDE_ENTER_NOTJUMPABLE, WALLSLIDE_JUMP,
		WALLSLIDE_JUMP_VERTICAL, WALLSLIDE_IDLE, WALLSLIDE_IDLE_READYTODASH, WALLRUN_LEFT_ENTER,
		WALLRUN_LEFT_JUMP, WALLRUN_LEFT_RUN, WALLRUN_RIGHT_ENTER, WALLRUN_RIGHT_JUMP, WALLRUN_RIGHT_RUN,

		/* For.Slide Actions */
		SLIDE_ENTER, SLIDE_ENTER_DASH, SLIDE_ENTER_FALLING, SLIDE_ENTER_GROUNDPOUND, SLIDE_EXIT_FALLING,
		SLIDE_EXIT_TO_MOVEMENT, SLIDE_EXIT_TO_MOVEMENT_WALK, SLIDE_FWD, SLIDE_JUMP, SLIDE_LEFT, SLIDE_RIGHT,

		/* For.Roll Actions */
		AIRDASH_START, ROLL_FROM_GROUNDPOUND, ROLL_FROM_PERFECTJUMP, ROLL_JOG_FROM_PERFECTDASH, ROLL_JUMP,
		ROLL_PERFECTDASH, ROLL_PERFECTJUMP, ROLL_START, ROLL_STOP, ROLL_STOP_FROM_PERFECTDASH, ROLL_TO_JOG,
		ROLL_TO_WALK, ROLL_WALK_FROM_PERFECTDASH,

		/* For.LedgeGrab Actions */
		LEDGEVAULT, LEDGEVAULTSTART, LEDGEGRAB_CLIMBLEFT, LEDGEGRAB_CLIMBRIGHT, LEDGEGRAB_CLIMBUP, LEDGEGRAB_DROP,
		LEDGEGRAB_JUMPBACK_LEFT, LEDGEGRAB_JUMPBACK_RIGHT, LEDGEGRAB_JUMPUP, LEDGEGRAB_IDLE, LEDGEGRAB_IDLE_LEFT120,
		LEDGEGRAB_IDLE_LEFT180, LEDGEGRAB_IDLE_LEFT30, LEDGEGRAB_IDLE_LEFT60, LEDGEGRAB_IDLE_LEFT90,
		LEDGEGRAB_IDLE_RIGHT120, LEDGEGRAB_IDLE_RIGHT180, LEDGEGRAB_IDLE_RIGHT30, LEDGEGRAB_IDLE_RIGHT60, LEDGEGRAB_IDLE_RIGHT90,
		LEDGEGRAB_START,

		/* For.Jump Actions */
		DOUBLEJUMP, JUMP_180L, JUMP_180R, JUMP_FALLING, JUMP_FALLING_BACKWARDS, JUMP_FALLING_BOOSTED, JUMP_FALLING_BOOSTED_LEFT,
		JUMP_FALLING_BOOSTED_RIGHT, JUMP_FALLING_BOOSTED_SUCKED, JUMP_FALLING_FORWARDS, JUMP_FALLING_LEFT, JUMP_FALLING_RIGHT,
		JUMP_FROMDASH, JUMP_FROMDASHSTOP, JUMP_LAND, IDLE, JUMP_LAND_DASH, JUMP_LAND_DASH_JOG, JUMP_LAND_DASH_WALK, JUMP_LAND_HIGH,
		JUMP_LAND_HIGH_JOG, JUMP_LAND_HIGH_JOG_CANCEL, JUMP_LAND_HIGH_JOG_JUMP, JUMP_LAND_HIGH_START_STILL, JUMP_LAND_HIGH_START_STILL_WALK,
		JUMP_LAND_HIGH_WALK, JUMP_LAND_JOG, JUMP_LAND_JOG_JUMP, JUMP_LAND_JOG_JUMP_VAR2, JUMP_LAND_JOG_JUMP_VAR3, JUMP_LAND_JOG_JUMP_VAR4, JUMP_LAND_JOG_JUMP_VAR5, JUMP_LAND_JOG_JUMP_VAR6,
		JUMP_LAND_PERFECTDASH, JUMP_LAND_PERFECTDASH_JOG, JUMP_LAND_PERFECTDASH_WALK, JUMP_LAND_START_STILL, JUMP_LAND_START_STILL_WALK, JUMP_LAND_START_STILL_JUMP, JUMP_LAND_WALK, JUMP_LONGJUMP,
		JUMP_LONGJUMP_PICKUPSMALL, JUMP_LONGJUMP_VAR1, JUMP_START, JUMPFROMPERFECTDASH, JUMPFROMPERFECTDASHSTOP, SKYDIVELANDING, SKYDIVELANDING_FWD, SKYDIVELANDING_WALK, SPRINT_JUMP,

		/* For.Grind Actions (레일맵) */
		GRIND_BTN_PRESS, GRIND_BTN_READY, GRIND_DASH, GRIND_FAST_IDLE, GRIND_FAST_IDLE_LEFT, GRIND_FAST_IDLE_RIGHT, GRIND_GRAPPLE_ENTER, GRIND_GRAPPLE_TO_GRIND,
		GRIND_JUMP, GRIND_LAND, GRIND_SLOW_IDLE, GRIND_SLOW_IDLE_LEFT, GRIND_SLOW_IDLE_LOOKLEFT, GRIND_SLOW_IDLE_LOOKRIGHT, GRIND_SLOW_IDLE_REACHLEFT, GRIND_SLOW_IDLE_REACHLEFTDOWN, GRIND_SLOW_IDLE_REACHLEFTUP,
		GRIND_SLOW_IDLE_REACHRIGHT, GRIND_SLOW_IDLE_REACHRIGHTDOWN, GRIND_SLOW_IDLE_REACHRIGHTUP, GRIND_SLOW_IDLE_RIGHT,
		GRIND_TRANSFER_LANDLEFT, GRIND_TRANSFER_LANDRIGHT, GRIND_TRANSFER_LEFT, GRIND_TRANSFER_RIGHT, GRIND_TURNAROUND_BOOST, GRIND_TURNAROUND_SLOWDOWN,
		GRIND_UPSIDEDOWN_ENTER, GRIND_UPSIDEDOWN_EXIT, GRIND_UPSIDEDOWN_FAST_IDLE, GRIND_UPSIDEDOWN_FAST_IDLE_LEFT, GRIND_UPSIDEDOWN_FAST_IDLE_RIGHT,
		GRIND_UPSIDEDOWN_SLOW_IDLE, GRIND_UPSIDEDOWN_SLOW_IDLE_LEFT, GRIND_UPSIDEDOWN_SLOW_IDLE_RIGHT,

		/* For.Crouch Actions (웅크리기) */
		CROUCH_ENTER_GROUNDPOUND, CROUCH_ENTER_LANDING, CROUCH_ENTER_IDLE, CROUCH_ENTER_SLIDE, CROUCH_ENTER_SLIDEMOVEMENT, CROUCH_ENTER_WALK,
		CROUCH_EXIT_IDLE, CROUCH_EXIT_MOVEMENT, CROUCH_EXIT_MOVEMENT_WALK,
		CROUCH_FWD, CROUCH_IDLE, CROUCH_START, CROUCH_STOP,

		/* For.etc (순서 이상하게 들어간 친구들) */
		JUMP_WALLCLIMB, JUMP_WALLCLIMB_SLIDEDOWN, ACTIONIDLE, ACTIONIDLE_TO_IDLE, EXHAUSTED_IDLE, EXHAUSTED_IDLE_TO_IDLE,
		IDLE_TO_ACTION, IDLE_TO_EXHAUSTED,

		/* For.Jog Actions (CapsLock 안누를 때 달리기) */
		JOG, JOG_ACTION_IDLE_START_FWD, JOG_BANK_LEFT, JOG_BANK_RIGHT,
		JOG_EXHAUSTED_IDLE_START_FWD, JOG_GOTOSTOP, JOG_LOOKBACKLEFT, JOG_LOOKBACKRIGHT, JOG_START_FWD, JOG_START_FWD_LEFT_180, JOG_START_FWD_RIGHT_180,
		JOG_START_INMOTION, JOG_START_INMOTION_VAR2, JOG_STOP_FWD, JOG_STOP_FWD_ACTION, JOG_STOP_FWD_EASY, JOG_STOP_FWD_EXHAUSTED,
		JOG_STRAFE_BWD, JOG_STRAFE_FWD, JOG_STRAFE_LEFT, JOG_STRAFE_RIGHT,

		/* For.Sprint Actions (CapsLock 눌렀을 때 달리기) */
		SPRINT, SPRINT_LEFT, SPRINT_RIGHT, SPRINT_START_FROMDASH, SPRINTTURNAROUND,
		STRAFE_BWD, STRAFE_BWDL, STRAFE_BWDR, STRAFE_FWD, STRAFE_FWDL, STRAFE_FWDR, STRAFE_LEFT, STRAFE_IDLE, STRAFE_RIGHT, STRAFE_TURNINPLACE_LEFT, STRAFE_TURNINPLACE_RIGHT,

		/* For.Walk Actions (걷기가 있나.) */
		WALK_ACTION_IDLE_START_FWD, WALK_BANK_LEFT, WALK_BANK_RIGHT, WALK_EXHAUSTED_IDLE_START_FWD, WALK_FWD, WALK_START_FWD, WALK_START_LEFT_180, WALK_START_RIGHT_180, WALK_START_INMOTION, WALK_START_INMOTION_VAR2,
		WALK_LOCOMOTION_WALK,

		/* For.Bounce Actions (점프 발판(쿠션) 밟을 때) */
		BHV_BOUNCE1, BHV_BOUNCE2, BHV_BOUNCE3, BHV_BOUNCE4, BHV_PICKUPSMALL_BOUNCE1, BHV_PICKUPSMALL_BOUNCE2, BHV_PICKUPSMALL_BOUNCE3, BHV_PICKUPSMALL_BOUNCE4,

		/* For.Bump Collide Actions (충돌했을때?) */
		BHV_BUMP_COLLIDE, BHV_BUMP_COLLIDE_FALL,

		/* For.Death Actions (사망) */
		BHV_DEATH_FALL_IDLE, BHV_DEATH_OIL_DROWN,

		/* For.GroundPound Actions (점프 후 내려찍기) */
		BHV_GROUNDPOUND_FALLING, BHV_GROUNDPOUND_JUMP_HIGH, BHV_GROUNDPOUND_JUMP_LOW,
		BHV_GROUNDPOUND_LAND, BHV_GROUNDPOUND_LAND_EXIT, BHV_GROUNDPOUND_LAND_IDLE, BHV_GROUNDPOUND_START,

		/* For.KnockDown Actions (넉백) */
		BHV_KNOCKDOWN_BACK_LAND, BHV_KNOCKDOWN_BACK_IDLE, BHV_KNOCKDOWN_BACK_START, BHV_KNOCKDOWN_FRONT, BHV_KNOCKDOWN_FRONT_LAND, BHV_KNOCKDOWN_FRONT_IDLE, BHV_KNOCKDOWN_FRONT_START,
		BHV_KNOCKDOWN_LEFT_LAND, BHV_KNOCKDOWN_LEFT_IDLE, BHV_KNOCKDOWN_LEFT_START, BHV_KNOCKDOWN_RIGHT_LAND, BHV_KNOCKDOWN_RIGHT_IDLE, BHV_KNOCKDOWN_RIGHT_START,

		/* For.Lever Actions (좌우 레버(배터리 넣는 로봇)) */
		BHV_LEVER_LEFT, BHV_LEVER_RIGHT,

		/* For.MiniGame Emote Actions (이모트 액션들 ㅇㅇ) */
		BHV_MINIGAME_EMOTES_ASSDANCE, BHV_MINIGAME_EMOTES_COSSACKDANCE, BHV_MINIGAME_EMOTES_EXPLOSION, BHV_MINIGAME_EMOTES_FISTPUMP, BHV_MINIGAME_EMOTES_FONZIE, BHV_MINIGAME_EMOTES_HAPPYDANCE, BHV_MINIGAME_EMOTES_HUZZAH, BHV_MINIGAME_EMOTES_JUMPUPLAND, BHV_MINIGAME_EMOTES_KARATEBOW, BHV_MINIGAME_EMOTES_LEAFFLEX,
		BHV_MINIGAME_EMOTES_LOSE_VAR1, BHV_MINIGAME_EMOTES_LOSE_VAR2, BHV_MINIGAME_EMOTES_LOSE_VAR3, BHV_MINIGAME_EMOTES_SHOULDERBRUSH, BHV_MINIGAME_EMOTES_WATCHINGYOU, BHV_MINIGAME_EMOTES_WEIRDROBOT,

		/* For.PickUp Actions (등에 짊어지거나 손에 들고있을 때 행동들) */
		BHV_PICKUP, BHV_PICKUP_AIM_STRAFE_BWD, BHV_PICKUP_AIM_STRAFE_FWD, BHV_PICKUP_AIM_STRAFE_LEFT, BHV_PICKUP_AIM_STRAFE_RIGHT,
		BHV_PICKUP_AIMSPACE_CENTER, BHV_PICKUP_IDLE, BHV_PICKUP_AIMSPACE_DOWN, BHV_PICKUP_AIMSPACE_UP, BHV_PICKUP_FUSECRAB_PUTINSOCKET,
		BHV_PICKUP_JOG, BHV_PICKUP_JOG_STOP, BHV_PICKUP_THROW, BHV_PICKUP_JUMP_FALLING, BHV_PICKUP_JUMP_LAND, BHV_PICKUP_JUMP_LAND_JOG,
		BHV_JUMP_START,
		BHV_PICKUP_HEAVYSMALL_JOG, BHV_PICKUP_HEAVYSMALL_JOG_STARTINMOTION, BHV_PICKUP_HEAVYSMALL_JOG_STOP, BHV_PICKUP_HEAVYSMALL_IDLE, BHV_PICKUP_HEAVYSMALL_PICKUP, BHV_PICKUP_HEAVYSMALL_PUTDOWN, BHV_PICKUP_HEAVYSMALL_PUTDOWN_INPLACE,
		BHV_PICKUP_HEAVYSMALL_JUMP_FALLING, BHV_PICKUP_HEAVYSMALL_JUMP_LAND, BHV_PICKUP_HEAVYSMALL_JUMP_LAND_JOG, BHV_PICKUP_HEAVYSMALL_JUMP_START,
		BHV_PICKUPSMALL, BHV_PICKUPSMALL_AIM_IDLE, BHV_PICKUPSMALL_AIM_STRAFE_FWD, BHV_PICKUPSMALL_AIMSPACE_DOWN, BHV_PICKUPSMALL_AIMSPACE_UP,
		BHV_PICKUPSMALL_BHV_GROUNDPOUND_FALLING, BHV_PICKUPSMALL_BHV_GROUNDPOUND_JUMP_HIGH, BHV_PICKUPSMALL_BHV_GROUNDPOUND_JUMP_LOW, BHV_PICKUPSMALL_BHV_GROUND_POUND_LAND,
		BHV_PICKUPSMALL_BHV_GROUNDPOUND_LAND_EXIT, BHV_PICKUPSMALL_BHV_GROUNDPOUND_LAND_IDLE, BHV_PICKUPSMALL_BHV_START,

		BHV_PICKUP_SMALL_BHV_SWINGING_BCK, BHV_PICKUP_SMALL_BHV_SWINGING_BCK_BCK, BHV_PICKUP_SMALL_BHV_SWINGING_BCK_FWD, BHV_PICKUP_SMALL_BHV_SWINGING_BCK_NEUTRAL, BHV_PICKUP_SMALL_BHV_SWINGING_BCK_TURNAROUND,
		BHV_PICKUP_SMALL_BHV_SWINGING_EXITBCK, BHV_PICKUP_SMALL_BHV_SWINGING_EXITFWD, BHV_PICKUP_SMALL_BHV_SWINGING_FWD, BHV_PICKUP_SMALL_BHV_SWINGING_JUMPBCK, BHV_PICKUP_SMALL_BHV_SWINGING_JUMPFWD, BHV_PICKUP_SMALL_BHV_SWINGING_NEUTRAL, BHV_PICKUP_SMALL_BHV_SWINGING_TURNAROUND,
		BHV_PICKUP_SMALL_JOG, BHV_PICKUP_SMALL_JOG_GOTOSTOP, BHV_PICKUP_SMALL_JOG_START_FWD, BHV_PICKUP_SMALL_JOG_START_INMOTION, BHV_PICKUP_SMALL_JOG_STOP_FWD,
		BHV_PICKUP_SMALL_SPRINT, BHV_PICKUP_SMALL_SPRINTSTARTFROMDASH, BHV_PICKUP_SMALL_SPRINTTURNAROUND, BHV_PICKUP_SMALL_IDLE, BHV_PICKUP_SMALL_PUTDOWN, BHV_PICKUP_SMALL_INPLACE,
		BHV_PICKUP_SMALL_STRAFE_AIM_SHUFFLE_LEFT, BHV_PICKUP_SMALL_STRAFE_AIM_SHUFFLE_RIGHT, BHV_PICKUP_SMALL_STRAFE_AIMSPACE_CHARGE_DOWN, BHV_PICKUP_SMALL_STRAFE_IDLE, BHV_PICKUP_SMALL_STRAFE_AIMSPACE_CHARGE_UP,
		BHV_PICKUP_SMALL_STRAFE_BWD, BHV_PICKUP_SMALL_STRAFE_BWD_L, BHV_PICKUP_SMALL_STRAFE_BWD_R, BHV_PICKUP_SMALL_STRAFE_FWD, BHV_PICKUP_SMALL_STRAFE_FWD_L, BHV_PICKUP_SMALL_STRAFE_FWD_R,
		BHV_PICKUP_SMALL_STRAFE_LEFT, BHV_PICKUP_SMALL_STRAFE_RIGHT, BHV_PICKUP_SMALL_THROW, BHV_PICKUP_SMALL_AIRDASHSTART, BHV_PICKUP_SMALL_DOUBLEJUMP,
		BHV_PICKUP_SMALL_GRIND_DASH, BHV_PICKUP_SMALL_GRIND_FAST, BHV_PICKUP_SMALL_GRIND_FAST_IDLE_LEFT, BHV_PICKUP_SMALL_GRIND_FAST_IDLE_RIGHT, BHV_PICKUP_SMALL_GRIND_GRAPPLE_ENTER, BHV_PICKUP_SMALL_GRIND_GRAPPLE_TOGRIND,
		BHV_PICKUP_SMALL_GRIND_JUMP, BHV_PICKUP_SMALL_GRIND_LAND, BHV_PICKUP_SMALL_GRIND_SLOW_IDLE, BHV_PICKUP_SMALL_GRIND_SLOW_IDLE_LEFT, BHV_PICKUP_SMALL_GRIND_SLOW_IDLE_RIGHT,
		BHV_PICKUP_SMALL_GRIND_TURNAROUND_BOOST, BHV_PICKUP_SMALL_GRIND_TURNAROUND_SLOWDOWN,
		BHV_PICKUPSMALL_JUMP_FALLING, BHV_PICKUPSMALL_JUMP_FROM_LANDING, BHV_PICKUPSMALL_JUMP_LAND, BHV_PICKUPSMALL_JUMP_LAND_JOG, BHV_PICKUPSMALL_JUMP_START, BHV_PICKUPSMALL_JUMP_STARTFROMSTILL,
		BHV_PICKUPSMALL_LEDGEGRAB_CLIMBUP, BHV_PICKUPSMALL_LEDGEGRAB_DROP, BHV_PICKUPSMALL_LEDGEGRAB_JUMPBACK_LEFT, BHV_PICKUPSMALL_LEDGEGRAB_JUMPUP, BHV_PICKUPSMALL_LEDGEGRAB_IDLE, BHV_PICKUPSMALL_LEDGEGRAB_IDLE_LEFT120, BHV_PICKUPSMALL_LEDGEGRAB_IDLE_LEFT180, BHV_PICKUPSMALL_LEDGEGRAB_IDLE_LEFT30, BHV_PICKUPSMALL_LEDGEGRAB_IDLE_LEFT60, BHV_PICKUPSMALL_LEDGEGRAB_IDLE_LEFT90,
		BHV_PICKUPSMALL_LEDGEGRAB_START, BHV_PICKUPSMALL_LEDGEVAULT, BHV_PICKUPSMALL_LEDGEVAULTSTART, BHV_PICKUPSMALL_PERFECTDASH, BHV_PICKUPSMALL_PERFECTDASHJOG, BHV_PICKUPSMALL_PERFECTDASHSTOP, BHV_PICKUPSMALL_ROLL_START, BHV_PICKUPSMALL_ROLL_STOP, BHV_PICKUPSMALL_ROLL_TO_JOG,
		BHV_PICKUPSMALL_WALLSLIDE_ENTER, BHV_PICKUPSMALL_WALLSLIDE_ENTER_FROMDASH, BHV_PICKUPSMALL_WALLSLIDE_ENTER_NOTJUMPABLE, BHV_PICKUPSMALL_WALLSLIDE_JUMP, BHV_PICKUPSMALL_WALLSLIDE_IDLE,

		/* For.PutDown Actions (내려놓기) */
		BHV_PUTDOWN, BHV_PUTDOWN_INPLACE,

		/* For. Pull/Push Actions */
		BHV_PULL, BHV_PUSH_ENTER, BHV_PUSH_EXIT, BHV_PUSH_FWD, BHV_PUSH_IDLE, BHV_PUSH_STRUGGLE,
		BHV_HOLDBUTTON_ENTER, BHV_HOLDBUTTON_EXIT, BHV_HOLDBUTTON_IDLE, BHV_PUSHBUTTON_VAR1, BHV_PUSHBUTTON_VAR2, BHV_PUSHBUTTON_VAR2_SAPGUN,

		/* For.Skydive Actions (낙하) */
		BHV_SKYDIVE_BWD, BHV_SKYDIVE_FWD, BHV_SKYDIVE_LEFT, BHV_SKYDIVE_IDLE, BHV_SKYDIVE_RIGHT, BHV_SKYDIVE_START,

		/* For.Swinging Actions (스윙) */
		BHV_SWINGING_BCK, BHV_SWINGING_BCK_BCK, BHV_SWINGING_BCK_FWD, BHV_SWINGING_BCK_LEFT, BHV_SWINGING_BCK_NEUTRAL, BHV_SWINGING_BCK_RIGHT, BHV_SWINGING_BCK_TURNAROUND,
		BHV_SWINGING_ENTER, BHV_SWINGING_EXITBCK, BHV_SWINGING_EXITFWD, BHV_SWINGING_FWD, BHV_SWINGING_JUMPBCK, BHV_SWINGING_JUMPFWD, BHV_SWINGING_LEFT, BHV_SWINGING_NEUTRAL, BHV_SWINGING_RIGHT, BHV_SWINGING_TURNAROUND,

		/* For.ChangeSize Actions (덩치 커졌을 때?) */
		CHANGESIZE_GROUNDPOUND_FALLING, CHANGESIZE_GROUNDPOUND_LAND_EXIT, CHANGESIZE_GROUNDPOUND_LAND_IDLE, CHANGESIZE_GROUNDPOUND_START,
		CHAGESIZE_PLANETPUSH_LARGE, CHAGESIZE_PLANETPUSH_MEDIUM, CHAGESIZE_PLANETPUSH_SMALL,
		CHANGESIZE_WALK_LARGE_BANK_LEFT, CHANGESIZE_WALK_LARGE_BANK_RIGHT, CHANGESIZE_WALK_LARGE_FWD, CHANGESIZE_WALK_LARGE_START, CHANGESIZE_WALK_LARGE_STOP,
		CHANGESIZE_JUMP_FALLING, CHANGESIZE_JUMP_LARGE_LAND, CHANGESIZE_JUMP_LARGE_LAND_JOG, CHANGESIZE_JUMP_START, CHANGESIZE_ROLL_START,

		/* For.FidgetSpinner Actions (피젯스피너 안쓸듯?) */
		FIDGETSPINNER_ENTER, FIDGETSPINNER_EXIT, FIDGETSPINNER_HOVER_FAST_IDLE, FIDGETSPINNER_HOVER_FAST_IDLE_LEFT, FIDGETSPINNER_HOVER_FAST_IDLE_RIGHT,
		FIDGETSPINNER_HOVER_SLOW_IDLE, FIDGETSPINNER_HOVER_SLOW_IDLE_LEFT, FIDGETSPINNER_HOVER_SLOW_IDLE_RIGHT,
		FIDGETSPINNER_INAIR_ENTER, FIDGETSPINNER_INAIR_EXIT,

		/* For.PlayRoom_Mage Actions (안쓸듯ㅇㅇ) */
		LAVALAMPBUTTON, MAGECOMBAT_ATTACK_1, MAGECOMBAT_ATTACK_2, MAGECOMBAT_ATTACK_3, MAGECOMBAT_ATTACK_4, MAGECOMBAT_ATTACK_5, MAGECOMBAT_ATTACK_6, MAGECOMBAT_ATTACK_7, MAGECOMBAT_ATTACK_8, MAGECOMBAT_ATTACK_9, MAGECOMBAT_ATTACK_10,
		MAGECOMBAT_BEAMATTACK, MAGECOMBAT_BLINK_EXIT, MAGECOMBAT_IDLE, MAGECOMBAT_ORBATTACK, MAGECOMBAT_DAMAGED1, MAGECOMBAT_DAMAGED2, MAGECOMBAT_DAMAGED3, MAGECOMBAT_JOG, MAGECOMBAT_JOG_START_FWD, MAGECOMBAT_JOG_STOP_FWD,

		/* For.PlasmaBall Actions (플라즈마공 안할 듯?)*/
		PLASAMABALL_ENTER, PLASAMABALL_EXIT, PLASAMABALL_IDLE, PLASAMABALL_BWD, PLASAMABALL_BWD_LEFT, PLASAMABALL_BWD_RIGHT, PLASAMABALL_FWD, PLASAMABALL_FWD_LEFT, PLASAMABALL_FWD_RIGHT,
		PLASAMABALL_LEFT, PLASAMABALL_RIGHT,

		/* For.Battery Actions (무조건 써야함 ㅇㅇ) */
		PUSH_BATTERY_BCK, PUSH_BATTERY_FWD, PUSH_BATTERY_IDLE,

		/* 그외의 다양한 상호작용 상태들 */
		SPINNINGSPACELAMP,
		ZEROGRAVITY_ADDITIVEROTATION_FWD, ZEROGRAVITY_ADDITIVEROTATION_RIGHT, ZEROGRAVITY_BACK, ZEROGRAVITY_FWD, ZEROGRAVITY_LEFT, ZEROGRAVITY_IDLE, ZEROGRAVITY_RIGHT,
		SHED_VACUUMLAUNCH_ENTER, SHED_VACUUMLAUNCH_IDLE,

		/* 밸브 돌리기 */
		VALVE_ROTATE_IDLE, VALVE_ROTATE_RIGHT, VALVE_ROTATE_RIGHT_SHORT, VALVE_STRUGGLELEFT, VALVE_STRUGGLELEFT_IDLE, VALVE_STRUGGLELEFTENTER,
		VALVE_STRUGGLERIGHT, VALVE_STRUGGLERIGHT_IDLE, VALVE_STRUGGLERIGHTENTER,

		/* 짜잘한 이동 상태들 */
		GENERIC_WALKIN, CHESSMINIGAME_GESTURES_POINT, DASHMOVETO_START, DASHMOVETO_START_INAIR, DASHMOVETO_START_LEFT90, DASHMOVETO_START_RIGHT90,
		IDLE_GESTURE_BIG_WAITING_ENTER, IDLE_GESTURE_BIG_WAITING_IDLE, IDLE_GESTURE_SMALL_ADJUSTLEAF, IDLE_GESTURE_SMALL_BELLY, IDLE_GESTURE_SMALL_DRUMMING, IDLE_GESTURE_SMALL_INSPECTNAILS,
		IDLE_GESTURE_SMALL_LOOKAROUND, IDLE_GESTURE_SMALL_SCRATCH, IDLE_GESTURE_SMALL_STRETCH, IDLE_GESTURE_SMALL_TOSSLEAF,

		/* 레버 당기는 상태 */
		LEVERPULL_EXIT_FAST, LEVERPULL_EXIT_SLOW, LEVERPULL_IDLE, LEVERPULL_START_FAST,

		/* 아케이드 미니게임 (안쓸듯) */
		ACRADESCREENLEVER_BCK, ACRADESCREENLEVER_BCKLEFT, ACRADESCREENLEVER_BCKRIGHT, ACRADESCREENLEVER_FWD, ACRADESCREENLEVER_FWDLEFT, ACRADESCREENLEVER_FWDRIGHT,
		ACRADESCREENLEVER_LEFT, ACRADESCREENLEVER_IDLE, ACRADESCREENLEVER_RIGHT, ACRADESCREENLEVER_TRIGGER,

		/* 로켓 날리기 ㅋ */
		ROCKETFIREWORK,

		/* 보스 컷씬들 */
		CUTSCENE_BOSSFIGHT_BEAMOUT, CUTSCENE_BOSSFIGHT_EJECT, CUTSCENE_BOSSFIGHT_ENTERUFO, CUTSCENE_BOSSFIGHT_INTRO, CUTSCENE_BOSSFIGHT_LANDINSIDE_UFO,
		CUTSCENE_BOSSFIGHT_OUTRO, CUTSCENE_BOSSFIGHT_LASER_RIPPED_OFF, CUTSCENE_HUB_SECOND_GENERATOR,


	};
#pragma endregion

public:
	enum PLAYER_SIZE { SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE, SIZE_END };
	enum CAMERA_WORK_STATE { STATE_SPACE_PORTAL, STATE_DUMMYWALL_JUMP, STATE_PIPEWALL_JUMP, STATE_END};

private:
	explicit CCody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCody(const CCody& rhs);
	virtual ~CCody() = default;

	// 부모로 부터 상속
public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;

public:
	/* Getter */
	CTransform*		 Get_Transform() { return m_pTransformCom; }
	CModel*			 Get_Model() { return m_pModelCom; }
	CPlayerActor*	 Get_Actor() { return m_pActorCom; }
	PLAYER_SIZE		 Get_Player_Size() { return m_eCurPlayerSize; }
	_bool			 Get_IsInGravityPipe() { return m_IsInGravityPipe; }
	_bool			 Get_IsInRocket() { return m_IsBossMissile_Control; }
	_bool			 Get_PushingBattery() { return m_IsPushingBattery; }
	_bool			 Get_ControlRoomPushingBattery() { return m_IsPushingControlRoomBattery; }
	_uint			 Get_CurState() const;
	_bool			 Get_IsPlayerInUFO();
	_float4x4		 Get_TriggerTargetWorld() { return m_TriggerTargetWorld; }
	_bool			 Get_IsWarpNextStage() { return m_IsWarpNextStage; }
	_bool			 Get_IsWarpDone() { return m_IsWarpDone; }
	_float			 Get_Acceleration() { return m_fAcceleration; }
	_bool			 Get_IsRoll() { return m_bRoll; }
	_bool			 Get_IsAirDash() { return m_IsAirDash; }

	_vector			 Get_TriggerTargetPos() { return XMLoadFloat3(&m_vTriggerTargetPos); }
	_vector			 Get_UFOTargetPos() { return m_vHookUFOOffsetPos; }
	_vector			 Get_CamTriggerPos() { return XMLoadFloat4(&m_vCamTriggerPos); }
	_bool			 Get_IsHooking() { return m_IsHookUFO; }
	_bool			 Get_IsInArcadeJoyStick() { return m_IsInJoyStick; }

	_bool			 Get_IsControllJoyStick() { return m_IsControlJoystick; }
	_bool			 Get_IsPinBall() { return m_IsPinBall; }
	_bool			 Get_IsWallJump() { return m_IsCamTriggerCollide; }
	_bool			 Get_OnRail() { return m_bOnRail; }

	_bool			 Get_IsRespawn() { return m_bRespawn; }
	_bool			 Get_IsDeadLine() { return m_IsDeadLine; }
	_bool			 Get_RespawnCheck() { return m_bRespawnCheck; }
	_bool			 Get_IsLaserTennis() { return m_bLaserTennis; }

	_bool			 Get_IsEnding() { return m_IsEnding; }
	_bool			Get_IsDeadInBossroom() { return m_bDead_InBossroom; }

	_bool			 Get_InterActiveUICreate() { return m_IsInterActiveUICreate; }

public:
	void			 Set_PushingBattery() { m_IsPushingBattery = false; }
	void			 Set_OnParentRotate(_matrix ParentMatrix);
	void			 Set_ControlJoystick(_bool IsCheck);
	void			 Set_AnimationRotate(_float fAngle);
	void			 Set_ActiveHpBar(_bool IsCheck);
	void			 Set_HpBarReduction(_float fDamage);
	void			 Set_ActiveMinigameHpBar(_bool IsCheck);
	void			 Set_MinigameHpBarReduction(_float fDamage);
	void			 Set_MinigameHpBarReset();
	void 			 Set_RocketOffSetPos(_fvector vRocketOffSetPos) { m_vRocketOffSetPos = vRocketOffSetPos; }
	void			 Set_RocketMatrix(_matrix matRocket) { m_matRocketMatrix = matRocket; }
	void			 Set_Escape_From_Rocket(_bool bEscape) { m_bEscapeFromRocket = bEscape; }
	void			 Set_Change_Size_After_UmbrellaCutScene();
	void			 Set_InJoyStick();

	/* For. Ending */
	void			Set_EndingRocketOffSetPos(_fvector vRocketOffSetPos) { m_vEndingRocketOffSetPos = vRocketOffSetPos; }
	void			Set_EndingRocketMatrix(_matrix matRocket) { m_matEndingRocketMatrix = matRocket; }


	// Tick 에서 호출될 함수들
private:
	virtual void KeyInput(_double dTimeDelta);

public: // 여기에 넣어놓아야 알거 같아서 여기에..		
	void Enforce_IdleState(); /* 강제로 Idle 상태로 바꿈 */

private:
	HRESULT Ready_Component();
	HRESULT Ready_UI();
	void Add_LerpInfo_To_Model();

private: // Effects
	class CEffect_Cody_Size* m_pEffect_Size = nullptr;

private: /* UI */
	class CHpBar*	m_pHpBar = nullptr;
	class CHpBar*	m_pSubHpBar = nullptr;
	class CMinigameHpBar* m_pMinigameHpBar = nullptr;
	class CMinigameHpBar* m_pMinigameSubHpBar = nullptr;

	// Components
private:
	class CMainCamera*	m_pCamera = nullptr;

	// UI
private:
	class CGauge_Circle*	m_pGauge_Circle = nullptr;

	// 생성 및 소멸 관련
public:
	static CCody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////    상태 변환 관련 변수들   /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	// 상태 && 이동
	void Move(const _double dTimeDelta);
	void Roll(const _double dTimeDelta);
	void Sprint(const _double dTimeDelta);
	void Jump(const _double dTimeDelta);
	void Change_Size(const _double dTimeDelta);
	void Ground_Pound(const _double dTimeDelta);

#pragma region BasicMovement
private:
	// 기본 움직임
	_bool m_bSprint			= false;
	_bool m_bRoll			= false;
	_bool m_bMove			= false;
	_bool m_bShortJump		= false;
	_bool m_bGroundPound	= false;
	_bool m_IsTurnAround	= false;

	_uint	m_iIndex = 0;

	// 구르기 관련
	_bool m_bAction = false;

	// 점프/떨어지는 중이니
	_bool m_IsJumping = false;
	_bool m_IsAirDash = false;
	_bool m_IsFalling = false;
	_bool m_bFallAniOnce = false;

	// 점프/에어대쉬 관련 변수
	_uint m_iJumpCount = 0;
	_uint m_iAirDashCount = 0;

	// 이동방향 및 이전키입력저장
	_float3 m_vMoveDirection = {};
	_int m_iSavedKeyPress = 0;

	// 움직임 가속
	_float m_fAcceleration = 5.0;
	_float m_fJogAcceleration = 25.f;
	_float m_fSprintAcceleration = 35.f;

	// GroundPound 관련
	_bool m_bPlayGroundPoundOnce = false;
	_bool m_bCanMove = true;
	_bool m_bAfterGroundPound = false;
	_uint m_iAfterGroundPoundCount = 0;
	_float m_fGroundPoundAirDelay = 0.f; // 체공시간.

	// IDLE 상태 길어지면 대기 상태 애니메이션 딜레이.
	_float	m_fIdleTime = 0.f;

	// 뭔가 들고있다면
	_bool m_IsPickUp = false;

	// 크기가 달라졌다면
	PLAYER_SIZE m_eCurPlayerSize = SIZE_MEDIUM;
	PLAYER_SIZE m_eNextPlayerSize = SIZE_MEDIUM;
	_float3 m_vScale = {1.f, 1.f, 1.f};
	_bool m_IsSizeChanging = false;
	_float m_fSizeDelayTime = 0.f;
	_bool m_bChangeSizeEffectOnce = false;
	
	// 컷씬이라면
	_bool m_IsCutScene = false;

	// 스테이지 클리어 확인
	_bool m_IsStGravityCleared = false;
	_bool m_IsStRailCleared = false;
	_bool m_IsStPinBallCleared = false;
#pragma endregion

#pragma region Trigger
	/* Getter */
public:
	CAMERA_WORK_STATE Get_CameraWorkState() { return m_eCameraWorkState; }
	_matrix Get_CameraTrigger_Matrix() { return XMLoadFloat4x4(&m_TriggerCameraWorld); }
	_bool Get_WarpEnd_CountDown(); // 포탈 종료 1초전 return

	/* Setter */
public:
	void SetTriggerID(GameID::Enum eID, _bool IsCollide, _fvector vTriggerTargetPos, _uint _iPlayerName = 0);
	void SetTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerTargetWorld, _uint _iPlayerName = 0);
	void SetTriggerID_Ptr(GameID::Enum eID, _bool IsCollide, CGameObject* pTargetPtr);
	void SetCameraTriggerID_Matrix(GameID::Enum eID, _bool IsCollide, _fmatrix vTriggerCameraWorld);
	void SetCameraTriggerID_Pos(_fvector vCamTriggerPos);
private:
	// CameraTrigger 
	CAMERA_WORK_STATE m_eCameraWorkState = STATE_END;
	GameID::Enum m_eCameraTriggerID = GameID::Enum::eWORMHOLE;
	_bool m_IsCamTriggerCollide = false;
	_float4x4 m_TriggerCameraWorld = {};
	_float4 m_vCamTriggerPos = {};


	// NormalTrigger
	GameID::Enum		m_eTargetGameID = GameID::Enum::eMAY;
	_float3				m_vTriggerTargetPos = {};
	_float4x4			m_TriggerTargetWorld = {};
	CGameObject*		m_pTargetPtr = nullptr;
	_uint				m_iCurrentStageNum = ST_GRAVITYPATH;


	// Else
	_bool m_IsCollide = false;
	_bool m_IsOnGrind = false;
	_bool m_IsHitStarBuddy = false;
	_bool m_IsHitRocket = false;
	_bool m_IsActivateRobotLever = false;

	/* For.PushBattery*/
	_bool m_IsPushingBattery = false;
	_float m_fPushDist = 0.f;
	_float m_fPushBatteryDelay = 0.f;
	_bool m_IsPushingControlRoomBattery = false;

	/* Hye::For.DeadLine, SavePoint */
	_bool	 m_IsDeadLine = false;
	_float3  m_vSavePoint = {};
	_double	 m_dDeadTime = 0.f;
	_float3	 m_DeadLinePos = {};
	/* Hye::For.PinBall*/
	_bool	 m_IsPinBall = false;
	_bool	 m_IsReadyPinball = false;
	_bool	 m_bPinBallScript = false;
	/* Hye::For.Tube*/
	_bool	 m_IsTube = false;
	/* Hye::For.SpaceShip */
	_bool	 m_bRespawn = false;
	_bool    m_bFirstCheck = false;
	_double	 m_dRespawnTime = 0.0;
	/* Hye::For.LaserTennis */
	_bool	 m_bLaserTennis = false;
	/* Hye::For.Laser_LaserTennis */
	_bool	 m_bLaser = false;
	_bool	 m_bCheckAnim = false;
	_uint	 m_iHP = 12;
	/* Hye::For.EndingCredit */
	_bool	m_bEndingCredit = false;
	_double m_dStartTime = 0.0;
	_bool	m_bEndingCheck = false;

	/* Hye::For.RespawnCheck */
	/* 죽었을 때 False로 바뀌고 리스폰시에 True로 바뀜 */
	_bool	m_bRespawnCheck = false;

	/* Hye::For.PlayerHit */
	/* m_bHit를 True로 하면 일정시간동안 플레이어가 빨개집니다. */
	_bool	m_bHit = false;
	_double m_dHitTime = 0.0;

	/* For.GravityTunnel */
	_bool m_bGoToGravityCenter = false;
	_bool m_IsInGravityPipe = false;
	_float m_fGoCenterTime = 0.f;

	/* For.Pipe*/
	_bool m_IsPipeBattery = false;

	/* For.Valve */
	_bool m_IsEnterValve = false;
	_bool m_bStruggle = false;
	_uint m_iRotateCount = 0;
	_uint m_iValvePlayerName = Player::Cody;

	/* For.Planet */
	_bool m_IsHitPlanet = false;
	_bool m_IsHitPlanet_Effect = false;
	_bool m_IsHitPlanet_Once = false;

	/* For.HookUFO */
	_bool m_IsHookUFO = false;
	_vector m_vHookUFOAxis = {};
	_bool m_bGoToHooker = false;
	_vector m_vHookUFOOffsetPos = {};
	_float m_faArmLength = 0.f;
	_float m_faVelocity = 0.f;
	_float m_faAcceleration = 0.f;
	_float m_fRopeAngle = 0.f;
	_float3 m_vStartPosition = {};
	_float3 m_vDstPosition = {};

	/* For.Umbrella */
	_bool m_IsControlJoystick = false;

	// Arbitrary damping
	_float m_faDamping = 0.995f;

	// Arbitrary ball radius
	_float3 m_vPoints[4] = {};
	_double	m_dTestTime = 0.0;

	/* For. WallJump */
	_bool	m_bWallAttach = false;
	_bool   m_IsWallJumping = false;
	_float	m_fWallJumpingTime = 0.f;
	_float	m_fWallToWallSpeed = 0.55f;

	/* For. WallJump */
	_bool	m_bElectricWallAttach = false;
	_bool   m_IsElectricWallJumping = false;
	_float	m_fElectricWallJumpingTime = 0.f;
	_float	m_fElectricWallToWallSpeed = 45.f;

	/* For. WallJump */
	_bool	m_bPipeWallAttach = false;
	_bool   m_IsPipeWallJumping = false;
	_float	m_fPipeWallJumpingTime = 0.f;
	_float	m_fPipeWallToWallSpeed = 45.f;

	// Warp NextStage
	_bool m_IsWarpNextStage = false;
	_float m_fWarpTimer = 0.f;
	_bool m_IsWarpDone = false;
	_bool m_IsWarpRotate = false;
	const _float4 m_vWormholePos = { 0.f, -100.f, -1000.f, 1.f };
	const _float m_fWarpTimer_Max = 3.9f;
	const _float m_fWarpTimer_InWormhole = 0.25f;

	// fire Door Dead
	_bool m_IsTouchFireDoor = false;

	// touch WallLaserTrap
	_bool m_IsWallLaserTrap_Touch = false;
	_bool m_IsWallLaserTrap_Effect = false;
 
	/* For. BossUFO */
	_bool m_IsHolding_UFO = false;
	_bool m_IsHolding_Low_UFO = false;
	_bool m_IsHolding_High_UFO = false;
	_uint m_iKeyDownCount = 0;
	_bool m_IsCodyEnter = false;
	_bool m_IsInterActiveUICreate = false;

	/* For.Boss Missile */
	_bool	m_IsBossMissile_Control = false;
	_bool	m_IsMoveToRocket = false;
	_bool   m_bEscapeFromRocket = false;
	_vector m_vRocketOffSetPos = {};
	_matrix m_matRocketMatrix = {};
	_bool	m_bLandHigh = false;


	// YYY
	void Hit_StarBuddy(const _double dTimeDelta);
	void Hit_Rocket(const _double dTimeDelta);
	void Activate_RobotLever(const _double dTimeDelta);
	void Push_Battery(const _double dTimeDelta);
	void Rotate_Valve(const _double dTimeDelta);
	void In_GravityPipe(const _double dTimeDelta);
	void Hit_Planet(const _double dTimeDelta);
	void Hook_UFO(const _double dTimeDelta);
	void Wall_Jump(const _double dTimeDelta);
	void Pipe_WallJump(const _double dTimeDelta);
	void ElectricWallJump(const _double dTimeDelta);
	void BossMissile_Control(const _double dTimeDelta);
	void Ride_Ending_Rocket(const _double dTimeDelta);

	// 정호
	void Warp_Wormhole(const _double dTimeDelta);
	void Touch_FireDoor(const _double dTimeDelta);
	void WallLaserTrap(const _double dTimeDelta);
	void Check_Warp_Wormhole_Size(); // 포탈 타기 직전 사이즈 체크

	/* Hye */
	void Falling_Dead(const _double dTimeDelta);
	void PinBall(const _double dTimeDelta);
	void Ready_PinBall(const _double dTimeDelta);
	void LaserTennis(const _double dTimeDelta);

	/* 진혁 */
	void Holding_BossUFO(const _double dTimeDelta);
	void Push_ControlRoomBattery(const _double dTimeDelta);

public:
	void PinBall_Respawn(const _double dTimeDelta);
	void SpaceShip_Respawn(const _double dTimeDelta);
	_uint Get_HP() { return m_iHP; }
private:
	_bool Trigger_End(const _double dTimeDelta);
	_bool Trigger_Check(const _double dTimeDelta);

#pragma endregion

#pragma region Rail
public:
	void	Set_SpaceRailNode(CSpaceRail_Node* pRail);

private:
	void	KeyInput_Rail(_double dTimeDelta);
	void	Clear_TagerRailNodes();
	void	Find_TargetSpaceRail(); // LateTick에서 호출되어야함.
	void	Start_SpaceRail();
	void	MoveToTargetRail(_double dTimeDelta);
	void	TakeRail(_double dTimeDelta);
	void	TakeRailEnd(_double dTimeDelta);
	void	ShowRailTargetTriggerUI();

	HRESULT Ready_Layer_Gauge_Circle(const _tchar * pLayerTag);

private:
	_bool						m_bMoveToRail = false;
	_bool						m_bOnRail = false;
	_bool						m_bOnRailEnd = false;
	_bool						m_bOnRail_Effect = false;

private:
	_double						m_dRailEnd_ForceDeltaT = 0.0;
	_float						m_fRailSoundVolume = 1.f;

private:
	vector<CSpaceRail_Node*>	m_vecTargetRailNodes;
	CSpaceRail*					m_pTargetRail = nullptr;
	CSpaceRail_Node*			m_pSearchTargetRailNode = nullptr;
	CSpaceRail_Node*			m_pTargetRailNode = nullptr;
#pragma endregion

#pragma region UFO_JoyStick
private:
	void In_JoyStick(_double dTimeDelta);

private:
	_bool m_bEnterJoyStick = false;
	_bool m_IsInJoyStick = false;

#pragma endregion

#pragma region RadiarBlur
public:
	void Start_RadiarBlur_FullScreen(_double dBlurTime);
	void Start_RadiarBlur(_double dBlurTime);
	void Loop_RadiarBlur(_bool bLoop);
	void Trigger_RadiarBlur(_double dTimeDelta);
	void Set_RadiarBlur(_bool bActive);

private:
	_bool	m_bRadiarBlur_FullScreen = false;
	_bool	m_bRadiarBlur_Trigger = false;
	_bool	m_bRadiarBlur_Loop = false;
	_double m_dRadiarBlurTime = 0.0;
	_double m_dRadiarBlurDeltaT = 0.0;
#pragma endregion

#pragma region Dead_InBossroom
public:
	void Respawn_InBossroom();
	_bool Get_bDeadInBossroom() { return m_bDead_InBossroom; }

private:
	void DeadInBossroom(const _double dTimeDelta);

private:
	_bool m_bDead_InBossroom = false;
#pragma endregion

private: /* For. Ending */
	_bool	m_IsEnding = false;
	_bool   m_bSetEndingOffSetOnce = false;
	_vector m_vEndingRocketOffSetPos = {};
	_matrix m_matEndingRocketMatrix = {};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////																////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////						/* For. Sound */						////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////																////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Sound_Variable
	// Jump
	_float m_fCodyBJumpVolume = 1.f;
	_float m_fCodyMJumpVolume = 0.08f;
	_float m_fCodySJumpVolume = 0.5f;
	_float m_fCodyBJumpVoiceVolume = 1.f;

	// Double Jump
	_float m_fCodyMJumpDoubleVolume = 2.f;
	_float m_fCodySJumpDoubleVolume = 0.5f;

	// JumpLanding
	_float m_fCodyBJump_Landing_Volume = 1.f;
	_float m_fCodyMJumpLandingVoice_Volume = 1.f;
	_float m_fCodySJumpLandingVoice_Volume = 0.5f;

	// SizeChanging
	_float m_fSizing_BToM_Volume = 1.f;
	_float m_fSizing_SToM_Volume = 1.f;
	_float m_fSizing_MToS_Volume = 1.f;
	_float m_fSizing_MToB_Volume = 1.f;

	// Dash
	_float m_fCodyMDash_Volume = 1.f;
	_float m_fCodyMDash_Landing_Volume = 1.f;
	_float m_fCodyMDash_Roll_Volume = 1.f;
	_float m_fCodySDash_Voice_Volume = 0.5f;

	// GroundPound
	_float m_fCodyB_GroundPound_Landing_Voice_Volume = 1.f;
	_float m_fCodyM_GroundPound_Landing_Voice_Volume = 1.f;
	_float m_fCodyM_GroundPound_Volume = 1.f;
	_float m_fCodyM_GroundPound_Roll_Volume = 1.f;
	_float m_fCodyS_GroundPound_Landing_Voice_Volume = 1.f;

	// Walk
	_float m_fCodyB_Walk_Volume = 1.f;

	// Jog
	_float m_fCodyM_Jog_Volume = 1.f;

	// Sprint
	_float m_fCodyM_Sprint_Volume = 1.f;

	// Dead
	_float m_fCodyM_Dead_Burn_Volume = 1.f;
	_float m_fCodyM_Dead_Fall_Volume = 1.f;
	_float m_fCodyM_Dead_Electric_Shock = 1.f;

	//Revive
	_float m_fCodyM_Revive_Volume = 1.f;

	// WallJump
	_float m_fCodyM_WallJump_Volume = 1.f;
	_float m_fCody_WallJump_Slide_Volume = 1.f;

	// Rope
	_float m_fCody_Rope_Rail_Volume = 1.f;
	_float m_fCody_Rope_UFO_Catch_Volume = 1.f;
	_bool  m_bUFOCatchSoundOnce = false;
	_float m_fCody_Rope_UFO_Move_Volume = 1.f;
	_float m_fCody_Rope_UFO_Release_Volume = 1.f;
	_float m_fCody_Rope_UFO_Throw_Volume = 1.f;

	// MiniGame
	_float m_fCody_MiniGame_Damaged_Volume = 1.f;

	// GravityPipe
	_float m_fCody_GravityPipe_Voice_Volume = 1.f;
	_bool  m_bGravityPipe_FirstIn = false;
	_float m_fGravityPipe_SoundDelay = 0.f;

	// CamEffect
	_float m_fFootStepDelay = 0.f;
#pragma endregion

};
END