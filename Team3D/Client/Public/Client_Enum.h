#pragma once

namespace Level
{
	enum ID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_STAGE, LEVEL_END };
}

namespace Player
{
	enum ID { Cody, May, Default, PLAYER_END };
}

namespace UI
{
	enum TRIGGER {
		InputButton_Dot, InputButton_InterActive, InputButton_PS_InterActive, InputButton_OK, InputButton_PS_OK, PC_Mouse_Reduction,
		PC_Mouse_Enlargement, InputButton_Cancle, InputButton_PS_Cancle, InputButton_Up, InputButton_Down, InputButton_PS_Up,
		InputButton_PS_Down, PlayerMarker, InputButton_PS_R1, Arrowkeys_Side, Arrowkeys_UpDown, InputButton_PS_L2, InputButton_PS_R2,
		StickIcon, LoadingBook /* 얘는 Gerator_UI할때 무조건 Default로 해주셈요 */, Portrait_Cody, Portrait_May, RespawnCircle, TRIGGER_END
	};
}

