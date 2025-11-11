# Archery Game - Build on AK Embedded Base Kit

## I. Giới thiệu

Tetris game là một tựa game chạy trên AK Embedded Base Kit.

### 1.1 Phần cứng

<p align="center"><img src="https://github.com/epcbtech/archery-game/blob/main/resources/images/AK_Embedded_Base_Kit_STM32L151.webp" alt="AK Embedded Base Kit - STM32L151" width="480"/></p>
<p align="center"><strong><em>Hình 1:</em></strong> AK Embedded Base Kit - STM32L151</p>

[AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) là một evaluation kit dành cho các bạn học phần mềm nhúng nâng cao.

KIT tích hợp LCD **OLED 1.3", 3 nút nhấn, và 1 loa Buzzer phát nhạc**, với các trang bị này thì đã đủ để học hệ thống event-driven thông qua thực hành thiết kế máy chơi game.

KIT cũng tích hợp **RS485**, **NRF24L01+**, và **Flash** lên đến 32MB, thích hợp cho prototype các ứng dụng thực tế trong hệ thống nhúng hay sử dụng như: truyền thông có dây, không dây wireless, các ứng dụng lưu trữ data logger,...

### 1.2 Mô tả trò chơi và đối tượng
Phần mô tả sau đây về **“Tetris game”** , giải thích cách chơi và cơ chế xử lý của trò chơi. Tài liệu này dùng để tham khảo thiết kế và phát triển trò chơi về sau.

<p align="center"><img src="https://github.com/KhThuan/TetrisGame/blob/main/resources/bin/screen_menu.png" alt="Menu game" width="480"/></p>
<p align="center"><strong><em>Hình 2:</em></strong> Menu game</p>

Trò chơi bắt đầu bằng màn hình **Menu game** với các lựa chọn sau: 
- **:** Chọn vào để bắt đầu chơi game.
- **Setting:** Chọn vào để cài đặt các thông số của game.
- **Charts:** Chọn vào để xem top 3 điểm cao nhất đạt được.
- **Exit:** Thoát menu vào màn hình chờ.


#### 1.2.1 Các đối tượng (Object) trong game:
|Đối tượng|Tên đối tượng|Ý nghĩa|
|---|---|---|
|**Tetromino**|Block|Gồm 7 khối và di chuyển từ trên xuống|
|**Board**|Board|Lưới chơi|
|**Game State**|Trạng thái game|Hoạt động dựa trên các tín hiệu|
|**Tetris Game Manager**|Lớp quản lý|Lớp trung tâm được các task sử dụng trong quá trình xử lý dữ liệu|


#### 1.2.2 Cách chơi game: 
- Trong trò chơi này bạn sẽ điều khiển Block, di chuyển **trái/phải** bằng hai nút **[Down]/[Up]**.
- Khi nhấn nút **[Mode]** Block sẽ xoay một góc 90 độ.

- Mục tiêu trò chơi là kiếm được càng nhiều điểm càng tốt, trò chơi sẽ kết thúc khi có  chạm vào Border.

#### 1.2.3 Cơ chế hoạt động:

- Cứ sau mỗi khoảng thời gian ngắn, khối Tetromino sẽ rơi xuống thêm một ô. Sau khi rơi xuống chạm đáy hoặc chạm vào các khối đã rơi xuống trước, khối hiện tại sẽ bị gắn lại và một khối mới sẽ rơi xuống.

- Khi một hàng được "hoàn thành", hay được lấp đầy bởi các khối, bạn sẽ được cộng điểm. Với càng nhiều hàng được hoàn thành một lúc, bạn càng được nhiều điểm, tối đa là 4 hàng với khối chữ I.

- Các hàng đã được hoàn thành sau đó sẽ được xóa khỏi bảng, cùng với khiến các khối ô ở phía trên nó thấp xuống dưới.

- Trò chơi kết thúc khi lượng khối đã rơi xuống chồng chất lên đến mức chạm vào cạnh trên cùng của board, và khối mới không thể rơi xuống được nữa.

- **Kết thúc trò chơi:** Kết thúc trò chơi: Khi Block chạm vào Board, trò chơi sẽ kết thúc. Các đối tượng sẽ được reset và số điểm sẽ được lưu. Bạn sẽ vào màn hình “Game Over” với 3 lựa chọn là: 
  - **Restart:** chơi lại.
  - **Charts:** vào xem bảng xếp hạng.
  - **Home:** về lại menu game.

## II. Thiết kế - TETRIS GAME
### 2.1 Sơ đồ trình tự
**Sơ đồ trình tự** được sử dụng để mô tả trình tự của các Message và luồng tương tác giữa các đối tượng trong một hệ thống.

