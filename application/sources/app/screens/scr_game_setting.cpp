#include "scr_game_setting.h"

/*****************************************************************************/
/* Variable Declaration - Setting game */
/*****************************************************************************/
tt_game_setting_t settingdata;
static uint8_t setting_location_chosse;

/*****************************************************************************/
/* View - Setting game */
/*****************************************************************************/
static void view_scr_game_setting();

view_dynamic_t dyn_view_item_game_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_setting
};

view_screen_t scr_game_setting = {
	&dyn_view_item_game_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_game_setting() {
	// Screen
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	// Icon
	view_render.drawBitmap(	0, \
							setting_location_chosse,\
							chosse_icon, \
							TT_GAME_SETTING_CHOSSE_ICON_SIZE_W, \
							TT_GAME_SETTING_CHOSSE_ICON_SIZE_H, \
							WHITE);

	// Frames
	//Level
	view_render.drawRoundRect(	TT_GAME_SETTING_FRAMES_AXIS_X, \
								TT_GAME_SETTING_FRAMES_AXIS_Y_1, \
								TT_GAME_SETTING_FRAMES_SIZE_W, \
								TT_GAME_SETTING_FRAMES_SIZE_H, \
								TT_GAME_SETTING_FRAMES_SIZE_R, \
								WHITE);

	//Music
	view_render.drawRoundRect(	TT_GAME_SETTING_FRAMES_AXIS_X, \
								TT_GAME_SETTING_FRAMES_AXIS_Y_1 + \
								TT_GAME_SETTING_FRAMES_STEP, \
								TT_GAME_SETTING_FRAMES_SIZE_W, \
								TT_GAME_SETTING_FRAMES_SIZE_H, \
								TT_GAME_SETTING_FRAMES_SIZE_R, \
								WHITE);
	//Exit
	view_render.drawRoundRect(	TT_GAME_SETTING_FRAMES_AXIS_X, \
								TT_GAME_SETTING_FRAMES_AXIS_Y_1 + \
								TT_GAME_SETTING_FRAMES_STEP*2, \
								TT_GAME_SETTING_FRAMES_SIZE_W, \
								TT_GAME_SETTING_FRAMES_SIZE_H, \
								TT_GAME_SETTING_FRAMES_SIZE_R, \
								WHITE);
	// Setting level: 1-10
	view_render.setCursor(TT_GAME_SETTING_TEXT_AXIS_X, 15);
	view_render.print(" Level ");

	view_render.setCursor(TT_GAME_SETTING_NUMBER_AXIS_X, 15);
	view_render.print(settingdata.block_speed);    

	//Setting music: On/Off
	view_render.setCursor(TT_GAME_SETTING_TEXT_AXIS_X, 30);
	view_render.print(" Music ");
	if (settingdata.silent == 0) {
		view_render.print("ON");
	}
	else {
		view_render.print("OFF");
	}

	//Setting exit
	view_render.setCursor(TT_GAME_SETTING_TEXT_AXIS_X, 45);
	view_render.print(" EXIT ") ;
	view_render.update();
}

/*****************************************************************************/
/* Handle - Setting game */
/*****************************************************************************/
void scr_game_setting_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// Clear view
		view_render.clear();
		// Chosse item arrdess 1
		setting_location_chosse = SETTING_ITEM_LEVEL;
		// Read setting data
		eeprom_read(	EEPROM_SETTING_START_ADDR, \
						(uint8_t*)&settingdata, \
						sizeof(settingdata));
	}
		break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: 
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		view_render.clear();
		switch (setting_location_chosse) 
		{
		case SETTING_ITEM_LEVEL: 
			{
				// Change block speed
				settingdata.block_speed++;
				if (settingdata.block_speed > 10) {
					settingdata.block_speed = 1;
				}
			}
			break;

		case SETTING_ITEM_MUSIC: 
			{
				settingdata.silent = !settingdata.silent;
				BUZZER_Sleep(settingdata.silent);
			}
			break;

		case SETTING_ITEM_EXIT: {
			// Save change and exit
			eeprom_write(	EEPROM_SETTING_START_ADDR, \
							(uint8_t*)&settingdata, \
							sizeof(settingdata));
			SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
			BUZZER_PlayTones(tones_startup);
		}
		}
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED");
		view_render.clear();

		setting_location_chosse += STEP_SETTING_CHOSSE;
		if(setting_location_chosse > SETTING_ITEM_EXIT)
		{
			setting_location_chosse = SETTING_ITEM_LEVEL;
		}
	}
	break;

	case AC_DISPLAY_BUTTON_UP_RELEASED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED");
		setting_location_chosse -= STEP_SETTING_CHOSSE;
		if(setting_location_chosse < SETTING_ITEM_LEVEL)
		{
			setting_location_chosse = SETTING_ITEM_EXIT;
		}
	}
	break;

	default:
	 	break;
	}
}
