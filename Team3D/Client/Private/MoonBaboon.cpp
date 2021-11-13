#include "stdafx.h"
#include "..\public\MoonBaboon.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "May.h"
#include "Cody.h"

CMoonBaboon::CMoonBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	
}

CMoonBaboon::CMoonBaboon(const CMoonBaboon & rhs)
	: CGameObject(rhs)
{
	C_ActionMH
		C_ActionMH_To_Idle
		C_AirDash_Start
		C_Bhv_ArcadeScreenLever_Bck
		C_Bhv_ArcadeScreenLever_BckLeft
		C_Bhv_ArcadeScreenLever_BckRight
		C_Bhv_ArcadeScreenLever_Fwd
		C_Bhv_ArcadeScreenLever_FwdLeft
		C_Bhv_ArcadeScreenLever_FwdRight
		C_Bhv_ArcadeScreenLever_Left
		C_Bhv_ArcadeScreenLever_MH
		C_Bhv_ArcadeScreenLever_Right
		C_Bhv_ArcadeScreenLever_Trigger
		C_Bhv_Bounce_01
		C_Bhv_Bounce_02
		C_Bhv_Bounce_03
		C_Bhv_Bounce_04
		C_Bhv_Bump_Collide
		C_Bhv_Bump_Collide_Fall
		C_Bhv_ChagneSize_PlanetPush_Small
		C_Bhv_ChangeSize_GroundPound_Falling
		C_Bhv_ChangeSize_GroundPound_Land_Exit
		C_Bhv_ChangeSize_GroundPound_Land_MH
		C_Bhv_ChangeSize_GroundPound_Start
		C_Bhv_ChangeSize_PlanetPush_Large
		C_Bhv_ChangeSize_PlanetPush_Medium
		C_Bhv_ChessMiniGame_Gestures_Point
		C_Bhv_DashMoveTo_Start
		C_Bhv_DashMoveTo_Start_InAir
		C_Bhv_DashMoveTo_Start_Left90
		C_Bhv_DashMoveTo_Start_Right90
		C_Bhv_Death_Fall_MH
		C_Bhv_Death_Oil_Drown
		C_Bhv_FidgetSpinner_Enter
		C_Bhv_FidgetSpinner_Exit
		C_Bhv_FidgetSpinner_Hover_Fast_MH
		C_Bhv_FidgetSpinner_Hover_Fast_MH_Left
		C_Bhv_FidgetSpinner_Hover_Fast_MH_Right
		C_Bhv_FidgetSpinner_Hover_Slow_MH
		C_Bhv_FidgetSpinner_Hover_Slow_MH_Left
		C_Bhv_FidgetSpinner_Hover_Slow_MH_Right
		C_Bhv_FidgetSpinner_InAir_Enter
		C_Bhv_FidgetSpinner_InAir_Exit
		C_Bhv_Generic_WalkIn
		C_Bhv_GroundPound_Falling
		C_Bhv_GroundPound_Jump_High
		C_Bhv_GroundPound_Jump_Low
		C_Bhv_GroundPound_Land
		C_Bhv_GroundPound_Land_Exit
		C_Bhv_GroundPound_Land_MH
		C_Bhv_GroundPound_Start
		C_Bhv_HoldButton_Enter
		C_Bhv_HoldButton_Exit
		C_Bhv_HoldButton_MH
		C_Bhv_Jump_Start
		C_Bhv_KnockDown_Back_Land
		C_Bhv_KnockDown_Back_MH
		C_Bhv_KnockDown_Back_Start
		C_Bhv_KnockDown_Front
		C_Bhv_KnockDown_Front_Land
		C_Bhv_KnockDown_Front_MH
		C_Bhv_KnockDown_Front_Start
		C_Bhv_KnockDown_Left_Land
		C_Bhv_KnockDown_Left_MH
		C_Bhv_KnockDown_Left_Start
		C_Bhv_knockDown_Right_Land
		C_Bhv_KnockDown_Right_MH
		C_Bhv_KnockDown_Right_Start
		C_Bhv_Lever_Left
		C_Bhv_Lever_Right
		C_Bhv_LeverPull_Exit_Fast
		C_Bhv_LeverPull_Exit_Slow
		C_Bhv_LeverPull_MH
		C_Bhv_LeverPull_Start_Fast
		C_Bhv_MH_Gesture_Big_Waiting_Enter
		C_Bhv_MH_Gesture_Big_Waiting_MH
		C_Bhv_MH_Gesture_Small_AdjustLeaf
		C_Bhv_MH_Gesture_Small_Belly
		C_Bhv_MH_Gesture_Small_Drumming
		C_Bhv_MH_Gesture_Small_InspectNails
		C_Bhv_MH_Gesture_Small_LookAround
		C_Bhv_MH_Gesture_Small_Scratch
		C_Bhv_MH_Gesture_Small_Stretch
		C_Bhv_MH_Gesture_Small_TossLeaf
		C_Bhv_MiniGame_Emotes_AssDance
		C_Bhv_MiniGame_Emotes_CossackDance
		C_Bhv_MiniGame_Emotes_Explosion
		C_Bhv_MiniGame_Emotes_FistPump
		C_Bhv_MiniGame_Emotes_Fonzie
		C_Bhv_MiniGame_Emotes_HappyDance
		C_Bhv_MiniGame_Emotes_Huzzah
		C_Bhv_MiniGame_Emotes_JumpUpLand
		C_Bhv_MiniGame_Emotes_KarateBow
		C_Bhv_MiniGame_Emotes_LeafFlex
		C_Bhv_MiniGame_Emotes_Lose_Var1
		C_Bhv_MiniGame_Emotes_Lose_Var2
		C_Bhv_MiniGame_Emotes_Lose_Var3
		C_Bhv_MiniGame_Emotes_ShoulderBrush
		C_Bhv_MiniGame_Emotes_WatchingYou
		C_Bhv_MiniGame_Emotes_WeirdRobot
		C_Bhv_PickUp
		C_Bhv_PickUp_Aim_Strafe_Bwd
		C_Bhv_PickUp_Aim_Strafe_Fwd
		C_Bhv_PickUp_Aim_Strafe_Left
		C_Bhv_PickUp_Aim_Strafe_Right
		C_Bhv_PickUp_AimSpace_Center
		C_Bhv_PickUp_AimSpace_Down
		C_Bhv_pickUp_AimSpace_Up
		C_Bhv_PickUp_FuseCrab_PutInSocket
		C_Bhv_PickUp_Jog
		C_Bhv_PickUp_Jog_Stop
		C_Bhv_PickUp_Jump_Falling
		C_Bhv_PickUp_Jump_Land
		C_Bhv_PickUp_Jump_Land_Jog
		C_Bhv_PickUp_MH
		C_Bhv_PickUp_Throw
		C_Bhv_PickUpHeavySmall_Jog
		C_Bhv_PickUpHeavySmall_Jog_StartInMotion
		C_Bhv_PickUpHeavySmall_Jog_Stop
		C_Bhv_PickUpHeavySmall_Jump_Falling
		C_Bhv_PickUpHeavySmall_Jump_Land
		C_Bhv_PickUpHeavySmall_Jump_Land_Jog
		C_Bhv_PickUpHeavySmall_Jump_Start
		C_Bhv_PickUpHeavySmall_MH
		C_Bhv_PickUpHeavySmall_PickUp
		C_Bhv_PickUpHeavySmall_PutDown
		C_Bhv_PickUpHeavySmall_PutDown_InPlace
		C_Bhv_PickUpSmall
		C_Bhv_PickUpSmall_Aim_MH
		C_Bhv_PickUpSmall_Aim_Strafe_Fwd
		C_Bhv_PickUpSmall_AimSpace_Down
		C_Bhv_PickUpSmall_AimSpace_Up
		C_Bhv_PickUpSmall_AirDashStart
		C_Bhv_PickUpSmall_Bhv_GroundPound_Falling
		C_Bhv_PickUpSmall_Bhv_GroundPound_Jump_High
		C_Bhv_PickUpSmall_Bhv_GroundPound_Jump_Low
		C_Bhv_PickUpSmall_Bhv_GroundPound_Land
		C_Bhv_PickUpSmall_Bhv_GroundPound_Land_MH
		C_Bhv_PickUpSmall_Bhv_GroundPound_Start
		C_Bhv_PickUpSmall_Bhv_Swinging_Bck
		C_Bhv_PickUpSmall_Bhv_Swinging_Bck_Bck
		C_Bhv_PickUpSmall_Bhv_Swinging_Bck_Fwd
		C_Bhv_PickUpSmall_Bhv_Swinging_Bck_Neutral
		C_Bhv_PickUpSmall_Bhv_Swinging_Bck_TurnAround
		C_Bhv_PickUpSmall_Bhv_Swinging_ExitBck
		C_Bhv_PickUpSmall_Bhv_Swinging_ExitFwd
		C_Bhv_PickUpSmall_Bhv_Swinging_Fwd
		C_Bhv_PickUpSmall_Bhv_Swinging_JumpBck
		C_Bhv_PickUpSmall_Bhv_Swinging_JumpFwd
		C_Bhv_PickUpSmall_Bhv_Swinging_Neutral
		C_Bhv_PickUpSmall_Bhv_Swinging_TurnAround
		C_Bhv_PickUpSmall_Bounce_01
		C_Bhv_PickUpSmall_Bounce_02
		C_Bhv_PickUpSmall_Bounce_03
		C_Bhv_PickUpSmall_Bounce_04
		C_Bhv_PickUpSmall_DoubleJump
		C_Bhv_PickUpSmall_Grind_Fast
		C_Bhv_PickUpSmall_Grind_Fast_MH_Left
		C_Bhv_PickUpSmall_Grind_Fast_MH_Right
		C_Bhv_PickUpSmall_Grind_Grapple_Enter
		C_Bhv_PickUpSmall_Grind_Grapple_ToGrind
		C_Bhv_PickUpSmall_Grind_Jump
		C_Bhv_PickUpSmall_Grind_Land
		C_Bhv_PickUpSmall_Grind_Slow_MH
		C_Bhv_PickUpSmall_Grind_Slow_MH_Left
		C_Bhv_PickUpSmall_Grind_Slow_MH_Right
		C_Bhv_PickUpSmall_Grind_TurnAround_Boost
		C_Bhv_PickUpSmall_Grind_TurnAround_SlowDown
		C_Bhv_PickUpSmall_Jog
		C_Bhv_PickUpSmall_Jog_GoToStop
		C_Bhv_PickUpSmall_Jog_Start_Fwd
		C_Bhv_PickUpSmall_Jog_Start_InMotion
		C_Bhv_PickUpSmall_Jog_Stop_Fwd
		C_Bhv_PickUpSmall_Jump_Falling
		C_Bhv_PickUpSmall_Jump_From_Landing
		C_Bhv_PickUpSmall_Jump_Land
		C_Bhv_PickUpSmall_Jump_Land_Jog
		C_Bhv_PickUpSmall_Jump_Start
		C_Bhv_PickUpSmall_Jump_StartFromStill
		C_Bhv_PickUpSmall_LedgeGrab_ClimbUp
		C_Bhv_PickUpSmall_LedgeGrab_Drop
		C_Bhv_PickUpSmall_LedgeGrab_JumpBack_Left
		C_Bhv_PickUpSmall_LedgeGrab_JumpUp
		C_Bhv_PickUpSmall_LedgeGrab_MH
		C_Bhv_PickUpSmall_LedgeGrab_MH_Left120
		C_Bhv_PickUpSmall_LedgeGrab_MH_Left180
		C_Bhv_PickUpSmall_LedgeGrab_MH_Left30
		C_Bhv_PickUpSmall_LedgeGrab_MH_Left60
		C_Bhv_PickUpSmall_LedgeGrab_MH_Left90
		C_Bhv_PickUpSmall_LedgeGrab_Start
		C_Bhv_PickUpSmall_LedgeVault
		C_Bhv_PickUpSmall_LedgeVaultStart
		C_Bhv_PickUpSmall_MH
		C_Bhv_PickUpSmall_PerfectDash
		C_Bhv_PickUpSmall_PerfectDashJog
		C_Bhv_PickUpSmall_PerfectDashStop
		C_Bhv_PickUpSmall_PutDown
		C_Bhv_PickUpSmall_PutDown_InPlace
		C_Bhv_PickUpSmall_Roll_Start
		C_Bhv_PickUpSmall_Roll_Stop
		C_Bhv_PickUpSmall_Roll_To_Jog
		C_Bhv_PickUpSmall_Sprint
		C_Bhv_PickUpSmall_SprintStartFromDash
		C_Bhv_PickUpSmall_SprintTurnAround
		C_Bhv_PickUpSmall_Strafe_Aim_Shuffle_Left
		C_Bhv_PickUpSmall_Strafe_Aim_Shuffle_Right
		C_Bhv_PickUpSmall_Strafe_AimSpace_Charge_Down
		C_Bhv_PickUpSmall_Strafe_AimSpace_Charge_Up
		C_Bhv_PickUpSmall_Strafe_Bwd
		C_Bhv_PickUpSmall_Strafe_Bwd_L
		C_Bhv_PickUpSmall_Strafe_Bwd_R
		C_Bhv_PickUpSmall_Strafe_Fwd
		C_Bhv_PickUpSmall_Strafe_Fwd_L
		C_Bhv_PickUpSmall_Strafe_Fwd_R
		C_Bhv_PickUpSmall_Strafe_Left
		C_Bhv_PickUpSmall_Strafe_MH
		C_Bhv_PickUpSmall_Strafe_Right
		C_Bhv_PickUpSmall_Throw
		C_Bhv_PickUpSmall_WallSlide_Enter
		C_Bhv_PickUpSmall_WallSlide_Enter_FromDash
		C_Bhv_PickUpSmall_WallSlide_Enter_NotJumpable
		C_Bhv_PickUpSmall_WallSlide_Jump
		C_Bhv_PickUpSmall_WallSlide_MH
		C_Bhv_pikUpSmall_Grind_Dash
		C_Bhv_PlasmaBall_Enter
		C_Bhv_PlasmaBall_Exit
		C_Bhv_PlasmaBall_MH
		C_Bhv_PlayRoom_LavaLampButton
		C_Bhv_PlayRoom_MageCombat_Attack_Var1
		C_Bhv_PlayRoom_MageCombat_Attack_Var1_2
		C_Bhv_PlayRoom_MageCombat_Attack_Var1_3
		C_Bhv_PlayRoom_MageCombat_Attack_Var2
		C_Bhv_PlayRoom_MageCombat_Attack_Var2_2
		C_Bhv_PlayRoom_MageCombat_Attack_Var2_3
		C_Bhv_PlayRoom_MageCombat_Attack_Var3
		C_Bhv_PlayRoom_MageCombat_Attack_Var3_2
		C_Bhv_PlayRoom_MageCombat_Attack_Var3_3
		C_Bhv_PlayRoom_MageCombat_Attack_Var3_4
		C_Bhv_PlayRoom_MageCombat_BeamAttack
		C_Bhv_PlayRoom_MageCombat_Blink_Exit
		C_Bhv_PlayRoom_MageCombat_MH
		C_Bhv_PlayRoom_MageCombat_OrbAttack
		C_Bhv_PlayRoom_MageCombat_TakeDamage_Var1
		C_Bhv_PlayRoom_MageCombat_TakeDamage_Var2
		C_Bhv_PlayRoom_MageCombat_TakeDamage_Var3
		C_Bhv_PlayRoom_SpinningSpaceLamp
		C_Bhv_PlayRoom_ZeroGravity_AdditiveRotation_Fwd
		C_Bhv_PlayRoom_ZeroGravity_AdditiveRotation_Right
		C_Bhv_PlayRoom_ZeroGravity_Back
		C_Bhv_PlayRoom_ZeroGravity_Fwd
		C_Bhv_PlayRoom_ZeroGravity_Left
		C_Bhv_PlayRoom_ZeroGravity_MH
		C_Bhv_PlayRoom_ZeroGravity_Right
		C_Bhv_Pull
		C_Bhv_Push_Battery_Bck
		C_Bhv_Push_Battery_Fwd
		C_Bhv_Push_Battery_MH
		C_Bhv_Push_Enter
		C_Bhv_Push_Exit
		C_Bhv_Push_Fwd
		C_Bhv_Push_MH
		C_Bhv_Push_Struggle
		C_Bhv_PushButton_Var1
		C_Bhv_PushButton_Var2
		C_Bhv_PushButton_Var2_SapGun
		C_Bhv_PutDown
		C_Bhv_PutDown_InPlace
		C_Bhv_RocketFirework
		C_Bhv_Shed_VacuumLaunch_Enter
		C_Bhv_Shed_VacuumLaunch_MH
		C_Bhv_Skydive_Bwd
		C_Bhv_Skydive_Fwd
		C_Bhv_Skydive_Left
		C_Bhv_Skydive_MH
		C_Bhv_Skydive_Right
		C_Bhv_Skydive_Start
		C_Bhv_Swinging_Bck
		C_Bhv_Swinging_Bck_Bck
		C_Bhv_Swinging_Bck_Fwd
		C_Bhv_Swinging_Bck_Left
		C_Bhv_Swinging_Bck_Neutral
		C_Bhv_Swinging_Bck_Right
		C_Bhv_Swinging_Bck_TurnAround
		C_Bhv_Swinging_Enter
		C_Bhv_Swinging_ExitBck
		C_Bhv_Swinging_ExitFwd
		C_Bhv_Swinging_Fwd
		C_Bhv_Swinging_JumpBck
		C_Bhv_Swinging_JumpFwd
		C_Bhv_Swinging_Left
		C_Bhv_Swinging_Neutral
		C_Bhv_Swinging_Right
		C_Bhv_Swinging_TurnAround
		C_Bhv_Valve_Rotate_MH
		C_Bhv_Valve_Rotate_R
		C_Bhv_Valve_Rotate_R_Short
		C_Bhv_Valve_StruggleLeft
		C_Bhv_Valve_StruggleLeft_MH
		C_Bhv_Valve_StruggleLeftEnter
		C_Bhv_Valve_StruggleRight
		C_Bhv_Valve_StruggleRight_MH
		C_Bhv_Valve_StruggleRightEnter
		C_ChangeSize_Jump_Falling
		C_ChangeSize_Jump_Large_Land
		C_ChangeSize_Jump_Large_Land_Jog
		C_ChangeSize_Jump_Start
		C_ChangeSize_Roll_Start
		C_ChangeSize_Walk_Large_Bank_Left
		C_ChangeSize_Walk_Large_Bank_Right
		C_ChangeSize_Walk_Large_Fwd
		C_ChangeSize_Walk_Large_Start
		C_ChangeSize_Walk_Large_Stop
		C_Crouch_Enter_GroundPound
		C_Crouch_Enter_Landing
		C_Crouch_Enter_MH
		C_Crouch_Enter_Slide
		C_Crouch_Enter_SlideMovement
		C_Crouch_Enter_Walk
		C_Crouch_Exit_MH
		C_Crouch_Exit_Movement
		C_Crouch_Exit_Movement_Walk
		C_Crouch_Fwd
		C_Crouch_MH
		C_Crouch_Start
		C_Crouch_Stop
		C_CutScene_BossFight_BeamOut
		C_CutScene_BossFight_Eject
		C_CutScene_BossFight_EnterUFO
		C_CutScene_BossFight_Intro
		C_CutScene_BossFight_LandInsideUFO
		C_CutScene_BossFight_LaserRippedOff
		C_CutScene_BossFight_Outro
		C_CutScene_Hub_SecondGenerator
		C_DoubleJump
		C_Exhausted_MH
		C_Exhausted_MH_To_Idle
		C_Grind_Btn_Press
		C_Grind_Btn_Ready
		C_Grind_Dash
		C_Grind_Fast_MH
		C_Grind_Fast_MH_Left
		C_Grind_Fast_MH_Right
		C_Grind_Grapple_Enter
		C_Grind_Grapple_ToGrind
		C_Grind_Jump
		C_Grind_Land
		C_Grind_Slow_MH
		C_Grind_Slow_MH_Left
		C_Grind_Slow_MH_LookLeft
		C_Grind_Slow_MH_LookRight
		C_Grind_Slow_MH_ReachLeft
		C_Grind_Slow_MH_ReachLeftDown
		C_Grind_Slow_MH_ReachLeftUp
		C_Grind_Slow_MH_ReachRight
		C_Grind_Slow_MH_ReachRightDown
		C_Grind_Slow_MH_ReachRightUp
		C_Grind_Slow_MH_Right
		C_Grind_Transfer_LandLeft
		C_Grind_Transfer_LandRight
		C_Grind_Transfer_Left
		C_Grind_Transfer_Right
		C_Grind_TurnAround_Boost
		C_Grind_TurnAround_SlowDown
		C_Grind_UpsideDown_Enter
		C_Grind_UpsideDown_Exit
		C_Grind_UpsideDown_Fast_MH
		C_Grind_UpsideDown_Fast_MH_Left
		C_Grind_UpsideDown_Fast_MH_Right
		C_Grind_UpsideDown_Slow_MH
		C_Grind_UpsideDown_Slow_MH_LEFT
		C_Grind_UpsideDown_Slow_MH_RIGHT
		C_Idle_To_Action
		C_Idle_To_Exhausted
		C_Jog
		C_Jog_Action_MH_Start_Fwd
		C_Jog_Bank_Left
		C_Jog_Bank_Right
		C_Jog_Exhausted_MH_Start_Fwd
		C_Jog_GoToStop
		C_Jog_LookBackL
		C_Jog_LookBackR
		C_Jog_Start_Fwd
		C_Jog_Start_Fwd_Left_180
		C_Jog_Start_Fwd_Right_180
		C_Jog_Start_InMotion
		C_Jog_Start_InMotion_Var2
		C_Jog_Stop_Fwd
		C_Jog_Stop_Fwd_Action
		C_Jog_Stop_Fwd_Easy
		C_Jog_Stop_Fwd_Exhausted
		C_Jog_Strafe_Bwd
		C_Jog_Strafe_Fwd
		C_Jog_Strafe_Left
		C_Jog_Strafe_Right
		C_Jump_180L
		C_Jump_180R
		C_Jump_Falling
		C_Jump_Falling_Backwards
		C_Jump_Falling_Boosted
		C_Jump_Falling_Boosted_Left
		C_Jump_Falling_Boosted_Right
		C_Jump_Falling_Boosted_Sucked
		C_Jump_Falling_Forwards
		C_Jump_Falling_Left
		C_Jump_Falling_Right
		C_Jump_FromDash
		C_Jump_FromDashStop
		C_Jump_Land
		C_Jump_Land_Dash
		C_Jump_Land_Dash_Jog
		C_Jump_Land_Dash_Walk
		C_Jump_Land_High
		C_Jump_Land_High_Jog
		C_Jump_Land_High_Jog_Cancel
		C_Jump_Land_High_Jog_Jump
		C_Jump_Land_High_Start_Still
		C_Jump_Land_High_Start_Still_Walk
		C_Jump_Land_High_Walk
		C_Jump_Land_Jog
		C_Jump_Land_Jog_Jump
		C_Jump_Land_Jog_Jump_Var2
		C_Jump_Land_Jog_Jump_Var3
		C_Jump_Land_Jog_Jump_Var4
		C_Jump_Land_Jog_Jump_Var5
		C_Jump_Land_Jog_Jump_Var6
		C_Jump_Land_PerfectDash
		C_Jump_Land_PerfectDash_Jog
		C_Jump_Land_PerfectDash_Walk
		C_Jump_Land_Start_Still
		C_Jump_Land_Start_Still_Walk
		C_Jump_Land_Still_Jump
		C_Jump_Land_Walk
		C_Jump_LongJump
		C_Jump_LongJump_PickUpSmall
		C_Jump_LongJump_Var1
		C_Jump_Start
		C_Jump_WallClimb
		C_Jump_WallClimb_SlideDown
		C_JumpFromPerfectDash
		C_JumpFromPerfectDashStop
		C_LedgeGrab_ClimbLeft
		C_LedgeGrab_ClimbRight
		C_LedgeGrab_ClimbUp
		C_LedgeGrab_Drop
		C_LedgeGrab_JumpBack_Left
		C_LedgeGrab_JumpBack_Right
		C_LedgeGrab_JumpUp
		C_LedgeGrab_MH
		C_LedgeGrab_MH_Left120
		C_LedgeGrab_MH_Left180
		C_LedgeGrab_MH_Left30
		C_LedgeGrab_MH_Left60
		C_LedgeGrab_MH_Left90
		C_LedgeGrab_MH_Right120
		C_LedgeGrab_MH_Right180
		C_LedgeGrab_MH_Right30
		C_LedgeGrab_MH_Right60
		C_LedgeGrab_MH_Right90
		C_LedgeGrab_Start
		C_LedgeVault
		C_LedgeVaultStart
		C_MH
		C_PickUpSmall_Bhv_GroundPound_Land_Exit
		C_PlasmaBall_Bwd
		C_PlasmaBall_Bwd_Left
		C_PlasmaBall_Bwd_Right
		C_PlasmaBall_Fwd
		C_PlasmaBall_Fwd_Left
		C_PlasmaBall_Fwd_Right
		C_PlasmaBall_Left
		C_PlasmaBall_Right
		C_PlayRoom_MageCombat_Jog
		C_PlayRoom_MageCombat_Jog_Start_Fwd
		C_PlayRoom_MageCombat_jog_Stop_Fwd
		C_Roll_From_GroundPound
		C_Roll_From_PerfectJump
		C_Roll_Jog_From_PerfectDash
		C_Roll_Jump
		C_Roll_PerfectDash
		C_Roll_PerfectJump
		C_Roll_Start
		C_Roll_Stop
		C_Roll_Stop_From_PerfectDash
		C_Roll_To_Jog
		C_Roll_To_Walk
		C_Roll_Walk_From_PerfectDash
		C_SkyDiveLanding
		C_SkyDiveLanding_Fwd
		C_SkyDiveLanding_Walk
		C_Slide_Enter
		C_Slide_Enter_Dash
		C_Slide_Enter_Falling
		C_Slide_Enter_GroundPound
		C_Slide_Exit_Falling
		C_Slide_ExitToMovement
		C_Slide_ExitToMovement_Walk
		C_Slide_Fwd
		C_Slide_Jump
		C_Slide_Left
		C_Slide_Right
		C_Sprint
		C_Sprint_Jump
		C_Sprint_Left
		C_Sprint_Right
		C_Sprint_Start_FromDash
		C_SprintTurnAround
		C_Strafe_Bwd
		C_Strafe_BwdL
		C_Strafe_BwdR
		C_Strafe_Fwd
		C_Strafe_FwdL
		C_Strafe_FwdR
		C_Strafe_Left
		C_Strafe_MH
		C_Strafe_Right
		C_Strafe_TurnInPlace_Left
		C_Strafe_TurnInPlace_Right
		C_Walk_Action_MH_Start_Fwd
		C_Walk_Bank_Left
		C_Walk_Bank_Right
		C_Walk_Exhausted_MH_Start_Fwd
		C_Walk_Fwd
		C_Walk_Locomotion_Walk
		C_Walk_Start_Fwd
		C_Walk_Start_Fwd_Left_180
		C_Walk_Start_Fwd_Right_180
		C_Walk_Start_InMotion
		C_Walk_Start_InMotion_Var2
		C_WallRun_Left_Enter
		C_WallRun_Left_Jump
		C_WallRun_Left_Run
		C_WallRun_Right_Enter
		C_WallRun_Right_Jump
		C_WallRun_Right_Run
		C_WallSlide_Enter
		C_WallSlide_Enter_FromDash
		C_WallSlide_Enter_NotJumpable
		C_WallSlide_Jump
		C_WallSlide_Jump_Vertical
		C_WallSlide_MH
		C_WallSlide_MH_ReadyToDash

}

