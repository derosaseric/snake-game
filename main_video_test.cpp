/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include <cstdlib>
#include <ctime>
#include <random>
#include "xadc_core.h"

// Function Prototypes
void pause_game(OsdCore *osd_p);
void move_tail(int tailX[], int tailY[], int numTail, SpriteCore* snake_p);
void update_tail(int tailX[], int tailY[], int &numTail, int x, int y);
int speedMul(double potValue);
void check_apple(SpriteCore *apple,int *x, int *y, int *apple_x_location, int *apple_y_location, int *game_score, unsigned int *timer, const int *MAX_X_COORD, const int *MAX_Y_COORD);

void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * test snake sprite
 * @param snake_p pointer to snake sprite instance
 */
void snake_check(SpriteCore *snake_p, char *direction, int *x, int *y, int *game_time_remaining, XadcCore *adc_p, SpriteCore *apple, int *apple_x_location, int *apple_y_location, int *game_score, unsigned int *timer, const int *MAX_X_COORD, const int *MAX_Y_COORD) {
	// slowly move snake pointer
	snake_p->bypass(0);
	snake_p->wr_ctrl(0x19);  // blue snake right

	//Declare potValue Variable
		 double potValue = adc_p->read_adc_in(0);



	   // Sprite movement
	   if (*direction == 'a') { // Move left
		   // Change snake direction
		   snake_p->wr_ctrl(0x1b);  // blue snake left
		   potValue = adc_p->read_adc_in(0);
		   for (int i = 0; i < 5; i++){

			  			   *x = *x - (4 * speedMul(potValue));

			   // Move sprite
			   snake_p->move_xy(*x, *y);
			   // Delay movement of sprite to display smooth transition
			   sleep_ms(100);
			   check_apple(apple, x, y, apple_x_location, apple_y_location, game_score, timer, MAX_X_COORD, MAX_Y_COORD);
		   }

	   } else if (*direction == 'w') { // Move up
		   // Change snake direction
		   snake_p->wr_ctrl(0x18);  // blue snake up
		   potValue = adc_p->read_adc_in(0);
		   for (int i = 0; i < 5; i++){

			   			   *y = *y - (4 * speedMul(potValue));
			   // Move sprite
			   snake_p->move_xy(*x, *y);
			   // Delay movement of sprite to display smooth transition
			   sleep_ms(100);
			   check_apple(apple, x, y, apple_x_location, apple_y_location, game_score, timer, MAX_X_COORD, MAX_Y_COORD);
		   }

	   } else if (*direction == 's') { // Move down
		   // Change snake direction
		   snake_p->wr_ctrl(0x1A);  // blue snake down
		   potValue = adc_p->read_adc_in(0);
		   for (int i = 0; i < 5; i++){

			   			   *y = *y + (4 * speedMul(potValue));
			   // Move sprite
			   snake_p->move_xy(*x, *y);
			   // Delay movement of sprite to display smooth transition
			   sleep_ms(100);
			   check_apple(apple, x, y, apple_x_location, apple_y_location, game_score, timer, MAX_X_COORD, MAX_Y_COORD);
		   }

	   } else if (*direction == 'd'){ // Continue moving right
		   snake_p->wr_ctrl(0x19);  // blue snake right
		   potValue = adc_p->read_adc_in(0);
		   for (int i = 0; i < 5; i++){

			   			   *x = *x + (4 * speedMul(potValue));
			   // Move sprite
			   snake_p->move_xy(*x, *y);
			   // Delay movement of sprite to display smooth transition
			   sleep_ms(100);
			   check_apple(apple, x, y, apple_x_location, apple_y_location, game_score, timer, MAX_X_COORD, MAX_Y_COORD);
		   }
	   }

	   // Remove time remaining from game
	   *game_time_remaining -= 500;

} // End of snake_check

// Clone Check
void clone_check(SpriteCore *body_p, int *x, int *y, int *numTail, int tailX[], int tailY[]) {
	body_p->bypass(0);
    // Save the current head position as the new tail segment
        body_p->move_xy((*x + 20), *y + 20);
}

void move_tail(int tailX[], int tailY[], int numTail, SpriteCore* snake_p) {
    // Draw the tail segments
    for (int i = 0; i < numTail; ++i) {
        snake_p->move_xy(tailX[i], tailY[i]);
        // Additional code to draw the tail segment on the screen
    }
}

