#include "tt_game_block.h"
#include "buzzer.h"
#include <cstring>
#include <cstdlib>

// Shape database
static const uint8_t (*SHAPE_DATABASE[NUM_BLOCK_TYPES])[4][MATRIX_SIZE][MATRIX_SIZE] = {
    &I_SHAPES, &J_SHAPES, &L_SHAPES, &T_SHAPES, &O_SHAPES, &S_SHAPES, &Z_SHAPES
};

/*==================== CLASS: GameBoard Implementation ====================*/
GameBoard::GameBoard() {
    clear();
}

GameBoard::~GameBoard() {}

void GameBoard::clear() {
    memset(cells, 0, sizeof(cells));
}

void GameBoard::draw() {
    // Draw border
    view_render.drawRect(0, 0, 128, 64, WHITE);

    // Draw locked blocks
    for(uint8_t x = 0; x < GAME_BOARD_WIDTH; x++) {
        for (uint8_t y = 0; y < GAME_BOARD_HEIGHT; y++) {
            if (cells[y][x] > 0) {
                uint8_t screenX = GAME_BOARD_X_OFFSET + x * BLOCK_SIZE;
                uint8_t screenY = GAME_BOARD_Y_OFFSET + y * BLOCK_SIZE;
                view_render.fillRect(screenX, screenY, BLOCK_SIZE - 1, BLOCK_SIZE - 1, WHITE);
            }
        }
    }
}

bool GameBoard::isCellOccupied(int8_t x, int8_t y) const {
    if (x < 0 || x >= GAME_BOARD_WIDTH || y < 0 || y >= GAME_BOARD_HEIGHT) {
        return true;
    }
    return cells[y][x] != 0;
}

void GameBoard::setCell(int8_t x, int8_t y, uint8_t value) {
    if (x >= 0 && x < GAME_BOARD_WIDTH && y >= 0 && y < GAME_BOARD_HEIGHT) {
        cells[y][x] = value;
    }
}

uint8_t GameBoard::getCell(int8_t x, int8_t y) const {
    if (x >= 0 && x < GAME_BOARD_WIDTH && y >= 0 && y < GAME_BOARD_HEIGHT) {
        return cells[y][x];
    }
    return 0;
}

//Check row full
bool GameBoard::isLineFull(uint8_t line) const {
    for (uint8_t i = 0; i < GAME_BOARD_WIDTH; i++) {
        if (cells[line][i] == 0) {
            return false;
        }
    }
    return true;
}

void GameBoard::clearLine(uint8_t line) {
    // Move all lines above down
    for (int8_t i = line; i > 0; i--) {
        for (uint8_t j = 0; j < GAME_BOARD_WIDTH; j++) {
            cells[i][j] = cells[i-1][j];
        }
    }
    
    // Clear top line
    for (uint8_t i = 0; i < GAME_BOARD_WIDTH; i++) {
        cells[0][i] = 0;
    }
}

//Check and deleted row full
uint8_t GameBoard::checkAndClearLines() {
    uint8_t clearedLines = 0;
    
    for (int8_t i = GAME_BOARD_HEIGHT - 1; i >= 0; i--) {
        if (isLineFull(i)) {
            clearLine(i);
            clearedLines++;
            i++; 
        }
    }
    
    return clearedLines;
}

void GameBoard::handleSetup() {
    clear();
}

//Check row deleted and add score
void GameBoard::handleCheckLines() {
    uint8_t cleared = checkAndClearLines();
    if(cleared > 0)
    {
        task_post_data(TT_GAME_STATE_ID, 
                       TT_GAME_STATE_LINES_CLEARED,
                       &cleared,
                       sizeof(cleared));
    }
    if (!getTetrisGameManager()->getState()->getIsGameOver()) {
        task_post_pure_msg(TT_GAME_SCREEN_ID, TT_GAME_SPAWN_BLOCK);
    }
}

void GameBoard::handleReset() {
    clear();
}

/*==================== CLASS: Block Implementation ====================*/
Block::Block() {
    x = 0;
    y = 0;
    rotation = 0;
    type = BLOCK_I;
    visible = false;
    shapeData = nullptr;
}

