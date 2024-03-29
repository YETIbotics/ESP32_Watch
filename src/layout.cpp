#include <Arduino.h>

#include "layout.h"
#include "fonts.h"
#include "util.h"

static const int HEIGHT_INDEX = 1; // From the font format at http://oleddisplay.squix.ch/

static const char * fonts[] = {Lato_Regular_32, Lato_Semibold_26, Lato_Light_24, Lato_Regular_20, Lato_Hairline_16, Lato_Regular_12, (const char*)ArialMT_Plain_10};

static const OLEDDISPLAY_TEXT_ALIGNMENT alignments[] = {TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT};



#ifdef COLOR_SCREEN


#include "SSD1331Extended.h"

const uint8_t SCLK_OLED =  14; //SCLK
const uint8_t MOSI_OLED =  13; //MOSI (Master Output Slave Input)
const uint8_t MISO_OLED =  12; // (Master Input Slave Output)
const uint8_t CS_OLED = 15;
const uint8_t DC_OLED =  16; //OLED DC(Data/Command)
const uint8_t RST_OLED =  4; //OLED Reset
  
SSD1331Extended display(SCLK_OLED, MISO_OLED, MOSI_OLED, CS_OLED, DC_OLED, RST_OLED);

#else

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
SSD1306  display(0x3c, 4, 15); // addr, SDA, SCL (reset = GPIO16)


#endif


void Layout::init(bool leftHanded) {
  #ifdef COLOR_SCREEN
    display.SSD1331_Init();
     display.CommandWrite(0xA0); //Remap & Color Depth setting　
     display.CommandWrite(0b01110010); //A[7:6] = 01; 65k color format

   #else
  display.init();

  if (leftHanded) {
    display.flipScreenVertically();
  }
#endif
}

void Layout::setContrast(int val) {
#ifdef COLOR_SCREEN
  display.Brightness(val);
#else
  display.setContrast(val);
#endif

}

const char * Layout::drawStringInto(int x, int y, int w, int h, String str, Alignment align) {

#ifdef COLOR_SCREEN
  display.setTextAlignment(alignments[(int)align]);
      display.setFont(Layout::getFontForSize(str, w,h));
        display.drawString(x, y, str);
#else
  static const int yOffset =  (h>22 ? 3:1);
  
	display.setColor(BLACK);
	display.fillRect(x, y + yOffset, w, h - yOffset);
	display.setColor(WHITE);
  
	//display.drawRect(x, y + yOffset, w, h - yOffset);	
	
	display.setTextAlignment(alignments[(int)align]);
	    display.setFont((const uint8_t*)Layout::getFontForSize(str, w,h));
        display.drawString(x, y, str);
#endif
}
  
  const char * Layout::drawDigitsInto(int x, int y, int w, int h, int hr, int m, char separator, int s) {
	         char buffer[20];
			 sprintf(buffer, "%02d:%02d:%02d", hr, m, s);
			 drawStringInto(x, y, w, h, buffer);

    }
	
    const char * Layout::drawDigitsInto(int x, int y, int w, int h, int m, int s) {
  	         char buffer[20];
  			 sprintf(buffer, "%02d:%02d", m, s);
  			 drawStringInto(x, y, w, h, buffer);

      }
  
  const char * Layout::getFontForSize( String str, unsigned char width, unsigned char height) {
  	for (int i = 0; i < NUM_OF(fonts); i++ )
  	{
		auto fontHeight = fonts[i][HEIGHT_INDEX];
		if (fontHeight > 22) {
			fontHeight -= 5;
		}

  		if (height >= fontHeight) {
			display.setFont((const uint8_t*)fonts[i]);
			int w = display.getStringWidth(str);
			if (w <= width) {
  				return fonts[i];
			}
  		}
  	}
  	return fonts[NUM_OF(fonts)-1];
  }
  
const char * Layout::getFontForHeight(unsigned char height) {

	for (int i = 0; i < NUM_OF(fonts); i++ )
	{
		auto fontHeight = fonts[i][HEIGHT_INDEX];
		if (height >= fontHeight) {
			return fonts[i];
		}
	}
	return fonts[NUM_OF(fonts)-1];
}

void Layout::clear() {
  #ifdef COLOR_SCREEN
  display.Display_Clear(0,0,DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1);
#else
 /*         display.setColor(BLACK);
display.fillRect(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1);
display.setColor(WHITE);*/
display.clear();
#endif
}

void Layout::swapBuffers() {
    #ifdef COLOR_SCREEN
#else
  display.display();
  #endif
}

void Layout::enableDisplay(bool on) {
    #ifdef COLOR_SCREEN
      display.CommandWrite(on ? 0xAF:0xAE);
#else
  if (on) {
    display.displayOn();
  }
  else {
    display.displayOff();    
  }
  #endif
}

void Layout::fillRect(int x, int y, int w, int h, Color color) {
    #ifdef COLOR_SCREEN
    display.Drawing_Rectangle_Fill(x, y, w, h, 10,10,10, 40,0,0);
    
#else
      display.setColor(color == 0 ? BLACK : WHITE);
   
      display.fillRect(x, y, w, h);
      #endif
}

void Layout::drawLine(int x, int y, int x2, int y2, Color color) {
    #ifdef COLOR_SCREEN
    display.Drawing_Line(x,y,x2,y2, 3, 3, 3);
#else
      display.setColor(color == 0 ? BLACK : WHITE);
      display.drawLine(x, y, x2, y2); 
      #endif
}