void update_tail(int tailX[], int tailY[], int& numTail, int x, int y) {
    // Save the current head position as the new tail segment
    if (numTail < 100) {
        tailX[numTail] = x;
        tailY[numTail] = y;
        numTail++;  // Update numTail through reference
    }
}

void display_background(FrameCore *frame_p) {
   int blocksize = 20; //20x20

   frame_p->bypass(0);

   //draw checker board pattern
   for (int hx = 0; hx < 640; hx++) {
       for (int hy = 0; hy < 480; hy += blocksize)
       {

           // Calculate the color based on the position
           int color = ((hx / blocksize) + (hy / blocksize)) % 2 == 0 ? 0x0A1 : 0x0F1;

           // Draw
           for (int i = 0; i < blocksize; i++)
           {
               frame_p->wr_pix(hx, hy + i, color);
           }
       }
   }
}

void osd_check(OsdCore *osd_p, Ps2Core *ps2_p, bool *start_game, bool *play_pause, bool *game_over, int *game_score, int *game_time_remaining) {
	// Declare score variables
	int tensDigits, onesDigits;
	char tensChar, onesChar;
	int tens_time, ones_time;
	char tens_time_char, ones_time_char;

   osd_p->set_color(0xFFF, 0x020); // white/dark green
   osd_p->bypass(0);
   osd_p->clr_screen();

   // "Score" text is display on the screen
   osd_p->wr_char(0, 0, 83, 1);  // S
   osd_p->wr_char(1, 0, 99, 1);  // c
   osd_p->wr_char(2, 0, 111, 1); // o
   osd_p->wr_char(3, 0, 114, 1); // r
   osd_p->wr_char(4, 0, 101, 1); // e
   osd_p->wr_char(5, 0, ':', 1); // :

   // Separate Tens and Ones digit
   tensDigits = (*game_score / 10 ) % 10;
   onesDigits = *game_score % 10;

   // Convert to characters
   tensChar = ps2_p->int_to_char(tensDigits);
   onesChar = ps2_p->int_to_char(onesDigits);

   // Score count is display on the screen
   osd_p->wr_char(6, 0, tensChar, 1);  // 0
   osd_p->wr_char(7, 0, onesChar, 1);  // 0

   // 'Timer' characters is display top middle
   osd_p->wr_char(35, 0, 'T', 1);
   osd_p->wr_char(36, 0, 'i', 1);
   osd_p->wr_char(37, 0, 'm', 1);
   osd_p->wr_char(38, 0, 'e', 1);
   osd_p->wr_char(39, 0, 'r', 1);
   osd_p->wr_char(40, 0, ':', 1);

   // Get time remaining and convert to characters
   tens_time = (*game_time_remaining / 10000 ) % 10;
   ones_time = (*game_time_remaining / 1000) % 10;

   tens_time_char = ps2_p->int_to_char(tens_time);
   ones_time_char = ps2_p->int_to_char(ones_time);

   // Display time value
   osd_p->wr_char(41, 0, tens_time_char, 1);
   osd_p->wr_char(42, 0, ones_time_char, 1);

   // Displays when player is not ready to play
	if (!*start_game) {
		// Gap for the space display
		for (int i = 0; i < 22; i++)
		   {
		            osd_p->wr_char(29 + i, 20, ' ');
		            osd_p->wr_char(29 + i, 22, ' ');
		   }

		   // Display "Press SPACE To Start"
		   osd_p->wr_char(29, 21, ' ');
		   osd_p->wr_char(30, 21, 'P');
		   osd_p->wr_char(31, 21, 'r');
		   osd_p->wr_char(32, 21, 'e');
		   osd_p->wr_char(33, 21, 's');
		   osd_p->wr_char(34, 21, 's');
		   osd_p->wr_char(35, 21, ' ');
		   osd_p->wr_char(36, 21, 'S');
		   osd_p->wr_char(37, 21, 'P');
		   osd_p->wr_char(38, 21, 'A');
		   osd_p->wr_char(39, 21, 'C');
		   osd_p->wr_char(40, 21, 'E');
		   osd_p->wr_char(41, 21, ' ');
		   osd_p->wr_char(42, 21, 'T');
		   osd_p->wr_char(43, 21, 'o');
		   osd_p->wr_char(44, 21, ' ');
		   osd_p->wr_char(45, 21, 'S');
		   osd_p->wr_char(46, 21, 't');
		   osd_p->wr_char(47, 21, 'a');
		   osd_p->wr_char(48, 21, 'r');
		   osd_p->wr_char(49, 21, 't');
		   osd_p->wr_char(50, 21, ' ');
		   //sleep_ms(3000);
	}

	// Display when player pauses the game
	if (*play_pause) {
		   // "Pause" text is display on the screen
		   osd_p->wr_char(35, 14, 80, 1);
		   osd_p->wr_char(36, 14, 97, 1);
		   osd_p->wr_char(37, 14, 117, 1);
		   osd_p->wr_char(38, 14, 115, 1);
		   osd_p->wr_char(39, 14, 101, 1);
	}

	// Display when the game is over
	if (*game_over){
		osd_p->set_color(0xF00, 0x000); // red/dark green
		   osd_p->bypass(0);
		   //osd_p->clr_screen();

		   // "GAME OVER" text is display on the screen
		   osd_p->wr_char(35, 15, 'G');
		   osd_p->wr_char(36, 15, 'A');
		   osd_p->wr_char(37, 15, 'M');
		   osd_p->wr_char(38, 15, 'E');
		   osd_p->wr_char(39, 15, ' ');
		   osd_p->wr_char(40, 15, 'O');
		   osd_p->wr_char(41, 15, 'V');
		   osd_p->wr_char(42, 15, 'E');
		   osd_p->wr_char(43, 15, 'R');

		   // Display "Press SPACE To Start"
		   osd_p->wr_char(28, 21, ' ');
		   osd_p->wr_char(29, 21, 'P');
		   osd_p->wr_char(30, 21, 'r');
		   osd_p->wr_char(31, 21, 'e');
		   osd_p->wr_char(32, 21, 's');
		   osd_p->wr_char(33, 21, 's');
		   osd_p->wr_char(34, 21, ' ');
		   osd_p->wr_char(35, 21, 'S');
		   osd_p->wr_char(36, 21, 'P');
		   osd_p->wr_char(37, 21, 'A');
		   osd_p->wr_char(38, 21, 'C');
		   osd_p->wr_char(39, 21, 'E');
		   osd_p->wr_char(40, 21, ' ');
		   osd_p->wr_char(41, 21, 'T');
		   osd_p->wr_char(42, 21, 'o');
		   osd_p->wr_char(43, 21, ' ');
		   osd_p->wr_char(44, 21, 'R');
		   osd_p->wr_char(45, 21, 'e');
		   osd_p->wr_char(46, 21, 's');
		   osd_p->wr_char(47, 21, 't');
		   osd_p->wr_char(48, 21, 'a');
		   osd_p->wr_char(49, 21, 'r');
		   osd_p->wr_char(50, 21, 't');
		   osd_p->wr_char(51, 21, ' ');
	}
}

