#ifndef _TT_GAME_BLOCK_H
#define _TT_GAME_BLOCK_H

#include <stdint.h>
#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"
#include "task.h"
#include "task_list.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"
#include "eeprom.h"
#include "app_eeprom.h"

#include "scr_tetris_game.h"
#include "screens.h"

#define MATRIX_SIZE  4
#define BLOCK_SIZE  6
#define GAME_BOARD_WIDTH 15
#define GAME_BOARD_HEIGHT 9
#define GAME_BOARD_X_OFFSET 36
#define GAME_BOARD_Y_OFFSET 10

enum BlockType
{
    BLOCK_I = 0,
    BLOCK_J,
    BLOCK_L,
    BLOCK_T,
    BLOCK_O,
    BLOCK_S,
    BLOCK_Z,
    NUM_BLOCK_TYPES
};

/*=================DATA BASE BLOCK====================*/
static const uint8_t I_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{1,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}, 
    {{1,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, 
    {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}} 
};

static const uint8_t J_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = 
{
    {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 
    {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
    {{1,1,1,0}, {0,0,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}
};

static const uint8_t L_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
    {{1,1,1,0}, {1,0,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}
};

static const uint8_t O_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
};

static const uint8_t S_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
    {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}}
};

static const uint8_t T_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
    {{1,1,1,0}, {0,1,0,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
};

static const uint8_t Z_SHAPES[4][MATRIX_SIZE][MATRIX_SIZE] = {
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
    {{0,1,0,0}, {0,1,1,0}, {1,0,0,0}, {0,0,0,0}}  
};


/*=============================== BOARD GAME =======================================*/
/*game matrix management*/
class GameBoard {
private:
    uint8_t cells[GAME_BOARD_HEIGHT][GAME_BOARD_WIDTH];
    bool isLineFull(uint8_t line) const;
    void clearLine(uint8_t line);
public:
    GameBoard();
    ~GameBoard();
    
    void clear();
    void draw();
    bool isCellOccupied(int8_t x, int8_t y) const;
    void setCell(int8_t x, int8_t y, uint8_t value);
    uint8_t getCell(int8_t x, int8_t y) const;
    uint8_t checkAndClearLines();
    
    // Event handlers
    void handleSetup();
    void handleCheckLines();
    void handleReset();
};

/*=============================== BLOCK =======================================*/
class Block {
private:
    int8_t x, y;
    uint8_t rotation;
    BlockType type;
    bool visible;
    const uint8_t (*shapeData)[4][MATRIX_SIZE][MATRIX_SIZE];
    
public:
    Block();
    Block(BlockType blockType);
    ~Block();
    
    void setup(BlockType blockType);
    void draw();
    bool checkCollision(const GameBoard& board, int8_t offsetX, int8_t offsetY, uint8_t rot) const;
    void lockToBoard(GameBoard& board);
    
    // Movement methods
    bool rotate(const GameBoard& board);
    bool moveLeft(const GameBoard& board);
    bool moveRight(const GameBoard& board);
    bool moveDown(const GameBoard& board);
    void hardDrop(const GameBoard& board);
    //bool moveUp(const GameBoard& board);
    
    // Event handlers
    void handleSetup(BlockType blockType);
    void handleRotate(const GameBoard& board);
    void handleMoveLeft(const GameBoard& board);
    void handleMoveRight(const GameBoard& board);
    void handleMoveDown(const GameBoard& board);
    void handleHardDrop(const GameBoard& board);
    void handleLock(GameBoard& board);
    void handleReset();

    //void handleMoveUp(const GameBoard& board);
    
    // Getters
    int8_t getX() const { return x; }
    int8_t getY() const { return y; }
    uint8_t getRotation() const { return rotation; }
    BlockType getType() const { return type; }
    bool isVisible() const { return visible; }
    
private:
    void loadShape();
};


/*=============================== GAME STATE =======================================*/
class GameState {
private:
    uint32_t score;
    uint32_t level;
    uint32_t linesCleared;
    bool isGameOver;
    BlockType nextBlockType;
    
public:
    GameState();
    ~GameState();
    
    void init();
    void reset();
    void addScore(uint8_t lines);
    void saveScore();
    void drawInfo();
    void drawNextBlock();
    void drawGameOver();
    
    // Event handlers
    void handleSetup();
    void handleCheckGameOver(const Block& currentBlock, const GameBoard& board);
    void handleReset();
    void handleLinesCleared(uint8_t lines);
    
    // Getters
    uint32_t getScore() const { return score; }
    uint32_t getLevel() const { return level; }
    uint32_t getLinesCleared() const { return linesCleared; }
    bool getIsGameOver() const { return isGameOver; }
    BlockType getNextBlockType() const { return nextBlockType; }
    
    // Setters
    void setGameOver(bool value) { isGameOver = value; }
    void generateNextBlockType();
};

/*=============================== TETRIS GAME MANAGER =======================================*/
class TetrisGameManager
{
private:
    static TetrisGameManager* instance;
    GameBoard* board;
    Block* currentBlock;
    GameState* state;

    TetrisGameManager();

public:
    ~TetrisGameManager();
    static TetrisGameManager* getInstance();
    static void destroyInstance();

    void init();
    void update();
    void render();

    GameBoard* getBoard() {return board;}
    GameState* getState() {return state;}
    Block* getCurrentBlock() {return currentBlock;}

    void handleGameSetup();
    void handleGameUpdate();
    void handleGameReset();
    void handleSpawnBlock();
};

inline TetrisGameManager* getTetrisGameManager()
{
    return TetrisGameManager::getInstance();
}

#endif // _TT_GAME_BLOCK_H
