#include <stdlib.h>
#include <stdbool.h>
#include <math.h>	
#include <stdint.h>
#include <string.h>

//Declare Addresses
#define FPGA_PIXEL_BUF_BASE 0x08000000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define PS2_BASE 0xFF200100
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define AUDIO_BASE 0xFF203040
#define CLOCK_BASE 0xFF202100
#define CHAR_BASE 0x9000000
#define line_max 20
//Struct for Line
typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    Point p1;
    Point p2;
} Line;

//VGA boundaries
int x_max = 319;
int x_min = 0;
int y_max = 239;
int y_min = 10;

//VGA Variables
int pixel_buffer_start;
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

//Cursor Variables
int cursorx = 160;
int cursory = 120; 
int prev_cursorx = 160;
int prev_cursory = 120;
int direction_array[4] = {0};


//Ball Variables
bool balldrop = false;	
float ballx = 60;
float bally = 14;
float prev_ballx = 100;
float prev_bally = 14;
float accely = 0.005;
float vely = 0.5;
float velx = 0;
bool toggle = true; // If the ball hasn't moved, toggle is true
int accelCount = 0;
int lives = 5;
bool endpoint;
int portalDelayCount = 0;

int redx0, redy0, redx1, redy1;

int level_erases_array[5] = {0, 0, 0, 0, 0};


Point hitPoint;

//Line array to store line points
Line lineArray[line_max];
Line temp_line_array[line_max];
bool placing_line = false;
bool exceeded_length = false;
int line_count = 0; 
int max_line_length = 5000;

//Variables for IO
bool led_array[10] = {0}; 
char b1 = 0, b2 = 0, b3 = 0;

//level variables
int portalx[2];
int portaly[2];
int starx;
int stary;
int current_level = 1;
int previous_level = 1;

int board_resetting = 0;

Line level1[] = {{{0, 80}, {180, 80}},
				  {{0, 81}, {180, 81}},
				  {{0, 82}, {180, 82}},
				  {{0, 83}, {180, 83}},
				  {{0, 84}, {180, 84}},
				  {{0, 85}, {180, 85}},
				  {{0, 86}, {180, 86}},
				  {{0, 87}, {180, 87}},
				  
				  {{319, 180}, {90, 180}},
				  {{319, 181}, {90, 181}},
				  {{319, 182}, {90, 182}},
				  {{319, 183}, {90, 183}},
				  {{319, 184}, {90, 184}},
				  {{319, 185}, {90, 185}},
				  {{319, 186}, {90, 186}},
				  {{319, 187}, {90, 187}},
				  
				  {{319, 180}, {90, 180}},
				  {{319, 181}, {90, 181}},
				  {{319, 182}, {90, 182}},
				  {{319, 183}, {90, 183}},
				  {{319, 184}, {90, 184}},
				  {{319, 185}, {90, 185}},
				  {{319, 186}, {90, 186}},
				  {{319, 187}, {90, 187}},
    };

Line level2[] = {{{55, 120}, {180, 40}},
				  {{55, 121}, {180, 41}},
				  {{55, 122}, {180, 42}},
				  {{55, 123}, {180, 43}},
				  {{55, 124}, {180, 44}},
				  {{55, 125}, {180, 45}},
				  {{55, 126}, {180, 46}},
				  {{55, 127}, {180, 47}},
				 
				  {{0, 110}, {40, 110}},
				  {{0, 111}, {40, 111}},
				  {{0, 112}, {40, 112}},
				  {{0, 113}, {40, 113}},
				  {{0, 114}, {40, 114}},
				  {{0, 115}, {40, 115}},
				  {{0, 116}, {40, 116}},
				  {{0, 117}, {40, 117}},
				 
				  {{0, 170}, {80, 170}},
				  {{0, 171}, {80, 171}},
				  {{0, 172}, {80, 172}},
				  {{0, 173}, {80, 173}},
				  {{0, 174}, {80, 174}},
				  {{0, 175}, {80, 175}},
				  {{0, 176}, {80, 176}},
				  {{0, 177}, {80, 177}},
				 
				  {{160, 170}, {168, 170}},
				  {{160, 171}, {168, 171}},
				  {{160, 172}, {168, 172}},
				  {{160, 173}, {168, 173}},
				  {{160, 174}, {168, 174}},
				  {{160, 175}, {168, 175}},
				  {{160, 176}, {168, 176}},
				  {{160, 177}, {168, 177}},
				 
				  {{180, 170}, {188, 170}},
				  {{180, 171}, {188, 171}},
				  {{180, 172}, {188, 172}},
				  {{180, 173}, {188, 173}},
				  {{180, 174}, {188, 174}},
				  {{180, 175}, {188, 175}},
				  {{180, 176}, {188, 176}},
				  {{180, 177}, {188, 177}},
				 
				  {{170, 210}, {178, 210}},
				  {{170, 211}, {178, 211}},
				  {{170, 212}, {178, 212}},
				  {{170, 213}, {178, 213}},
				  {{170, 214}, {178, 214}},
				  {{170, 215}, {178, 215}},
				  {{170, 216}, {178, 216}},
				  {{170, 217}, {178, 217}}, 
    };