int getRandomMultipleOf20(unsigned int *timer, const int *MAX_NUM) {
    // Seed the random number generator with the current time
    srand(static_cast<unsigned int>(*timer));

    // Generate a random number and ensure it is a multiple of 20
    int randomNumber = rand() % *MAX_NUM;
    int multipleOf20 = randomNumber * 20;

    return multipleOf20 + 2;
}

int speedMul(double potValue)
{

    int speed;

    if((potValue * 1000) < 256)
    {
        speed = 1;
    }
    if((potValue * 1000) >= 256 && (potValue * 1000) < 512)
    {
        speed = 2;
    }
    if((potValue * 1000) >= 512 && (potValue * 1000) < 767)
    {
        speed = 3;
    }
    if((potValue * 1000) >= 767)
    {
        speed = 4;
    }

    return speed;
}

void check_apple(SpriteCore *apple,int *x, int *y, int *apple_x_location, int *apple_y_location, int *game_score, unsigned int *timer, const int *MAX_X_COORD, const int *MAX_Y_COORD){
	if (*x >= *apple_x_location - 2 and *x <= *apple_x_location + 18  and *y >= *apple_y_location - 2 and *y <= *apple_y_location +18) {
				// Increase score and tail
				*game_score += 1;

				// Change apple location to random location
				*apple_x_location = getRandomMultipleOf20(timer, MAX_X_COORD);
				*apple_y_location = getRandomMultipleOf20(timer, MAX_Y_COORD);
				apple->move_xy(*apple_x_location, *apple_y_location);
	}
}
// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
FrameCore frame(FRAME_BASE);
SpriteCore snake(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore tail(get_sprite_addr(BRIDGE_BASE, V4_USER4), 1024);
SpriteCore body(get_sprite_addr(BRIDGE_BASE, V5_USER5), 1024);
SpriteCore apple(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));