Block::Block(BlockType blockType) {
    setup(blockType);
}

Block::~Block() {}

void Block::setup(BlockType blockType) {
    type = blockType;
    rotation = 0;
    x = (GAME_BOARD_WIDTH / 2) - (MATRIX_SIZE / 2);
    y = 0;
    visible = true;
    loadShape();
}

void Block::loadShape() {
    shapeData = SHAPE_DATABASE[type];
}

void Block::draw() {
    if (!visible || shapeData == nullptr) return;
    
    for (uint8_t i = 0; i < MATRIX_SIZE; i++) {
        for (uint8_t j = 0; j < MATRIX_SIZE; j++) {
            if ((*shapeData)[rotation][i][j] == 1) {
                uint8_t screenX = GAME_BOARD_X_OFFSET + (x + i) * BLOCK_SIZE; 
                uint8_t screenY = GAME_BOARD_Y_OFFSET + (y + j) * BLOCK_SIZE;   
                view_render.fillRect(screenX, screenY, BLOCK_SIZE - 1, BLOCK_SIZE - 1, WHITE);
                view_render.drawRect(screenX, screenY, BLOCK_SIZE - 1, BLOCK_SIZE - 1, BLACK);
            }
        }
    }
}

//Check collision with board and block else
bool Block::checkCollision(const GameBoard& board, int8_t offsetX, int8_t offsetY, uint8_t rot) const {
    if (shapeData == nullptr) return true;
    
    for (uint8_t i = 0; i < MATRIX_SIZE; i++) {
        for (uint8_t j = 0; j < MATRIX_SIZE; j++) {
            if ((*shapeData)[rot][i][j] == 1) {
                int8_t col = x + i + offsetX;
                int8_t row = y + j + offsetY;
                if (board.isCellOccupied(col, row)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//Lock to block with board
void Block::lockToBoard(GameBoard& board) {
    if (shapeData == nullptr) return;
    
    for (uint8_t i = 0; i < MATRIX_SIZE; i++) {
        for (uint8_t j = 0; j < MATRIX_SIZE; j++) {
            if ((*shapeData)[rotation][i][j] == 1) {
                board.setCell(x + i, y + j, type + 1);
            }
        }
    }
    visible = false;
}

bool Block::rotate(const GameBoard& board) {
    uint8_t newRotation = (rotation + 1) % 4;
    
    if (!checkCollision(board, 0, 0, newRotation)) {
        rotation = newRotation;
        return true;
    }
    return false;
}

bool Block::moveLeft(const GameBoard& board) {
    if (!checkCollision(board, -1, 0, rotation)) {
        x--;
        return true;
    }
    return false;
}

bool Block::moveRight(const GameBoard& board) {
    if (!checkCollision(board, 1, 0, rotation)) {
        x++;
        return true;
    }
    return false;
}

bool Block::moveDown(const GameBoard& board) {
    if (!checkCollision(board, 0, 1, rotation)) {
        y++;
        return true;
    }
    return false;
}

// void Block::hardDrop(const GameBoard& board) {
//     while (!checkCollision(board, 0, 1, rotation)) {
//         y++;
//     }
// }

void Block::handleSetup(BlockType blockType) {
    setup(blockType);
}

void Block::handleRotate(const GameBoard& board) {
    if (rotate(board)) {
    }
}

void Block::handleMoveLeft(const GameBoard& board) {
    moveLeft(board);
}

void Block::handleMoveRight(const GameBoard& board) {
    moveRight(board);
}

void Block::handleMoveDown(const GameBoard& board) {
    if (!moveDown(board)) {
        task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_LOCK);
    }
}

void Block::handleHardDrop(const GameBoard& board) {
    hardDrop(board);
    task_post_pure_msg(TT_GAME_BLOCK_ID, TT_GAME_BLOCK_LOCK);
}

void Block::handleLock(GameBoard& board) {
    lockToBoard(board);
    task_post_pure_msg(TT_GAME_BOARD_ID, TT_GAME_BOARD_CHECK_LINES);
}

void Block::handleReset() {
    x = 0;
    y = 0;
    rotation = 0;
    visible = false;
}

/*==================== CLASS: GameState Implementation ====================*/
GameState::GameState() {
    init();
}

GameState::~GameState() {}

void GameState::init() {
    score = 0;
    level = 1;
    linesCleared = 0;
    isGameOver = false;
    generateNextBlockType();
}

void GameState::reset() {
    init();
}

void GameState::generateNextBlockType() {
    nextBlockType = (BlockType)(rand() % NUM_BLOCK_TYPES);
}

void GameState::addScore(uint8_t lines) {
    if (lines > 0) {
        linesCleared += lines;
        score += lines * lines * 10 * level;
        level = 1 + linesCleared / 10;
    }
}

void GameState::drawInfo() {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(1, 5);
    view_render.print("SCORE");
    view_render.setCursor(1, 15);
    view_render.print(score);
    view_render.setCursor(2, 27);
    view_render.print("LV");
    view_render.print(level);

    //draw line border
    view_render.drawLine(35, 0, 35, 64, WHITE);
}

void GameState::drawNextBlock() {
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(1, 43);
    view_render.print("NEXT");
    view_render.fillRect(2, 12, 12, 12, BLACK);
    const uint8_t (*shape)[4][MATRIX_SIZE][MATRIX_SIZE] = SHAPE_DATABASE[nextBlockType];
    
    for (uint8_t i = 0; i < MATRIX_SIZE; i++) {
        for (uint8_t j = 0; j < MATRIX_SIZE; j++) {
            if ((*shape)[0][i][j] == 1) {
                uint8_t screenX = 10 + j * 3;
                uint8_t screenY = 55 + i * 3;
                view_render.fillRect(screenX, screenY, 2, 2, WHITE);
            }
        }
    }
}

void GameState::drawGameOver() {
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(15, 20);
    view_render.print("GAME");
    view_render.setCursor(15, 36);
    view_render.print("OVER");
    
    view_render.setTextSize(1);
    view_render.setCursor(10, 52);
    view_render.print("Score:");
    view_render.print(score);
}

void GameState::handleSetup() {
    init();
}

void GameState::handleCheckGameOver(const Block& currentBlock, const GameBoard& board) {
    if (currentBlock.checkCollision(board, 0, 0, currentBlock.getRotation())) {
        isGameOver = true;
        task_post_pure_msg(TT_GAME_SCREEN_ID, TT_GAME_EXIT_GAME);
    }
}

void GameState::handleReset() {
    reset();
}

void GameState::handleLinesCleared(uint8_t lines) {
    addScore(lines);
}

/*==================== CLASS: TetrisGame Implementation (Singleton) ====================*/
TetrisGameManager* TetrisGameManager::instance = nullptr;

TetrisGameManager::TetrisGameManager() {
    board = new GameBoard();
    currentBlock = new Block();
    state = new GameState();
}

TetrisGameManager::~TetrisGameManager() {
    if (board) delete board;
    if (currentBlock) delete currentBlock;
    if (state) delete state;
}

/*Hàm đảm bảo chỉ có 1 đối tượng của Tetris Manager được tồn tại*/
TetrisGameManager* TetrisGameManager::getInstance() {
    if (instance == nullptr) {
        instance = new TetrisGameManager();
    }
    return instance;
}

void TetrisGameManager::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void TetrisGameManager::init() {
    state->handleSetup();
    board->handleSetup();
    currentBlock->handleSetup(state->getNextBlockType());
    state->generateNextBlockType();
}

void TetrisGameManager::update() {
    if (!state->getIsGameOver()) {
        currentBlock->handleMoveDown(*board);
    }
}

void TetrisGameManager::render() {
    if (state->getIsGameOver()) {
        state->drawGameOver();
    } else {
        board->draw();
        currentBlock->draw();
        state->drawNextBlock();
        state->drawInfo();
    }
}

void TetrisGameManager::handleGameSetup() {
    init();
}

void TetrisGameManager::handleGameUpdate() {
    update();
}

void TetrisGameManager::handleGameReset() {
    state->handleReset();
    board->handleReset();
    currentBlock->handleReset();
}

void TetrisGameManager::handleSpawnBlock() {
    currentBlock->handleSetup(state->getNextBlockType());
    state->generateNextBlockType();
    task_post_pure_msg(TT_GAME_STATE_ID, TT_GAME_STATE_CHECK_GAME_OVER);
}

/*==================== TASK HANDLERS (Event-Driven) ====================*/
void tt_game_block_handle(ak_msg_t* msg) {
    TetrisGameManager* game = getTetrisGameManager();
    Block* block = game->getCurrentBlock();
    GameBoard* board = game->getBoard();
    
    switch (msg->sig) {
        case TT_GAME_BLOCK_SETUP: {
            APP_DBG_SIG("TT_GAME_BLOCK_SETUP\n");
            BlockType nextType = game->getState()->getNextBlockType();
            block->handleSetup(nextType);
            game->getState()->generateNextBlockType();
        }
        break;
        
        case TT_GAME_BLOCK_ROTATE: {
            APP_DBG_SIG("TT_GAME_BLOCK_ROTATE\n");
            block->handleRotate(*board);
        }
        break;
        
        case TT_GAME_BLOCK_MOVE_LEFT: {
            APP_DBG_SIG("TT_GAME_BLOCK_MOVE_LEFT\n");
            block->handleMoveLeft(*board);
        }
        break;
        
        case TT_GAME_BLOCK_MOVE_RIGHT: {
            APP_DBG_SIG("TT_GAME_BLOCK_MOVE_RIGHT\n");
            block->handleMoveRight(*board);
        }
        break;
        
        case TT_GAME_BLOCK_MOVE_DOWN: {
            APP_DBG_SIG("TT_GAME_BLOCK_MOVE_DOWN\n");
            block->handleMoveDown(*board);
        }
        break;
        
        case TT_GAME_BLOCK_HARD_DROP: {
            APP_DBG_SIG("TT_GAME_BLOCK_HARD_DROP\n");
            block->handleHardDrop(*board);
        }
        break;
        
        case TT_GAME_BLOCK_LOCK: {
            APP_DBG_SIG("TT_GAME_BLOCK_LOCK\n");
            block->handleLock(*board);
        }
        break;
        
        case TT_GAME_BLOCK_RESET: {
            APP_DBG_SIG("TT_GAME_BLOCK_RESET\n");
            block->handleReset();
        }
        break;
        
        default:
            break;
    }
}

void tt_game_board_handle(ak_msg_t* msg) {
    TetrisGameManager* game = getTetrisGameManager();
    GameBoard* board = game->getBoard();
    
    switch (msg->sig) {
        case TT_GAME_BOARD_SETUP: {
            APP_DBG_SIG("TT_GAME_BOARD_SETUP\n");
            board->handleSetup();
        }
        break;
        
        case TT_GAME_BOARD_CHECK_LINES: {
            APP_DBG_SIG("TT_GAME_BOARD_CHECK_LINES\n");
            board->handleCheckLines();
        }
        break;
        
        case TT_GAME_BOARD_RESET: {
            APP_DBG_SIG("TT_GAME_BOARD_RESET\n");
            board->handleReset();
        }
        break;
        
        default:
            break;
    }
}

void tt_game_state_handle(ak_msg_t* msg) {
    TetrisGameManager* game = getTetrisGameManager();
    GameState* state = game->getState();
    
    switch (msg->sig) {
        case TT_GAME_STATE_SETUP: {
            APP_DBG_SIG("TT_GAME_STATE_SETUP\n");
            state->handleSetup();
        }
        break;
        
        case TT_GAME_STATE_CHECK_GAME_OVER: {
            APP_DBG_SIG("TT_GAME_STATE_CHECK_GAME_OVER\n");
            state->handleCheckGameOver(*game->getCurrentBlock(), *game->getBoard());
        }
        break;
        
        case TT_GAME_STATE_RESET: {
            APP_DBG_SIG("TT_GAME_STATE_RESET\n");
            state->handleReset();
        }
        break;
        
        default:
            break;
    }
}