<p align="center"><img src="https://github.com/KhThuan/TetrisGame/blob/main/resources/bin/image.png" alt="aTetris game UML" width="720"/></p>
<p align="center"><strong><em>Hình 3:</em></strong> The sequence diagram</p>

### Ghi chú:
**SCREEN_ENTRY:** Cài đặt các đối tượng trong game.
- TT_GAME_BOARD_SETUP: Thiết lập thông số ban đầu cho Board.
- TT_GAME_BLOCK_SETUP: Thiết lập thông số ban đầu cho đối tượng Block.
- TT_GAME_STATE_SETUP: Thiết lập thông số ban đầu cho Game State.
- Setup timer - Time tick: Khởi tạo Timer - Time tick cho game.
- STATE (GAME_ON): Cập nhật trạng thái game -> GAME_ON
- Level setup: Thiết lập thông số cấp độ game.

**GAME PLAY - NORMAL**: GAME hoạt động ở trạng thái bình thường
- TT_GAME_BLOCK_MOVE_DOWN: Cập nhật di chuyển cho Block
- TT_GAME_CHECK_GAME_OVER: Cập nhật kiểm tra Game Over.
- TT_GAME_BLOCK_LOCK: Khóa block khi đã rơi xuống đáy.
- TT_GAME_BOARD_CHECK_LINES: Board kiểm tra hàng.
- TT_GAME_STATE_CHECK_GAME_OVER: Game State kiểm tra game over mỗi khi có khối rơi.

**GAME PLAY - ACTION:** Game hoạt động ở trạng thái có nút nhấn.
- TT_GAME_BLOCK_MOVE_RIGHT: Player nhấn nút Up điều khiển block di chuyển sang phải
- TT_GAME_BLOCK__MOVE_LEFT: Player nhấn nút Down điều khiển block di chuyển sang trái
- TT_GAME_BLOCK_ROTATE: Player nhấn nút Mode điều khiển block xoay.

**RESET GAME:** Đặt lại các thông số trước khi thoát game
- STATE(GAME_OVER): Cập nhật trạng thái game -> GAME_OVER
- TT_GAME_STATE_CHECK_GAME_OVER: kiểm tra game over.
- TT_GAME_RESET: Signal cài đặt lại game do Border gửi đến.
- TT_GAME_BOARD_RESET: cài đặt lại thông số cho Board.
- TT_GAME_STATE_RESET: cài đặt lại các thông số cho State.
- TT_GAME_BLOCK_RESET: cài đặt lại các thông số cho Block.

- Save and reset Score: Lưu số điểm hiện tại và cài đặt lại.
- Timer remove - Timer exit: xóa timer - time tick
- Setup timer - Timer exit: Tạo 1 timer one shot để thoát game. Nhằm tạo ra một khoảng delay cho người chơi có thể nhận thức được là mình đã game over trước khi chuyển sang màn hình thông báo game over.

**EXIT**: Thoát khỏi game và chuyển sang màn hình Game Over.
- TT_GAME_EXIT: Signal do Timer exit gửi đến.
- STATE (GAME_OFF): Cập nhật trạng thái game -> GAME_OFF
- Change the screen - SCREEN_TRAN(scr_game_over_handle, &scr_game_over): Chuyển màn hình sang màn hình Game Over..

### 2.2 Chi tiết

Sau khi xác định được các đối tượng trong game mà chúng ta cần, tiếp theo chúng ta phải liệt kê ra các thuộc tính, các task, các signal và bitmap mà trong game sẽ sử dụng tới.
Việc liệt kê càng chi tiết thì việc làm game diễn ra càng nhanh và tạo tình rõ ràng minh bạch cho phần tài nguyên giúp phần code game diễn ra suông sẻ hơn.

#### 2.2.1 Thuộc tính đối tượng
Việc liệt kê các thuộc tính của đối tượng trong game có các tác dụng quan trọng sau:
- Giúp xác định rõ thông tin về đối tượng trong game.
- Giúp xác định cấu trúc dữ liệu phù hợp để lưu trữ thông tin của đối tượng.
- Khi bạn xác định trước các thuộc tính cần thiết, bạn giảm thiểu khả năng bỏ sót hoặc nhầm lẫn trong việc xử lý và sử dụng các thuộc tính.

**Các biến quan trọng:**
- **tt_game_score:** Điểm của trò chơi.
- **tt_game_status:** Trạng thái quả trò chơi.
  - GAME_OFF: Tắt .
  - GAME_ON: Bật.
  - GAME_OVER: Đã thua.

