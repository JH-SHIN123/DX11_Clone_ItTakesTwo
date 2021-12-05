#pragma once

namespace Level
{
	enum ID { LEVEL_STATIC, LEVEL_STAGE, LEVEL_LOGO, LEVEL_END };
}

namespace Stage
{
	enum Stage { GravityPath, HitPlanet, ShootPinBall };
}

namespace Player
{
	enum ID { Cody, May, Default, PLAYER_END };
}

namespace UI
{
	enum TRIGGER {
		InputButton_Dot, InputButton_InterActive, InputButton_InterActive_Render,InputButton_PS_InterActive, InputButton_OK, InputButton_PS_OK, PC_Mouse_Reduction,
		PC_Mouse_Enlargement, InputButton_Cancle, InputButton_PS_Cancle, InputButton_Up, InputButton_Down, InputButton_PS_Up,
		InputButton_PS_Down, PlayerMarker, InputButton_PS_R1, Arrowkeys_Side, Arrowkeys_UpDown, Arrowkeys_All, InputButton_PS_L2, InputButton_PS_R2,
		StickIcon, Portrait_Cody, Portrait_May, RespawnCircle, MenuScreen, AlphaScreen, LoadingBook /*Default */, ControllerIcon_KeyBoard /*Default */,
		ControllerIcon_Pad /*Default */, HeaderBox, HeaderBox1P /*Default */, HeaderBox2P /*Default */, HeaderBox_1p_Ready /*Default */, 
		HeaderBox_2p_Ready /*Default */,  TRIGGER_END
	};

	enum INTERACTIVE_ID {
		ControlRoom_Battery, StarBuddy, PinBall_Door, PinBall_Handle, TutorialDoor, Umbrella_Joystick, SpaceValve, INTERACTIVE_ID_END
	};

}

