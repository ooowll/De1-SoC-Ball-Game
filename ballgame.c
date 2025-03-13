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
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>	
#include <stdint.h>
	
int pixel_buffer_start;
int cursorx = 160;
int cursory = 120; 
bool led_array[10] = {0}; 
int direction_array[4] = {0};
char b1 = 0, b2 = 0, b3 = 0;

/* function prototypes */
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void HEX_PS2(char, char, char);
void wait_for_vsync();
char keyboard_movement(char b1, char b2);

int main(void) {
    volatile int *LEDR = LEDR_BASE;
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
	volatile long int *clock = (long int *)CLOCK_BASE;
    pixel_buffer_start = *pixel_ctrl_ptr; //Read location of the pixel buffer from the pixel buffer controller
    clear_screen();
    while (1) {
		update_cursor();
		*LEDR = led_array[9] << 9 | led_array[8] << 8 | led_array[7] << 7 | led_array[6] << 6 | led_array[5] << 5 | led_array[4] << 4 | led_array[3] << 3 | led_array[2] << 2 | led_array[1] << 1 | led_array[0] << 0;	
		plot_pixel(cursorx, cursory, 0xFFFF);
		if((*clock % 1000)==0){
		cursorx = cursorx+direction_array[0]+direction_array[3];
		cursory = cursory+direction_array[1]+direction_array[2];
		}
    }
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

void clear_screen(){
	int y, x;
	for(x = 0;x<320; x++){
		for(y = 0; y<240; y++){
			plot_pixel(x, y, 0);
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
	if(x >= 319){
	x = 319;	
	}
	if(x <= 0){
	x = 0;	
	}
	if(y >= 239){
	y = 239;	
	}
	if(y <= 0){
	y = 0;	
	}
    one_pixel_address =  pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
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

void update_cursor(void){
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
	PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
	RVALID = PS2_data & 0x8000; // extract the RVALID field    
	if(RVALID != 0) {
		b1 = b2;
		b2 = b3;
		b3 = PS2_data & 0xFF;	
	}	
	HEX_PS2(b1, b2, b3);
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
	led_array[0] = direction_array[0] == 1;
	led_array[1] = direction_array[1] == 1;
	led_array[2] = direction_array[2] == -1;
	led_array[3] = direction_array[3] == -1;
}