Line level3[] =  {{{40, 10}, {40, 170}},
				  {{41, 10}, {41, 170}},
				  {{42, 10}, {42, 170}},
				  {{43, 10}, {43, 170}},
				  {{44, 10}, {44, 170}},
				  {{45, 10}, {45, 170}},
				  {{46, 10}, {46, 170}},
				  {{47, 10}, {47, 170}},
				 
				  {{80, 10}, {80, 170}},
				  {{81, 10}, {81, 170}},
				  {{82, 10}, {82, 170}},
				  {{83, 10}, {83, 170}},
				  {{84, 10}, {84, 170}},
				  {{85, 10}, {85, 170}},
				  {{86, 10}, {86, 170}},
				  {{87, 10}, {87, 170}},
				  
				  {{80, 210}, {40, 170}},
				  {{81, 210}, {41, 170}},
				  {{82, 210}, {42, 170}},
				  {{83, 210}, {43, 170}},
				  {{84, 210}, {44, 170}},
				  {{85, 210}, {45, 170}},
				  {{86, 210}, {46, 170}},
				  {{87, 210}, {47, 170}},
				  
				  {{80, 210}, {120, 170}},
				  {{81, 210}, {121, 170}},
				  {{82, 210}, {122, 170}},
				  {{83, 210}, {123, 170}},
				  {{84, 210}, {124, 170}},
				  {{85, 210}, {125, 170}},
				  {{86, 210}, {126, 170}},
				  {{87, 210}, {127, 170}},
				  
				  {{200, 100}, {140, 170}},
				  {{201, 100}, {141, 170}},
				  {{202, 100}, {142, 170}},
				  {{203, 100}, {143, 170}},
				  {{204, 100}, {144, 170}},
				  {{205, 100}, {145, 170}},
				  {{206, 100}, {146, 170}},
				  {{207, 100}, {147, 170}},
				  
				  {{250, 100}, {310, 170}},
				  {{251, 100}, {311, 170}},
				  {{252, 100}, {312, 170}},
				  {{253, 100}, {313, 170}},
				  {{254, 100}, {314, 170}},
				  {{255, 100}, {315, 170}},
				  {{256, 100}, {316, 170}},
				  {{257, 100}, {317, 170}},
				  
				  {{140, 170}, {317, 170}},
				  {{140, 171}, {317, 171}},
				  {{140, 172}, {317, 172}},
				  {{140, 173}, {317, 173}},
				  {{140, 174}, {317, 174}},
				  {{140, 175}, {317, 175}},
				  {{140, 176}, {317, 176}},
				  {{140, 177}, {317, 177}},

    };

Line level4[] =  {{{0, 10}, {50, 10}},
				  {{0, 11}, {50, 11}},
				  {{0, 12}, {50, 12}},
				  {{0, 13}, {50, 13}},
				  {{0, 14}, {50, 14}},
				  {{0, 15}, {50, 15}},
				  {{0, 16}, {50, 16}},
				  {{0, 17}, {50, 17}},
				  {{70, 10}, {319, 10}},
				  {{70, 11}, {319, 11}},
				  {{70, 12}, {319, 12}},
				  {{70, 13}, {319, 13}},
				  {{70, 14}, {319, 14}},
				  {{70, 15}, {319, 15}},
				  {{70, 16}, {319, 16}},
				  {{70, 17}, {319, 17}},
					
				  {{0, 50}, {50, 50}},
				  {{0, 51}, {50, 51}},
				  {{0, 52}, {50, 52}},
				  {{0, 53}, {50, 53}},
				  {{0, 54}, {50, 54}},
				  {{0, 55}, {50, 55}},
				  {{0, 56}, {50, 56}},
				  {{0, 57}, {50, 57}},
				  {{70, 50}, {250, 50}},
				  {{70, 51}, {250, 51}},
				  {{70, 52}, {250, 52}},
				  {{70, 53}, {250, 53}},
				  {{70, 54}, {250, 54}},
				  {{70, 55}, {250, 55}},
				  {{70, 56}, {250, 56}},
				  {{70, 57}, {250, 57}},
				  
				  {{0, 90}, {50, 90}},
				  {{0, 91}, {50, 91}},
				  {{0, 92}, {50, 92}},
				  {{0, 93}, {50, 93}},
				  {{0, 94}, {50, 94}},
				  {{0, 95}, {50, 95}},
				  {{0, 96}, {50, 96}},
				  {{0, 97}, {50, 97}},
				  {{70, 90}, {187, 90}},
				  {{70, 91}, {187, 91}},
				  {{70, 92}, {187, 92}},
				  {{70, 93}, {187, 93}},
				  {{70, 94}, {187, 94}},
				  {{70, 95}, {187, 95}},
				  {{70, 96}, {187, 96}},
				  {{70, 97}, {187, 97}},
				  
				  {{0, 130}, {50, 130}},
				  {{0, 131}, {50, 131}},
				  {{0, 132}, {50, 132}},
				  {{0, 133}, {50, 133}},
				  {{0, 134}, {50, 134}},
				  {{0, 135}, {50, 135}},
				  {{0, 136}, {50, 136}},
				  {{0, 137}, {50, 137}},
				  {{100, 130}, {180, 130}},
				  {{100, 131}, {180, 131}},
				  {{100, 132}, {180, 132}},
				  {{100, 133}, {180, 133}},
				  {{100, 134}, {180, 134}},
				  {{100, 135}, {180, 135}},
				  {{100, 136}, {180, 136}},
				  {{100, 137}, {180, 137}},
				  
				  {{180, 130}, {180, 239}},
				  {{181, 130}, {181, 239}},
				  {{182, 130}, {182, 239}},
				  {{183, 130}, {183, 239}},
				  {{184, 130}, {184, 239}},
				  {{185, 130}, {185, 239}},
				  {{186, 130}, {186, 239}},
				  {{187, 130}, {187, 239}},
				  
				  {{260, 100}, {260, 239}},
				  {{261, 100}, {261, 239}},
				  {{262, 100}, {262, 239}},
				  {{263, 100}, {263, 239}},
				  {{264, 100}, {264, 239}},
				  {{265, 100}, {265, 239}},
				  {{266, 100}, {266, 239}},
				  {{267, 100}, {267, 239}},
				 
				  {{260, 130}, {318, 130}},
				  {{260, 131}, {318, 131}},
				  {{260, 132}, {318, 132}},
				  {{260, 133}, {318, 133}},
				  {{260, 134}, {318, 134}},
				  {{260, 135}, {318, 135}},
				  {{260, 136}, {318, 136}},
				  {{260, 137}, {318, 137}},
				  
				  
    };

