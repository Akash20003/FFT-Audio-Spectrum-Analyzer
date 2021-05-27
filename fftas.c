#include "U8glib.h"
#include "fix_fft.h"


// LCD SPI SCK-EN, MOSI-RW & SS-CS

#define EN 6
#define RW 5
#define CS 4
// display set up, bar, line position L & R
#define LINEY 50
#define LINEXL 0
#define LINEXR 128

#define SAMPLES 128

#define AUDIO A0

U8GLIB_ST7920_128X64_1X lcd(EN, RW, CS); // serial use, PSB = GND

char im[SAMPLES];
char data[SAMPLES];
int barht[SAMPLES];

void setup()
{
  lcd.begin(); // inti display
  
}

void loop()
{
  static int i, j;
  int val;
  

  // get audio data
  for(i = 0; i < SAMPLES; i++)
  {
    val = analogRead(AUDIO); // 0-1023
    data[i] = (char)(val/4 - 128); // store as char
    im[i] = 0; // init all as 0
  }


  // run FFT
  fix_fft(data, im, 7, 0);

  // extract absolute value of data only, for 64 results
  for(i = 0; i < SAMPLES/2; i++)
  {
    barht[i] = (int)sqrt(data[i] * data[i] + im[i] * im[i]);
  }
  
  for(i = 0, j = 0; i < SAMPLES/2; i++, j += 2)
  {
    barht[i] = barht[j] + barht[j + 1];
  }
  
  // display barchart
  barchart(SAMPLES/4, barht); // plot SAMPLES / 4 = 32 as barchart gen cannot handle 128 bars
} 

// plot line and bar at position and height
void barchart(int n, int bh[])
{
  int i, s, w; // bars, spacing and width
  
  s = (LINEXR - LINEXL) / n;
  w = s / 2;
  
  lcd.firstPage();
  do
  {
    lcd.setFont(u8g_font_helvR08);
    lcd.drawStr(20, 10, "FFT Audio Spectrum");
    lcd.drawLine(LINEXL, LINEY, LINEXR, LINEY);
    lcd.drawStr(0, LINEY + 10, "0");
    lcd.drawStr(29, LINEY + 10, "1k");
    lcd.drawStr(59, LINEY + 10, "2k");
    lcd.drawStr(91, LINEY + 10, "3k");
    lcd.drawStr(115, LINEY + 10, "Hz");
    
    for(i = 0; i < n; i++)
    {
      lcd.drawBox(LINEXL + s * i, LINEY - bh[i], w, bh[i] + 1); // u8glib doesn't accept box height of 0
    }
      
  }while(lcd.nextPage()); 
}