- **tt_game_setting_t** settingsetup : Cấu hình cấp độ của trò chơi.
  - settingsetup.silent : Bật/tắt chế độ im lặng.
  - settingsetup.block_speed : Cấu hình tốc độ các khối tetromino

## III. Hướng dẫn chi tiết code trong đối tượng
### 3.1 Block
**Sequence diagram:**

<p align="center"><img src="https://github.com/KhThuan/TetrisGame/blob/main/resources/bin/sequence_block.png" alt="Block sequence" width="640"/></p>
<p align="center"><strong><em>Hình 4:</em></strong> Block sequence</p>

**Tóm tắt nguyên lý:** Tetris sẽ nhận Signal thông được gửi từ 2 nguồn là Screen và Button. Quá trình xử lý của đối tượng phần làm 3 giai đoạn:
- **Giai đoạn 1:** Bắt đầu game, cài đặt các thông số của Tetris như vị trí và hình ảnh.
- **Giai đoạn 2:** Chơi game, trong giai đoạn này chia làm 2 hoạt động là:
  - Cập nhật: Screen gửi Signal cập nhật cho Block mỗi 500ms để cập nhật trạng thái hiện tại của Block.
  - Hành động: Button gửi Signal di chuyển trái/phải cho Block mỗi khi nhấn nút.
- **Giai đoạn 3:** Kết thúc game, thực hiện cài đặt lại trạng thái của Block trước khi thoát game.

**Code:**


### 3.2 Board 

**Sequence diagram:**

<p align="center"><img src="https://github.com/KhThuan/TetrisGame/blob/main/resources/bin/squence_board.png" alt="board sequence" width="640"/></p>
<p align="center"><strong><em>Hình 5:</em></strong> Board sequence</p>

**Tóm tắt nguyên lý:** Board sẽ nhận Signal thông được gửi từ 2 nguồn là Screen và Block. Quá trình xử lý của đối tượng phần làm 3 giai đoạn:
- **Giai đoạn 1:** Bắt đầu game, cài đặt các thông số của Board.

- **Giai đoạn 2:** Chơi game, trong giai đoạn này:
  - Cập nhật: Mỗi khi một Block chạm đáy thì board sẽ khóa khối và gọi hàm kiểm tra hàng đầy.

### 3.3 GameState

**Sequence diagram:**

<p align="center"><img src="https://github.com/KhThuan/TetrisGame/blob/main/resources/bin/squence_state.png" alt="State sequence" width="640"/></p>
<p align="center"><strong><em>Hình 6:</em></strong>State sequence</p>

**Tóm tắt nguyên lý:** GameState sẽ nhận Signal từ Screen.

- **Giai đoạn 1:** Bắt đầu game, cài đặt các thông số score, level, nextBlock lên màn hình.

- **Giai đoạn 2:** Chơi game, khi một Block được tạo ra sẽ gọi hàm handleCheckGameOver để xem khối có va chạm vào phần trên cùng của màn hình không.

- **Giai đoạn 3:** Kết thúc game, thực hiện cài đặt lại các thông số trước khi thoát game.

### 3.4 TetrisGameManager

**Tóm tắt nguyên lý:** Border là 1 lớp đối tượng quản lý và không nhận cũng như xử lý tín hiệu.

## IV. Hiển thị và âm thanh trong trò chơi bắn cung
### 4.1 Đồ họa

Trong trò chơi, màn hình hiện thị là 1 màn hình **LCD OLed 1.3"** có kích thước là **128px*64px**. Nên các đối tượng được hiển thị trong game phải có kích thước hiển thị phù hợp với màn hình nên cần được thiết kế riêng. 