Line level5[] =  {{{0, 30}, {40, 30}},
{{0, 31}, {40, 31}},
{{0, 32}, {40, 32}},
{{0, 33}, {40, 33}},
{{0, 34}, {40, 34}},
{{0, 35}, {40, 35}},
{{0, 36}, {40, 36}},
{{0, 37}, {40, 37}},
{{0, 38}, {40, 38}},
{{0, 39}, {40, 39}},
{{0, 40}, {40, 40}},

{{0, 95}, {30, 95}},
{{0, 96}, {30, 96}},
{{0, 97}, {30, 97}},
{{0, 98}, {30, 98}},
{{0, 99}, {30, 99}},
{{0, 100}, {30, 100}},
{{0, 101}, {30, 101}},
{{0, 102}, {30, 102}},
{{0, 103}, {30, 103}},
{{0, 104}, {30, 104}},

{{0, 170}, {30, 170}},
{{0, 171}, {30, 171}},
{{0, 172}, {30, 172}},
{{0, 173}, {30, 173}},
{{0, 174}, {30, 174}},
{{0, 175}, {30, 175}},
{{0, 176}, {30, 176}},
{{0, 177}, {30, 177}},
{{0, 178}, {30, 178}},
{{0, 179}, {30, 179}},
{{0, 180}, {30, 180}},

{{75, 11}, {75, 80}},
{{76, 11}, {76, 80}},
{{77, 11}, {77, 80}},
{{78, 11}, {78, 80}},
{{79, 11}, {79, 80}},
{{80, 11}, {80, 80}},
{{81, 11}, {81, 80}},
{{82, 11}, {82, 80}},
{{83, 11}, {83, 80}},
{{84, 11}, {84, 80}},

{{75, 126}, {75, 195}},
{{76, 126}, {76, 195}},
{{77, 126}, {77, 195}},
{{78, 126}, {78, 195}},
{{79, 126}, {79, 195}},
{{80, 126}, {80, 195}},
{{81, 126}, {81, 195}},
{{82, 126}, {82, 195}},
{{83, 126}, {83, 195}},
{{84, 126}, {84, 195}},

{{55, 70}, {75, 70}},
{{55, 71}, {75, 71}},
{{55, 72}, {75, 72}},
{{55, 73}, {75, 73}},
{{55, 74}, {75, 74}},
{{55, 75}, {75, 75}},
{{55, 76}, {75, 76}},
{{55, 77}, {75, 77}},
{{55, 78}, {75, 78}},
{{55, 79}, {75, 79}},

{{55, 126}, {75, 126}},
{{55, 127}, {75, 127}},
{{55, 128}, {75, 128}},
{{55, 129}, {75, 129}},
{{55, 130}, {75, 130}},
{{55, 131}, {75, 131}},
{{55, 132}, {75, 132}},
{{55, 133}, {75, 133}},
{{55, 134}, {75, 134}},
{{55, 135}, {75, 135}},
{{55, 136}, {75, 136}},

{{120, 11}, {120, 110}},
{{121, 11}, {121, 110}},
{{122, 11}, {122, 110}},
{{123, 11}, {123, 110}},
{{124, 11}, {124, 110}},
{{125, 11}, {125, 110}},
{{126, 11}, {126, 110}},
{{127, 11}, {127, 110}},
{{128, 11}, {128, 110}},
{{129, 11}, {129, 110}},
{{130, 11}, {130, 110}},
{{131, 11}, {131, 110}},
{{132, 11}, {132, 110}},
{{133, 11}, {133, 110}},
{{134, 11}, {134, 110}},
{{135, 11}, {135, 110}},

{{120, 180}, {120, 240}},
{{121, 180}, {121, 240}},
{{122, 180}, {122, 240}},
{{123, 180}, {123, 240}},
{{124, 180}, {124, 240}},
{{125, 180}, {125, 240}},
{{126, 180}, {126, 240}},
{{127, 180}, {127, 240}},
{{128, 180}, {128, 240}},
{{129, 180}, {129, 240}},
{{130, 180}, {130, 240}},
{{131, 180}, {131, 240}},
{{132, 180}, {132, 240}},
{{133, 180}, {133, 240}},
{{134, 180}, {134, 240}},
{{135, 180}, {135, 240}},

{{180, 100}, {180, 190}},
{{181, 100}, {181, 190}},
{{182, 100}, {182, 190}},
{{183, 100}, {183, 190}},
{{184, 100}, {184, 190}},
{{185, 100}, {185, 190}},
{{186, 100}, {186, 190}},
{{187, 100}, {187, 190}},
{{188, 100}, {188, 190}},
{{189, 100}, {189, 190}},
{{190, 100}, {190, 190}},
{{191, 100}, {191, 190}},
{{192, 100}, {192, 190}},
{{193, 100}, {193, 190}},
{{194, 100}, {194, 190}},
{{195, 100}, {195, 190}},

{{240, 180}, {240, 240}},
{{241, 180}, {241, 240}},
{{242, 180}, {242, 240}},
{{243, 180}, {243, 240}},
{{244, 180}, {244, 240}},
{{245, 180}, {245, 240}},
{{246, 180}, {246, 240}},
{{247, 180}, {247, 240}},
{{248, 180}, {248, 240}},
{{249, 180}, {249, 240}},
{{250, 180}, {250, 240}},
{{251, 180}, {251, 240}},
{{252, 180}, {252, 240}},
{{253, 180}, {253, 240}},
{{254, 180}, {254, 240}},
{{255, 180}, {255, 240}},

{{240, 11}, {240, 110}},
{{241, 11}, {241, 110}},
{{242, 11}, {242, 110}},
{{243, 11}, {243, 110}},
{{244, 11}, {244, 110}},
{{245, 11}, {245, 110}},
{{246, 11}, {246, 110}},
{{247, 11}, {247, 110}},
{{248, 11}, {248, 110}},
{{249, 11}, {249, 110}},
{{250, 11}, {250, 110}},
{{251, 11}, {251, 110}},
{{252, 11}, {252, 110}},
{{253, 11}, {253, 110}},
{{254, 11}, {254, 110}},
{{255, 11}, {255, 110}},

{{295, 95}, {320, 95}},
{{295, 96}, {320, 96}},
{{295, 97}, {320, 97}},
{{295, 98}, {320, 98}},
{{295, 99}, {320, 99}},
{{295, 100}, {320, 100}},
{{295, 101}, {320, 101}},
{{295, 102}, {320, 102}},
{{295, 103}, {320, 103}},
{{295, 104}, {320, 104}},
{{295, 105}, {320, 105}},
{{295, 106}, {320, 106}},
{{295, 107}, {320, 107}},
{{295, 108}, {320, 108}},
{{295, 109}, {320, 109}},
{{295, 110}, {320, 110}},
    };

int obstacle_count[5] = {16, 48, 56, 88, 8};

