#include "scr_tetris_game.h"
#include "screens.h"
#include "buzzer.h"

uint8_t tt_game_state = GAME_OFF;
/*==================== VIEW SCREEN ====================*/
static void view_scr_tetris_game();

view_dynamic_t dyn_view_item_tetris_game = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_tetris_game
};

view_screen_t scr_tetris_game = {
    &dyn_view_item_tetris_game,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_tetris_game() {
    getTetrisGameManager()->render();
}

void tt_game_time_tick_setup()
{
    timer_set(AC_TASK_DISPLAY_ID, 
                     TT_GAME_TIME_TICK, 
                     TT_GAME_TIME_TICK_INTERVAL, 
                     TIMER_PERIODIC);
}

/*==================== SCREEN HANDLER (Event-Driven) ====================*/
void scr_tetris_game_handle(ak_msg_t* msg) {
    TetrisGameManager* game = getTetrisGameManager();
    
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("TETRIS_SCREEN_ENTRY\n");
            
            game->handleGameSetup();
            
            // Check if spawn position is valid
            task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_SETUP);
            task_post_pure_msg(TT_GAME_BOARD_ID, TT_GAME_BOARD_SETUP);
            
            tt_game_time_tick_setup();
            tt_game_state = GAME_PLAY;
            
        }
        break;
        
        case TT_GAME_TIME_TICK: {
            APP_DBG_SIG("TT_GAME_TIME_TICK\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_MOVE_DOWN);
            }
        }
        break;
        
        case TT_GAME_SPAWN_BLOCK: {
            APP_DBG_SIG("TT_GAME_SPAWN_BLOCK\n");
            game->handleSpawnBlock();
        }
        break;
        
        case TT_GAME_OVER: {
            APP_DBG_SIG("TT_GAME_OVER\n");
            tt_game_state = GAME_OVER;
            timer_remove_attr(AC_TASK_DISPLAY_ID, TT_GAME_TIME_TICK);
            
            tt_game_time_tick_setup();
        }
        break;
        
        case TT_GAME_EXIT_GAME: {
            APP_DBG_SIG("TT_GAME_EXIT_GAME\n");
            
            tt_game_state = GAME_OFF;
            game->handleGameReset();
            
            // Return to menu
            SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
        }
        break;
        
        case AC_DISPLAY_BUTTON_MODE_RELEASED: {
            APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_ROTATE);
            } else {
                // Restart game
                task_post_pure_msg(TT_GAME_SCREEN_ID, TT_GAME_EXIT_GAME);
            }
        }
        break;
        
        case AC_DISPLAY_BUTTON_UP_RELEASED: {
            APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_HARD_DROP);
            }
        }
        break;
        
        case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
            APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_MOVE_DOWN);
            }
        }
        break;
        
        case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {
            APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_LONG_PRESSED\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_MOVE_LEFT);
            }
        }
        break;
        
        case AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED: {
            APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED\n");
            if (!game->getState()->getIsGameOver()) {
                task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_MOVE_RIGHT);
            }
        }
        break;
        
        default:
            break;
    }
}