HRESULT CMoonBaboon::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	m_pCodyTransform = ((CCody*)CDataBase::GetInstance()->GetCody())->Get_Transform();
	if (nullptr == m_pCodyTransform)
		return E_FAIL;
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataBase::GetInstance()->GetMay())->Get_Transform();
	if (nullptr == m_pMayTransform)
		return E_FAIL;
	Safe_AddRef(m_pMayTransform);
	
	m_pModelCom->Set_Animation(0, m_pTransformCom);
	m_pModelCom->Set_NextAnimIndex(0);

	return S_OK;
}

_int CMoonBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	Check_State(dTimeDelta);
	Change_State(dTimeDelta);
	During_Animation_Behavior(dTimeDelta);


	PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);
	m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CMoonBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}



CMoonBaboon::MOON_STATE CMoonBaboon::Check_State(_double TimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		m_pModelCom->Set_Animation(m_eNextState, m_pTransformCom);
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CMoonBaboon::Change_State(_double TimeDelta)
{
	if (m_eTarget == TARGET_CODY)
	{
		int i = 0;
	}
	else
	{
		int i = 0;
	}
}

void CMoonBaboon::During_Animation_Behavior(_double TimeDelta)
{
}

HRESULT CMoonBaboon::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CMoonBaboon::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CMoonBaboon::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	return S_OK;
}

CMoonBaboon * CMoonBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonBaboon* pInstance = new CMoonBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoonBaboon::Clone_GameObject(void * pArg)
{
	CMoonBaboon* pInstance = new CMoonBaboon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon::Free()
{
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}