/* function prototypes */
void clear_screen();
void draw_line(float fx0, float fy0, float fx1, float fy1, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void HEX_PS2(char, char, char);
void wait_for_vsync();
char keyboard_movement(char b1, char b2);
void update_cursor();
void keyboard();
void draw_title();
void plot_crosshair(int x, int y, short int line_color);
void plot_ball(float fx, float fy, short int line_color);
void update_ball(void);
bool collision(float bx, float by, Point p1, Point p2);
void deflectBall(float x1, float x2, float y1, float y2);
void plot_hearts();
void draw_level(int level);
void draw_star(int x, int y, int line_color);
int iround(float x);
void writeCharacter(char character, int x, int y);
void writeWord(char word[], int x, int y);
void drawMenu();
void clearChar();
void erase_level(int level);
void nextLevel();
void reflectOffPoint(float px, float py);
void attemptsHex();
void playLossNoise();

int main(void) {
    volatile int *LEDR = LEDR_BASE;
	volatile long int *clock = (long int *)CLOCK_BASE;
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
	volatile int* AUDIO_ptr = 0xFF203040;
   /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	for(int i = 0; i<line_max; i++){
		lineArray[i].p1.x = -1;
		lineArray[i].p1.y = -1;
		lineArray[i].p2.x = -1;
		lineArray[i].p2.y = -1;
	}
	//Main Menu Screen
	while(1){
		drawMenu();
		clear_screen();
		char start[] = "Press [Space] to Start";
		writeWord(start, 29, 55);
		keyboard();
		if(b3 == 0x29 && b2 == 0xF0){
			clearChar();
			b3 = 0;
			b2 = 0;
			break;
		}
		attemptsHex();
	}
    draw_level(current_level);
	
    while (1) {
		attemptsHex();
        draw_level(current_level);
		keyboard();	
		if(exceeded_length){
			draw_line(redx0, redy0, redx1, redy1, 0x0);
			exceeded_length = false;
		}
		if(placing_line){
			draw_line((int)lineArray[line_count].p1.x, (int)lineArray[line_count].p1.y, prev_cursorx, prev_cursory, 0x0);	
		}
		plot_crosshair(prev_cursorx, prev_cursory, 0);
		plot_ball(prev_ballx, prev_bally, 0x0);
		prev_cursorx = cursorx;
		prev_cursory = cursory;
		prev_ballx = ballx;
		prev_bally = bally;
		update_cursor();
		update_ball();
		if(placing_line){
			draw_line((int)lineArray[line_count].p1.x, (int)lineArray[line_count].p1.y, cursorx, cursory, 0xf800);
			redx0 = (int)lineArray[line_count].p1.x;
			redy0 = (int)lineArray[line_count].p1.y;
			redx1 = cursorx;
			redy1 = cursory;
		}
		if(lives == 0 || board_resetting > 0 || (b3 == 0x2D && b2 == 0xF0)) {
			reset_board();
			for(int i = 0; i<line_max; i++){
                if(temp_line_array[i].p1.x != -1 &&  temp_line_array[i].p1.y != -1 &&  temp_line_array[i].p2.x != -1 && temp_line_array[i].p2.y != -1){
                    draw_line(temp_line_array[i].p1.x, temp_line_array[i].p1.y, temp_line_array[i].p2.x, temp_line_array[i].p2.y, 0x0);	
                }
            }
			board_resetting++;
			if(board_resetting == 2){
				board_resetting = 0;
			}
			b3 = 0;
		}
		plot_crosshair(cursorx, cursory, 0xFFFF);
		plot_ball(ballx, bally, 0xfd80);
		plot_line_bar();
        plot_hearts();
        for(int i = 0; i<line_max; i++){
			if(lineArray[i].p1.x != -1 &&  lineArray[i].p1.y != -1 &&  lineArray[i].p2.x != -1 && lineArray[i].p2.y != -1){
				draw_line(lineArray[i].p1.x, lineArray[i].p1.y, lineArray[i].p2.x, lineArray[i].p2.y, 0xFFFF);	
			}
		}

		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
	//Draw game over screen or draw game win screen

}

void HEX_PS2(char b1, char b2, char b3) {
	volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
	volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
	/* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
	* a single 7-seg display in the DE1-SoC Computer, for the hex digits 0 - F
	*/
	unsigned char seven_seg_decode_table[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
	0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int shift_buffer, nibble;
	unsigned char code;
	int i;
	shift_buffer = (b1 << 16) | (b2 << 8) | b3;
	for (i = 0; i < 6; ++i) {
		nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
		code = seven_seg_decode_table[nibble];
		hex_segs[i] = code;
		shift_buffer = shift_buffer >> 4;
	}
	/* drive the hex displays */
	*(HEX3_HEX0_ptr) = *(int *)(hex_segs);
	*(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}

void attemptsHex() {
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
    // Seven-segment encoding for hex digits 0-F
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
    };
    int ones = line_count % 10;  
    int tens = line_count / 10;  
	unsigned int hex_display = (seven_seg_decode_table[tens] << 8) | seven_seg_decode_table[ones];
    *(HEX3_HEX0_ptr) = hex_display; 
    *(HEX5_HEX4_ptr) = 0;             
}

void clearChar(){
	for(int i = 0; i < 80; i++){
		for(int j = 0; j < 60; j++){
			writeCharacter(' ', i, j);
		}
	}
}

void clear_screen(){
	int y, x;
	for(x = 0;x<320; x++){
		for(y = 0; y<240; y++){
			plot_pixel(x, y, 0);
		}
	}
	draw_title();
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    one_pixel_address =  pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}
void plot_crosshair(int x, int y, short int line_color)
{
	plot_pixel(x, y, line_color);
	plot_pixel(x+1, y, line_color);
	plot_pixel(x+2, y, line_color);
	plot_pixel(x-1, y, line_color);
	plot_pixel(x-2, y, line_color);
	plot_pixel(x, y+1, line_color);
	plot_pixel(x, y+2, line_color);
	plot_pixel(x, y-1, line_color);
	plot_pixel(x, y-2, line_color);
}
void plot_ball(float fx, float fy, short int line_color)
{
	int x = iround(fx);
	int y = iround(fy);
	if(x+1 >= x_max){
		x = x_max-1;	
	}
	if(x-1 <= x_min){
		x = x_min+1;	
	}
	if(y+1 >= y_max){
		y = y_max-1;	
	}
	if(y-1 <= y_min){
		y = y_min+1;	
	}
	plot_pixel(x, y, line_color);
	plot_pixel(x+1, y, line_color);
	plot_pixel(x+1, y-1, line_color);
	plot_pixel(x, y-1, line_color);
	plot_pixel(x-1, y-1, line_color);
	plot_pixel(x-1, y, line_color);
	plot_pixel(x-1, y+1, line_color);
	plot_pixel(x, y+1, line_color);
	plot_pixel(x+1, y+1, line_color);
}
void wait_for_vsync()
{
	volatile int * pixel_ctrl_ptr = (int *) 0xff203020; // base address
	int status;
	*pixel_ctrl_ptr = 1; // start the synchronization process
	// write 1 into front buffer address register
	status = *(pixel_ctrl_ptr + 3); // read the status register
	while ((status & 0x01) != 0) // polling loop waiting for S bit to go to 0
	{
		status = *(pixel_ctrl_ptr + 3);
	} // polling loop/function exits when status bit goes to 0
}

void update_ball(void){
	volatile int* audio_ptr = 0xFF203040;
	//Keyboard Input
	if(b3 == 0x29 && toggle){
		if(b2 == 0xF0){
			balldrop = true;
			toggle = false;
			b3 = 0;
			b2 = 0;
		}
	}
	//Check for line collision
	if(balldrop == true){
		int num_substeps = 5;
		float step_velx = velx / num_substeps;
		float step_vely = vely / num_substeps;

		for(int i = 0; i < num_substeps; i++){
			
			ballx += step_velx;
            bally += step_vely;
			
			if (bally <= 10) {
                bally = 10;
                vely *= -1;
                break;
            }
			bool coll = false;
			for(int j = 0; j < line_count; j++){
				endpoint = false;
				if (collision(ballx, bally, lineArray[j].p1, lineArray[j].p2)) {
					if(endpoint){
						reflectOffPoint(hitPoint.x, hitPoint.y);
					}
					else{
						deflectBall(lineArray[j].p1.x, lineArray[j].p1.y, lineArray[j].p2.x, lineArray[j].p2.y);
					}
					ballx += velx * 0.5;
					bally += vely * 0.5;
					coll = true;
					break;
				}
			}
			if(coll){
				break;
			}
		}
		//Accelerate Velocity
		vely += accely;
		//Check for wall collision
		if(ballx <= 0 ||ballx >= 319 ||bally >= 239){
			liveLost();
		}

		//Check for obstacle collision
		if(current_level == 1){
			int size =  sizeof(level1) / sizeof(level1[0]);
			for (int i = 0; i < size; i++){
				if(collision(ballx, bally, level1[i].p1, level1[i].p2)){
					liveLost();
					break;
				}
			}
		}
		if(current_level == 2){
			int size =  sizeof(level2) / sizeof(level2[0]);
			for (int i = 0; i < size; i++){
				if(collision(ballx, bally, level2[i].p1, level2[i].p2)){
					liveLost();
					break;
				}
			}
		}
		if(current_level == 3){
			int size =  sizeof(level3) / sizeof(level3[0]);
			for (int i = 0; i < size; i++){
				if(collision(ballx, bally, level3[i].p1, level3[i].p2)){
					liveLost();
					break;
				}
			}
		}
		if(current_level == 4){
			int size =  sizeof(level4) / sizeof(level4[0]);
			for (int i = 0; i < size; i++){
				if(collision(ballx, bally, level4[i].p1, level4[i].p2)){
					liveLost();
					break;
				}
			}
		}
		if(current_level == 5){
			int size =  sizeof(level5) / sizeof(level5[0]);
			for (int i = 0; i < size; i++){
				if(collision(ballx, bally, level5[i].p1, level5[i].p2)){
					liveLost();
					break;
				}
			}
		}
		//Collision with portal
		for(int i = 0; i < 2; i++){
			if(ballx >= portalx[i] - 4 && ballx <= portalx[i] + 4 && bally >= portaly[i] - 4 && bally <= portaly[i] + 4){
				if(portalDelayCount == 0){
					portalTravel(i);
					portalDelayCount++;
					break;
				}
				else{
					portalDelayCount++;
					if(portalDelayCount == 10){
						portalDelayCount = 0;
					}
				}
			}
		}
		//Check for victory condition
		if(ballx >= starx - 10 && ballx <= starx + 10 && bally >= stary - 10 && bally <= stary + 10){
			nextLevel();
			return;
		}

	}	
}
void nextLevel(){
	reset_board();
	current_level++;
}

void liveLost(){
	balldrop = false;
	ballx = 60;
	bally = 14;
	lives--;
	toggle = true;
	velx = 0;
	vely = 0.5;
}

void reset_board(){
	balldrop = false;
	ballx = 60;
	bally = 14;
	toggle = true;
	velx = 0;
	vely = 0.5;	
	lives = 5;
	if(board_resetting == 0){
	memcpy(temp_line_array, lineArray, sizeof(lineArray)); 
		for(int i = 0; i<line_max; i++){
			lineArray[i].p1.x = -1;
			lineArray[i].p1.y = -1;
			lineArray[i].p2.x = -1;
			lineArray[i].p2.y = -1;
		}
	}
	line_count = 0;
	max_line_length = 5000;
}

bool collision(float bx, float by, Point p1, Point p2) {
    float dx = p2.x - p1.x; 
    float dy = p2.y - p1.y;

    float t = ((bx - p1.x) * dx + (by - p1.y) * dy) / (dx * dx + dy * dy);

    if (t < 0) {
		t = 0;
	} else if (t > 1) {
		t = 1;
	}

    // closest point
    float closestX = p1.x + t * dx;
    float closestY = p1.y + t * dy;

    // distance
    float distX = bx - closestX;
    float distY = by - closestY;
    float distanceSquared = distX * distX + distY * distY;

    if (distanceSquared <= 4) {
        endpoint = (t == 0 || t == 1); 
        if (endpoint) {
            hitPoint.x = (t == 0) ? p1.x : p2.x;
            hitPoint.y = (t == 0) ? p1.y : p2.y;
        }
        return true;
    }
    return false;
}

float isqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                     
	i  = 0x5f3759df - ( i >> 1 );               
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   
	return y;
}

void reflectOffPoint(float px, float py) {
    float dx = ballx - px;
    float dy = bally - py;

    float dist_sq = dx * dx + dy * dy;
    if (dist_sq < 1e-6) return; 

    float invDist = isqrt(dist_sq);
    dx *= invDist;
    dy *= invDist;

    // reflect based off point deflection
    float dot_product = velx * dx + vely * dy;
    velx -= 2 * dot_product * dx;
    vely -= 2 * dot_product * dy;

    ballx = px + dx * 2;
    bally = py + dy * 2;
}

void deflectBall(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float line_length = isqrt(dx * dx + dy * dy);

    // calculate normal vector
    dx *= line_length;
    dy *= line_length;
    float nx = -dy;
    float ny = dx;
    float dot_product = velx * nx + vely * ny;

    // if dot product is almost 0, ball is parallel to line
    if (dot_product < 1e-6 && dot_product > -1e-6) {
        return;
    }

    // reflect velocity
    velx = velx - 2 * dot_product * nx;
    vely = vely - 2 * dot_product * ny;
}

void portalTravel(int num){
	if(num == 0){
		ballx = portalx[1];
		bally = portaly[1];
	}
	else{
		ballx = portalx[0];
		bally = portaly[0];
	}
}

void keyboard(void){
	volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
	PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
	RVALID = PS2_data & 0x8000; // extract the RVALID field    
	if(RVALID != 0) {
		b1 = b2;
		b2 = b3;
		b3 = PS2_data & 0xFF;	
	}	
}

void update_cursor(void){
	//right
	if(b3 == 0x23){
		if(b2 == 0xF0){
			
			direction_array[0] = 0;
		}
		else {
			direction_array[0] = 1;
		}
	}
	//
	if(b3 == 0x1B){
		if(b2 == 0xF0){
			direction_array[1] = 0;
		}
		else {
			direction_array[1] = 1;
		}
	}
	//left
	if(b3 == 0x1C){
		if(b2 == 0xF0){
			direction_array[3] = 0;
		}
		else {
			direction_array[3] = -1;
		}
	}
	//up
	if(b3 == 0x1D){
		if(b2 == 0xF0){
			direction_array[2] = 0;
		}
		else {
			direction_array[2] = -1;
		}
	}
	if(b3 == 0x3A){
		if(b2 == 0xF0){
			nextLevel();
			b3 = 0;
		}
	}
	

	
	cursorx = cursorx+direction_array[0]+direction_array[3];
	cursory = cursory+direction_array[1]+direction_array[2];
	
	if(cursorx+2 >= x_max){
		cursorx = x_max-2;	
	}
	if(cursorx-2 <= x_min){
		cursorx = x_min+2;	
	}
	if(cursory+2 >= y_max){
		cursory = y_max-2;	
	}
	if(cursory-2 <= y_min){
		cursory = y_min+2;	
	}
	
	//line placing
	if(line_count < line_max){
		if(b3 == 0x5A){
			if(b2 == 0xF0){
				if(!placing_line){
					if((int)lineArray[line_count-1].p2.x != cursorx || (int)lineArray[line_count-1].p2.y != cursory){
						placing_line = true;	
						lineArray[line_count].p1.x = cursorx;
						lineArray[line_count].p1.y = cursory;	
						exceeded_length =  false;
					} 
				}
				else if(placing_line){
					if((int)lineArray[line_count].p1.x != cursorx || (int)lineArray[line_count].p1.y != cursory){
						if(calculate_length((int)lineArray[line_count].p1.x, (int)lineArray[line_count].p1.y, cursorx, cursory) < max_line_length){
							lineArray[line_count].p2.x = cursorx;
							lineArray[line_count].p2.y = cursory;	
							placing_line = false;	
							line_count++;
							max_line_length -= calculate_length((int)lineArray[line_count].p1.x, 
																(int)lineArray[line_count].p2.y, 
																cursorx, cursory);
							printf("placed line with length %d\n", calculate_length((int)lineArray[line_count].p1.x,
																					(int)lineArray[line_count].p2.y, 
																					cursorx, cursory));
                                                                                    exceeded_length = true;
							if(max_line_length <= 0){
								max_line_length = 0;	
							}
						} else {
							;
							lineArray[line_count].p1.x = -1;	
							lineArray[line_count].p1.y = -1;	
							placing_line = false;
							exceeded_length = true;
						}
					}
				}
				b2 = 0; 
				b3 = 0;
				
			}
		}
	}

}
void draw_title() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 320; x++) {
            plot_pixel(x, y, 0xad75);
        }
    }

    char *bounce_master[] = {
        " *****                                       *    *                                   ",
        " *    *                                      **  **             	                   ",
        " *    *                              ****    * ** *         *****   *    ****  * ***  ",
        " *****  ****** *    *  ****   ***** *    *   *    *  ****   *     ***** *    * **   * ",
        " *    * *    * *    * *    * *      ******   *	   * *    *	 *****   *   ****** *      ",
        " *    * *    * *    * *    * *      *        *	   * *    *      *   *   *      *      ",
        " *****  ******  ****  *    *  *****  *****   *	   *  **** * ***** 	 *    ***** *      "
    };

    int text_x = (320 - strlen(bounce_master[0])) / 2;
    int text_y = 1;

    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < strlen(bounce_master[row]); col++) {
            if (bounce_master[row][col] == '*') {
                plot_pixel(text_x + col, text_y + row, 0xFFFF);
            }
        }
    }
}