// ====================================== Start of Main =================================================
int main() {
	// Constant initiaizations
	const char SPACEBAR = ' ';
	const char PAUSE_KEY = 'p';
	const int MAX_X_COORD = 31;  // Range: (31 * 20 = 620)
	const int MAX_Y_COORD = 23;  // Range: (23 * 20 = 460)
	const int TIME_REMAINING = 20000; // 20 seconds == 20000 ms

	// Timer for random tick
	unsigned int timer = 0;

	// Game timer (20 seconds is 20000)
	int game_time_remaining = TIME_REMAINING;

	// Initialize snake starting direction
	char user_input = '0';
	char direction = 'd'; // moving to the right
	char start_game_input = '0';

	// Create snake tail location arrays
	//int tailX[100] = {0};  // Initialize tail coordinates
	//int tailY[100] = {0};
	int numTail = 0;

	// Initialize game state
	bool game_over = false;
	bool play_pause = false;  // Play = 1 and Pause = 0

	// Apple start location
	int apple_x_location = 202;
	int apple_y_location = 102;

	// Snake start location
	int x = 2;
	int y = 102;

	// Initialize game score
	int game_score = 0;

	// Initialize ready state
	bool start_game = false;

	// Start
	test_start(&led);

	// Bypass all cores
	frame.bypass(1);
	snake.bypass(1);
	body.bypass(1);
	tail.bypass(1);
    apple.bypass(1);
	osd.bypass(1);
	sleep_ms(3000);

	// Display checker background
	display_background(&frame);

	// Display start menu
	osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);

	// Check for user ready state
	do {
		ps2.get_kb_ch(&start_game_input);
	} while(start_game_input != SPACEBAR);

	// Player is ready
	start_game = true;
	osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);

	// Spawn apple to given location
	apple.bypass(0);
	apple.move_xy(apple_x_location, apple_y_location);

	while (!game_over) {
		// enable cores one by one
		ps2.get_kb_ch(&user_input);

		// Check if user pauses game
		if (user_input == 'p') {
			// Display 'Pause' characters
			user_input = '0';
			play_pause = true;
			osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);

			// Pause the game until key 'p' pressed again
			do {
				ps2.get_kb_ch(&user_input);
			} while (user_input != PAUSE_KEY); //play_pause

			// Remove 'Pause' characters
			play_pause = false;
			osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);
			user_input = '0';
		  // Assign directions only for movement
		} else if (user_input == 'a' || user_input == 'w' || user_input == 's' || user_input == 'd') {
			direction = user_input;
		}

		// Move snake
		snake_check(&snake, &direction, &x, &y, &game_time_remaining, &adc, &apple,  &apple_x_location, &apple_y_location, &game_score, &timer, &MAX_X_COORD, &MAX_Y_COORD);

		// Clone snake
		//clone_check(&body, &x, &y, &numTail, tailX, tailY);

		// Check if the snake ate an apple to increase score
		//if (x == apple_x_location and y == apple_y_location){//(x >= apple_x_location - 2 and x <= apple_x_location + 18  and y >= apple_y_location - 2 and y <= apple_y_location +18) {
			// Increase score and tail
			//game_score += 1;

			// Change apple location to random location
			//apple_x_location = getRandomMultipleOf20(&timer, &MAX_X_COORD);
			//apple_y_location = getRandomMultipleOf20(&timer, &MAX_Y_COORD);
			//apple.move_xy(apple_x_location, apple_y_location);

			// Display Score
			osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);
		//}

		// Game Over if the snake is out of bounds
		if (x < 0 || x > 622 || y < 0 || y > 462 || game_time_remaining <= 0) {
			// Display Game Over Message
			game_over = true;
			osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);

			// Ask user to continue playing
			do {
				ps2.get_kb_ch(&user_input);
			} while (user_input != SPACEBAR); //play_pause

			// Restart game
			game_over = false;

			// Reset values
			x = 2;
			y = 102;
			timer = 0;
			game_score = 0;
			direction = 'd';
			user_input = '0';
			numTail = 0;
			game_time_remaining = TIME_REMAINING;

			// Update score to zero
			osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);
		}

		// Update score board
		osd_check(&osd, &ps2, &start_game, &play_pause, &game_over, &game_score, &game_time_remaining);

		// Increase timer for randomness tick
		timer += 1;
   } // End of while loop
} // ======================================= End of Main ======================================