Đồ họa được thiết kế từng phần theo từng đối tượng bằng phần mềm [Pixilart](https://www.pixilart.com/)

#### 4.1.1 Thiết kế đồ họa cho các đối tượng

**Bitmap** là một cấu trúc dữ liệu được sử dụng để lưu trữ và hiển thị hình ảnh trong game.

**Animation** là ứng dụng việc nối ảnh của của nhiều ảnh liên tiếp tạo thành hoạt ảnh cho đổi tượng muốn miêu tả. Trong game, biến “action_image” trong đối tượng được sử dụng nối các ảnh theo thứ tự tạo thành animation.

**Ghi chú:** Trong thiết kế trên có nhiều ảnh khác nhau cho cùng 1 đối tượng để tạo animation cho đối tượng đó nhằm tăng tính chân thật lúc chơi game.

#### 4.1.2 Code

**Block display:**
```sh
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
```

**Board display:**
```sh
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
```

**Game State display:**
```sh
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
```

</details>

### 4.2 Âm thanh
Âm thành được thiết kế qua website [Arduino Music](https://www.instructables.com/Arduino-Music-From-Sheet-Music/)

Trong khi chơi, để trò chơi thêm phần sinh động và chân thật thì việc có âm thanh là điều cần thiết. 

Các âm thanh cần thiết kế: nút nhấn, bắn tên, vụ nổ, nhạc game.

**Code:**

```sh
// Âm thanh Bắt đầu game 
BUZZER_PlayTones(tones_SMB);

// Âm thanh Vụ nổ 
BUZZER_PlayTones(tones_BUM);

// Âm thanh nút nhấn
BUZZER_PlayTones(tones_cc);

// Âm thanh cảnh báo
BUZZER_PlayTones(tones_3beep);

// Merry Christmas
BUZZER_PlayTones(tones_merryChristmas);

/*________________BUZZER______________*/

void BUZZER_Sleep(bool sleep);
/*  sleep = 0 : bật âm thanh 
    sleep = 1 : tắt âm thanh */
static const Tone_TypeDef tones_BUM[] = {
    {3000,3},
    {4500,6},
    {   0,0}
};

static const Tone_TypeDef tones_cc[] = {
    {2000,2}, 
    {   0,0}, 
};

static const Tone_TypeDef tones_startup[] = {
    {2000,3},
    {   0,3},
    {3000,3},
    {   0,3},
    {4000,3},
    {   0,3},
    {1200,4},
    {   0,6},
    {4500,6},
    {   0,0}     // <-- tones end
};

static const Tone_TypeDef tones_3beep[] = {
    {4000, 3},
    {   0,10},
    {1000, 6},
    {   0,10},
    {4000, 3},
    {   0, 0}
};

// "Super Mario bros." =
static const Tone_TypeDef tones_SMB[] = {
    {2637,18}, // E7 x2
    {   0, 9}, // x3
    {2637, 9}, // E7
    {   0, 9}, // x3
    {2093, 9}, // C7
    {2637, 9}, // E7
    {   0, 9}, // x3
    {3136, 9}, // G7
    {   0,27}, // x3
    {1586, 9}, // G6
    {   0,27}, // x3

    {2093, 9}, // C7
    {   0,18}, // x2
    {1586, 9}, // G6
    {   0,18}, // x2
    {1319, 9}, // E6
    {   0,18}, // x2
    {1760, 9}, // A6
    {   0, 9}, // x1
    {1976, 9}, // B6
    {   0, 9}, // x1
    {1865, 9}, // AS6
    {1760, 9}, // A6
    {   0, 9}, // x1

    {1586,12}, // G6
    {2637,12}, // E7
    {3136,12}, // G7
    {3520, 9}, // A7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {3136, 9}, // G7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2093, 9}, // C7
    {2349, 9}, // D7
    {1976, 9}, // B6
    {   0,18}, // x2

    {2093, 9}, // C7
    {   0,18}, // x2
    {1586, 9}, // G6
    {   0,18}, // x2
    {1319, 9}, // E6
    {   0,18}, // x2
    {1760, 9}, // A6
    {   0, 9}, // x1
    {1976, 9}, // B6
    {   0, 9}, // x1
    {1865, 9}, // AS6
    {1760, 9}, // A6
    {   0, 9}, // x1

    {1586,12}, // G6
    {2637,12}, // E7
    {3136,12}, // G7
    {3520, 9}, // A7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {3136, 9}, // G7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2093, 9}, // C7
    {2349, 9}, // D7
    {1976, 9}, // B6

    {   0, 0}
};

// Merry Christmas
static const Tone_TypeDef tones_merryChristmas[] = {
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0,18}, // x2

    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0,18}, // x2

    {2637, 9}, // E7
    {   0, 9}, // x1
    {3136, 9}, // G7
    {   0, 9}, // x1
    {2093, 9}, // C7
    {   0, 9}, // x1
    {2349, 9}, // D7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0,24}, // x2

    {2794, 9}, // F7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {   0, 9}, // x1
    {2794, 9}, // F7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2349, 9}, // D7
    {   0, 9}, // x1
    {2349, 9}, // D7
    {   0, 9}, // x1
    {2637, 9}, // E7
    {   0, 9}, // x1
    {2349, 9}, // D7
    {   0, 9}, // x1
    {3136, 9}, // G7
    {   0, 0}  // <-- tones end
};
```

</details>

# TetrisGame