void draw_line(float fx0, float fy0, float fx1, float fy1, short int line_color){
	int x0 = iround(fx0);
	int y0 = iround(fy0);
	int x1 = iround(fx1);
	int y1 = iround(fy1);

	bool is_steep = abs(y1-y0) > abs(x1 - x0);
	if(is_steep){
		//swap(x0,y0)
		int temp = x0;
		x0 = y0;
		y0 = temp;
		
		//swap(x1,y1)
		temp = x1;
		x1 = y1;
		y1 = temp;
	}
	if(x0 > x1){
		//swap(x0,x1)
		int temp = x0;
		x0 = x1;
		x1 = temp;
		
		//swap(y0,y1)
		temp = y0;
		y0 = y1;
		y1 = temp;
	}

	int deltax = x1 - x0;
	int deltay = abs(y1-y0);
	int error = -(deltax/2);
	int y = y0;
	int y_step;
	if(y0 < y1){
		y_step = 1;
	}
	else{
		y_step = -1;
	}
	
	for(int x = x0; x < x1; x++){
		if(is_steep){
			plot_pixel(y, x, line_color);
		}
		else{
			plot_pixel(x, y, line_color);
		}
		error = error + deltay;
		if(error>0){
			y += y_step;
			error = error - deltax;
		}
	}
}

