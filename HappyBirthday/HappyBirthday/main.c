#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "i2c.h"
#include "SSD1306.h"

// Balloon bitmap (16x16 pixels)
const unsigned char BALLOON_BITMAP[] = {
	0x00, 0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0xF0, 0x07, 0xF0,
	0x0F, 0xF8, 0x0F, 0xF8, 0x0F, 0xF8, 0x07, 0xF0, 0x07, 0xF0, 0x03, 0xE0,
	0x01, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80
};

// Function to display a simple balloon at specified position
void OLED_DisplayBalloon(uint8_t x, uint8_t y) {
	uint8_t width = 16;  // Balloon width in pixels
	uint8_t height = 16; // Balloon height in pixels
	uint8_t i, j, byte;
	
	// Position cursor at the specified coordinates
	for (j = 0; j < height/8; j++) {  // For each page (8 vertical pixels)
		// Skip if this would overlap with the birthday text (top 2 rows)
		if (y/8 + j < 2) {
			continue;
		}
		
		OLED_SetCursor(y/8 + j, x);
		
		oledSendStart(SSD1306_ADDRESS);
		oledSendByte(SSD1306_DATA_CONTINUE);
		
		for (i = 0; i < width; i++) {  // For each column
			byte = BALLOON_BITMAP[i + (j * width)];
			oledSendByte(byte);
		}
	}
	
	// Add the balloon string - only if it doesn't overlap with text
	if (y/8 + 2 >= 2) {
		OLED_SetCursor(y/8 + 2, x + 7);  // Position for the string
		oledSendStart(SSD1306_ADDRESS);
		oledSendByte(SSD1306_DATA_CONTINUE);
		for (i = 0; i < 3; i++) {
			oledSendByte(0x80);  // Vertical line
		}
	}
}

// Structure to track balloon position
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t prev_y; // Track previous y position to clear old balloon
} Balloon;

// Function to display three floating balloons animation without clearing screen
void OLED_AnimateBalloons() {
	// Initialize three balloons at different horizontal positions
	Balloon balloons[3] = {
		{30, 64, 64},   // Left balloon
		{56, 64, 64},   // Middle balloon
		{82, 64, 64}    // Right balloon
	};
	
	uint8_t j;
	
	// First, draw the birthday message
	OLED_SetCursor(0, 0);
	OLED_Printf("HAPPY BIRTHDAY HANNAH");
	OLED_SetCursor(1, 0);
	OLED_Printf("!!!!!!!!!!!!!!!!!!!!!");
	
	// Endless balloon animation
	while (1) {
		// Update and display each balloon
		for (j = 0; j < 3; j++) {
			// Clear previous balloon position (only portion that changed)
			if (balloons[j].prev_y < 64 && balloons[j].prev_y != balloons[j].y) {
				uint8_t page = (balloons[j].prev_y + 15) / 8; // Get bottom page of previous position
				
				// Only clear if not overlapping with text area
				if (page >= 2) {
					OLED_SetCursor(page, balloons[j].x);
					oledSendStart(SSD1306_ADDRESS);
					oledSendByte(SSD1306_DATA_CONTINUE);
					for (uint8_t k = 0; k < 16; k++) {
						oledSendByte(0x00); // Clear the bottom row
					}
				}
			}
			
			// Store current position for next frame's clearing
			balloons[j].prev_y = balloons[j].y;
			
			// Move balloon up
			balloons[j].y--;
			
			// If balloon reaches top, reset it to bottom
			if (balloons[j].y <= 0) {
				balloons[j].y = 64; // Reset to below screen
			}
			
			// Display balloon if it's on screen
			if (balloons[j].y < 64) {
				OLED_DisplayBalloon(balloons[j].x, balloons[j].y);
			}
		}
		
		_delay_ms(100);  // Consistent speed for all balloons
	}
	
	// Optional: redraw birthday message at the end to clean up any artifacts
	OLED_SetCursor(0, 0);
	OLED_Printf("HAPPY BIRTHDAY HANNAH");
	OLED_SetCursor(1, 0);
	OLED_Printf("!!!!!!!!!!!!!!!!!!!!!");
}


int main(void) {
	// Initialize hardware

	OLED_Init();
	OLED_Clear();
	
	// Setup pins

	
	// Seed random number generator
	srand(42);  // Fixed seed for now
	
	// Enable global interrupts
	sei();
	
	// Reset game state

	
	// Main game loop
	while (1) {

			// Clear screen once at the beginning
			OLED_Clear();
			
			// Display birthday message and animate balloons endlessly
			OLED_AnimateBalloons();
			
			// If we exit the animation, wait a moment before potentially restarting
			_delay_ms(1000);
			break;
			
			// Your other game states
		
	}
}