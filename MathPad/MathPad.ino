//#include <KeyboardAzertyFr.h> // French keyboard
#include <Elegoo_TFTLCD.h>    // Hardware-specific library
#include <TouchScreen.h>      // Touch enabling library
#include <Keyboard.h>       // Keyboard emulation library
#include <SD.h>               // SD card reading library



/*
 * Screen
 */
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 20
#define TS_MAXY 920
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/*
 * SD reading
 */
File file;

#define MAX_BMP         10                      // bmp file num
#define FILENAME_LEN    20                      // max file name length

const int __Gnbmp_height = 320;                 // bmp hight
const int __Gnbmp_width  = 240;                 // bmp width

unsigned char __Gnbmp_image_offset  = 0;        // offset


/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL       60                      // must be a divisor of 240 
#define BUFFPIXEL_X3    180                     // BUFFPIXELx3

void bmpdraw(File f, int x, int y)
{
    file.seek(__Gnbmp_image_offset);

    //uint32_t time = millis();

    uint8_t sdbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

    for (int i=0; i< __Gnbmp_height; i++) {
        for(int j=0; j<(240/BUFFPIXEL); j++) {
            file.read(sdbuffer, BUFFPIXEL_X3);
            
            uint8_t buffidx = 0;
            int offset_x = j*BUFFPIXEL;
            unsigned int __color[BUFFPIXEL];
            
            for(int k=0; k<BUFFPIXEL; k++) {
                __color[k] = sdbuffer[buffidx+2]>>3;                        // read
                __color[k] = __color[k]<<6 | (sdbuffer[buffidx+1]>>2);      // green
                __color[k] = __color[k]<<5 | (sdbuffer[buffidx+0]>>3);      // blue
                
                buffidx += 3;
            }

      for (int m = 0; m < BUFFPIXEL; m ++) {
              if(__color[m]!=WHITE){
                tft.drawPixel(m+offset_x, i,__color[m]);
              }
      }
        }
    }
    
    //Serial.print(millis() - time, DEC);
    //Serial.println(" ms");
}

boolean bmpReadHeader(File f) 
{
    // read header
    uint32_t tmp;
    uint8_t bmpDepth;
    
    if (read16(f) != 0x4D42) {
        // magic bytes missing
        return false;
    }

    // read file size
    tmp = read32(f);
    //Serial.print("size 0x");
    //Serial.println(tmp, HEX);

    // read and ignore creator bytes
    read32(f);

    __Gnbmp_image_offset = read32(f);
    //Serial.print("offset ");
    //Serial.println(__Gnbmp_image_offset, DEC);

    // read DIB header
    tmp = read32(f);
    //Serial.print("header size ");
    //Serial.println(tmp, DEC);
    
    int bmp_width = read32(f);
    int bmp_height = read32(f);
    
    if(bmp_width != __Gnbmp_width || bmp_height != __Gnbmp_height)  {    // if image is not 320x240, return false
        return false;
    }

    if (read16(f) != 1)
    return false;

    bmpDepth = read16(f);
    //Serial.print("bitdepth ");
    //Serial.println(bmpDepth, DEC);

    if (read32(f) != 0) {
        // compression not supported!
        return false;
    }

    //Serial.print("compression ");
    //Serial.println(tmp, DEC);

    return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f)
{
    uint16_t d;
    uint8_t b;
    b = f.read();
    d = f.read();
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f)
{
    uint32_t d;
    uint16_t b;

    b = read16(f);
    d = read16(f);
    d <<= 16;
    d |= b;
    return d;
}


/*
 * MENU
 */
int page=1;
#define PAGES 5
#define STEP  40
#define STEP3 37
//load a menu
void menu(){
  page = page%PAGES;
  tft.fillScreen(WHITE);
  //fast lines
  if(page!=4){
    for(int i=0; i<8;i++){
      tft.drawFastHLine(0,STEP*i, 240, BLACK);
    }
    for(int i=0; i<6;i++){
      tft.drawFastVLine(STEP*i, 0, 320, BLACK);
      
    }
    tft.drawFastVLine(STEP*3, 1, STEP, WHITE);

    tft.fillRect(STEP*0+2,2,STEP3,STEP3,BLACK);
    tft.fillRect(STEP*1+2,2,STEP3,STEP3,BLACK);
    tft.fillRect(STEP*4+2,2,STEP3,STEP3,BLACK);
    tft.fillRect(STEP*5+2,2,STEP3,STEP3,BLACK);
  }
    

  //delay(5000);
  //int time = millis();

  //load image
  file = SD.open(String(page)+"menu.bmp");
  //Serial.println(String(page)+"menu.bmp");
  if(! bmpReadHeader(file)) {}
  bmpdraw(file, 0, 0);
  file.close();
  delay(5);
  //Serial.print(millis() - time);

  
  
}


#define STEP 40
#define BOX 39

void setup(void) {
  //Serial.begin(9600);

  //Init screen
  tft.begin(0x9341);
  tft.setRotation(2);
  
  //Init SD
  pinMode(10, OUTPUT);
  SD.begin(10);
  
  //init screen
  menu();
  
  //init keyboard
  Keyboard.begin();
}
/*
 * 
 */



#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define WAIT        20

byte lastX;
int lastY;
unsigned long last;

void loop()
{
  //get pressure
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) { //test if there is pressure
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    p.y =  map(p.y, 20, 320, 0, 320) ;
    //scale to grid
    byte x=(p.x-(p.x%STEP))/STEP;
    int y=(p.y-(p.y%STEP))/STEP;
    
    if( (x!=lastX) || (y!=lastY) || ((millis()-last)>330) ) { //test if pressure isn't capture twice
      if(y!=0){ //proper key
        //feedback
        tft.drawRect(lastX*STEP+1, lastY*STEP+1, BOX, BOX, WHITE);
        tft.drawRect(x*STEP+1, y*STEP+1, BOX, BOX, BLACK);
        ////send text
        //open code
        file = SD.open(String(page)+"-"+String(y)+String(x)+".key");
        //set QWERTY keyboard
        Keyboard.press(KEY_LEFT_ALT);
        delay(WAIT);
        Keyboard.write(KEY_LEFT_SHIFT);
        delay(WAIT);
        Keyboard.releaseAll();
        delay(WAIT);
        //write code
        
        while (file.available()) {
          //Keyboard.print(file.read());
          char ch=file.read();
          if(ch==63){ //if '?'
            Keyboard.write(KEY_LEFT_ARROW);
          }
          else{
            Keyboard.print(ch);
          }
          //Keyboard.print(ch);
          //Serial.println(int(ch));
          
        }
        
        //set back AZERTY keyboard
        Keyboard.press(KEY_LEFT_ALT);
        delay(WAIT);
        Keyboard.write(KEY_LEFT_SHIFT);
        delay(WAIT);
        Keyboard.releaseAll();
        delay(WAIT);
        //close code
        file.close();
      }
      else{ //change menu page
        switch (x) {
          case 0:
            page++;
            page++;
            page++;
            page++;
            menu();
            break;
          case 1:
            page++;
            page++;
            page++;
            menu();
            break;
          case 4:
            page++;
            page++;
            menu();
            break;
          case 5:
            page++;
            menu();
            break;
        }
      }
      
      //remember last touch
      lastX=x;
      lastY=y;
      last=millis();
    }
  }
}