void plot_line_bar(){
	draw_line(3, 2, 104, 2, 0x0);
	draw_line(3, 6, 104, 6, 0x0);
	draw_line(3, 2, 3, 6, 0x0);
	draw_line(104, 2, 104, 7, 0x0);
	draw_line(104, 3, (int)(max_line_length/50)+4, 3, 0xad75);	
	draw_line(104, 4, (int)(max_line_length/50)+4, 4, 0xad75);	
	draw_line(104, 5, (int)(max_line_length/50)+4, 5, 0xad75);
	draw_line(4, 3, (int)(max_line_length/50)+4, 3, 0x07e0);	
	draw_line(4, 4, (int)(max_line_length/50)+4, 4, 0x07e0);	
	draw_line(4, 5, (int)(max_line_length/50)+4, 5, 0x07e0);
	
	
}
int calculate_length(int x0, int y0, int x1, int y1){
	return sqrt(abs((x0-x1)*(x0-x1)) + abs((y0-y1)*(y0-y1)));
}

void draw_star(int x, int y, int line_color){
	plot_pixel(x, y, line_color);
	plot_pixel(x+3, y+3, line_color);
	plot_pixel(x-3, y+3, line_color);
	plot_pixel(x-3, y-3, line_color);
	plot_pixel(x+3, y-3, line_color);
	draw_line(x-2, y-2, x+2, y-2, line_color);
	draw_line(x-2, y+2, x+2, y+2, line_color);
	draw_line(x-2, y-2, x-2, y+2, line_color);
	draw_line(x+2, y-2, x+2, y+3, line_color);
}


void draw_portal(int x, int y, int line_color){
    plot_pixel(x, y - 6, line_color);
    plot_pixel(x, y + 6, line_color);
    plot_pixel(x - 6, y, line_color);
    plot_pixel(x + 6, y, line_color);
    plot_pixel(x - 5, y - 5, line_color);
    plot_pixel(x + 5, y - 5, line_color);
    plot_pixel(x - 5, y + 5, line_color);
    plot_pixel(x + 5, y + 5, line_color);

    // Middle ring
    draw_line(x - 4, y - 4, x + 4, y - 4, line_color);
    draw_line(x - 4, y + 4, x + 4, y + 4, line_color);
    draw_line(x - 4, y - 4, x - 4, y + 4, line_color);
    draw_line(x + 4, y - 4, x + 4, y + 4, line_color);
    
    // Inner swirl (core magic effect)
    plot_pixel(x - 2, y, line_color);
    plot_pixel(x + 2, y, line_color);
    plot_pixel(x, y - 2, line_color);
    plot_pixel(x, y + 2, line_color);
    plot_pixel(x - 1, y - 1, line_color);
    plot_pixel(x + 1, y - 1, line_color);
    plot_pixel(x - 1, y + 1, line_color);
    plot_pixel(x + 1, y + 1, line_color);
}

void plot_hearts(){
    int x;
    for(x = 260; x < 290; x+=6){
        plot_pixel(x, 2, 0xad75);
        plot_pixel(x+1, 2, 0xad75);
        plot_pixel(x+2, 2, 0xad75);
        plot_pixel(x+3, 2, 0xad75);
        plot_pixel(x+4, 2, 0xad75);

        plot_pixel(x, 3, 0xad75);
        plot_pixel(x+1, 3, 0xad75);
        plot_pixel(x+2, 3, 0xad75);
        plot_pixel(x+3, 3, 0xad75);
        plot_pixel(x+4, 3, 0xad75);

        plot_pixel(x, 4, 0xad75);
        plot_pixel(x+1, 4, 0xad75);
        plot_pixel(x+2, 4, 0xad75);
        plot_pixel(x+3, 4, 0xad75);
        plot_pixel(x+4, 4, 0xad75);

        plot_pixel(x, 5, 0xad75);
        plot_pixel(x+1, 5, 0xad75);
        plot_pixel(x+2, 5, 0xad75);
        plot_pixel(x+3, 5, 0xad75);
        plot_pixel(x+4, 5, 0xad75);

        plot_pixel(x, 6, 0xad75);
        plot_pixel(x+1, 6, 0xad75);
        plot_pixel(x+2, 6, 0xad75);
        plot_pixel(x+3, 6, 0xad75);
        plot_pixel(x+4, 6, 0xad75);
    }
    if(lives > 0){
        for(x = 260; x < 260 + (lives*6); x+=6){
            plot_pixel(x, 2, 0xad75);
            plot_pixel(x+1, 2, 0xf800);
            plot_pixel(x+2, 2, 0xad75);
            plot_pixel(x+3, 2, 0xf800);
            plot_pixel(x+4, 2, 0xad75);

            plot_pixel(x, 3, 0xf800);
            plot_pixel(x+1, 3, 0xf800);
            plot_pixel(x+2, 3, 0xf800);
            plot_pixel(x+3, 3, 0xf800);
            plot_pixel(x+4, 3, 0xf800);

            plot_pixel(x, 4, 0xf800);
            plot_pixel(x+1, 4, 0xf800);
            plot_pixel(x+2, 4, 0xf800);
            plot_pixel(x+3, 4, 0xf800);
            plot_pixel(x+4, 4, 0xf800);

            plot_pixel(x, 5, 0xad75);
            plot_pixel(x+1, 5, 0xf800);
            plot_pixel(x+2, 5, 0xf800);
            plot_pixel(x+3, 5, 0xf800);
            plot_pixel(x+4, 5, 0xad75);

            plot_pixel(x, 6, 0xad75);
            plot_pixel(x+1, 6, 0xad75);
            plot_pixel(x+2, 6, 0xf800);
            plot_pixel(x+3, 6, 0xad75);
            plot_pixel(x+4, 6, 0xad75);
        }
    }

}

void draw_level(int level){

	if(level == 1){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level1[i].p1.x, level1[i].p1.y, level1[i].p2.x, level1[i].p2.y, 0x059f);	
		}
		starx = 20;
		stary = 160;
	} else {
        if(level_erases_array[0]<2){
            erase_level(1);
            for(int i = 0; i<line_max; i++){
                if(temp_line_array[i].p1.x != -1 &&  temp_line_array[i].p1.y != -1 &&  temp_line_array[i].p2.x != -1 && temp_line_array[i].p2.y != -1){
                    draw_line(temp_line_array[i].p1.x, temp_line_array[i].p1.y, temp_line_array[i].p2.x, temp_line_array[i].p2.y, 0x0);	
                }
            }
            level_erases_array[0]++;

        }
    }
	
	if(level == 2){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level2[i].p1.x, level2[i].p1.y, level2[i].p2.x, level2[i].p2.y, 0x059f);	
		} 
		starx = 174;
		stary = 195;
	} else if(level == 3) {
        if(level_erases_array[1]<2){
            erase_level(2);
            for(int i = 0; i<line_max; i++){
                if(temp_line_array[i].p1.x != -1 &&  temp_line_array[i].p1.y != -1 &&  temp_line_array[i].p2.x != -1 && temp_line_array[i].p2.y != -1){
                    draw_line(temp_line_array[i].p1.x, temp_line_array[i].p1.y, temp_line_array[i].p2.x, temp_line_array[i].p2.y, 0x0);	
                }
            }
            level_erases_array[1]++;
        }
    }
	
	if(level == 3){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level3[i].p1.x, level3[i].p1.y, level3[i].p2.x, level3[i].p2.y, 0x059f);	
		}
		starx = 225;
		stary = 150;
	} else if(level == 4) {
        if(level_erases_array[2]<2){
            erase_level(3);
            for(int i = 0; i<line_max; i++){
                if(temp_line_array[i].p1.x != -1 &&  temp_line_array[i].p1.y != -1 &&  temp_line_array[i].p2.x != -1 && temp_line_array[i].p2.y != -1){
                    draw_line(temp_line_array[i].p1.x, temp_line_array[i].p1.y, temp_line_array[i].p2.x, temp_line_array[i].p2.y, 0x0);	
                }
            }
            level_erases_array[2]++;
        }
    }
	
	if(level == 4){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level4[i].p1.x, level4[i].p1.y, level4[i].p2.x, level4[i].p2.y, 0x059f);	
		}
		starx = 220;
		stary = 220;
        portalx[0] = 300;
        portaly[0] = 40;
        portalx[1] = 120;
        portaly[1] = 200;
        draw_portal(portalx[0], portaly[0], 0xc81f);
        draw_portal(portalx[1], portaly[1], 0xc81f);
	} else if(level == 5) {
        if(level_erases_array[3]<2){
            erase_level(4);
            for(int i = 0; i<line_max; i++){
                if(temp_line_array[i].p1.x != -1 &&  temp_line_array[i].p1.y != -1 &&  temp_line_array[i].p2.x != -1 && temp_line_array[i].p2.y != -1){
                    draw_line(temp_line_array[i].p1.x, temp_line_array[i].p1.y, temp_line_array[i].p2.x, temp_line_array[i].p2.y, 0x0);	
                }
            }
            level_erases_array[3]++;
        }
    }

    if(level == 5){
    	for(int i = 0; i< sizeof(level5) / sizeof(level5[0]); i++){
			draw_line(level5[i].p1.x, level5[i].p1.y, level5[i].p2.x, level5[i].p2.y, 0x059f);	
		}
		starx = 300;
		stary = 220;
        portalx[0] = 20;
        portaly[0] = 215;
        portalx[1] = 300;
        portaly[1] = 35;
        draw_portal(portalx[0], portaly[0], 0xc81f);
        draw_portal(portalx[1], portaly[1], 0xc81f);
	}
	draw_star(starx, stary, 0xff60);
}

void erase_level(int level){
    int erase_starx, erase_stary;;
	if(level == 1){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level1[i].p1.x, level1[i].p1.y, level1[i].p2.x, level1[i].p2.y, 0x0);	
		}
		erase_starx = 20;
		erase_stary = 160;
	} 
	if(level == 2){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level2[i].p1.x, level2[i].p1.y, level2[i].p2.x, level2[i].p2.y, 0x0);	
		}
		erase_starx = 174;
		erase_stary = 195;
	} 	
	
	if(level == 3){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level3[i].p1.x, level3[i].p1.y, level3[i].p2.x, level3[i].p2.y, 0x0);	
		}
		erase_starx = 225;
		erase_stary = 150;
	} 

    if(level == 4){
    	for(int i = 0; i<obstacle_count[level-1]; i++){
			draw_line(level4[i].p1.x, level4[i].p1.y, level4[i].p2.x, level4[i].p2.y, 0x0);	
		}
		erase_starx = 220;
		erase_stary = 220;
        draw_portal(300, 40, 0x0);
        draw_portal(120, 200, 0x0);
	} 
	draw_star(erase_starx, erase_stary, 0x0);
}

int iround(float x){
	if(x - (int)x > 0.5){
		return (int)x + 1;
	}
	else{
		return (int)x;
	}
}

void writeCharacter(char character, int x, int y) {
    // pointer to character buffer
    volatile char *char_buffer = (char *)CHAR_BASE;

    // format word
    int offset = (y << 7) + x;

    // write word into memory
    char_buffer[offset] = character;
    return;
}

void writeWord(char word[], int x, int y){
	int size = strlen(word);
	for(int i = 0; i < size; i++){
		writeCharacter(word[i], x + i, y);
		//writeCharacter(word[i], x, y + i);
	}
}



void drawMenu(){
	char word1[] = "PRESS [SPACE]";
	for(int i = 0; i < 10; i++){
		int randx = 10 + rand() % 49;
		int randy = 10 + rand() % 41;
		writeWord(word1, randx, randy);
	}
}
