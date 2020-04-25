#include <NeoPixelBus.h>

const uint16_t PixelCount = 31; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 7;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128
#define fast_animation 25
#define slow_animation 150

const int p1_button = 8;
const int p2_button = 9;

bool p1_primed = false;
bool p2_primed = false;

int  which_way = 1;
int current_position = 15;

int p1_score = 0;
int p2_score = 0;

int initial_velo = 100;
int dot_speed;
int velo_increment = 6;

int max_score = 14;

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);


RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

RgbColor col = white;

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    pinMode(p1_button, INPUT_PULLUP);
    pinMode(p2_button, INPUT_PULLUP);
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();


    Serial.println();
    Serial.println("Running...");
    bootup_animation();
    dot_speed = initial_velo;
    delay(1000);
}


void loop()
{
   
   int reading_p1 = digitalRead(p1_button);
   int reading_p2 = digitalRead(p2_button);
 
   if (reading_p1 == LOW) {
    p1_primed = true;
   }
   else p1_primed = false;
   
   if (reading_p2 == LOW) {
    p2_primed = true;
   }
   else p2_primed = false;



   move_once(col, dot_speed, which_way);
   
   check_death();
   check_win();

   
   if (p1_primed == true) {
    if (digitalRead(p1_button) == HIGH && which_way == 2){
      switch_direction();
    }
   }
   if (p2_primed == true) {
    if (digitalRead(p2_button) == HIGH && which_way == 1){
      switch_direction();
    }
   }
   
}
//==========================

void show_score(){
  set_all_leds(black);
   for (int i=0; i <= p1_score-1; i++)
    {
      strip.SetPixelColor(i, red);
    }
    for (int i=PixelCount; i >= PixelCount-p2_score; i--)
    {
      strip.SetPixelColor(i, blue);
    }
    strip.Show();
    delay(1000);
    restart();
}

void restart()
{
  set_all_leds(black);
  dot_speed = initial_velo;
  current_position = 15;
}

void check_death()
{
  if (current_position == 0){
    p2_score++;
    if (p1_score > 0 || p2_score > 0) {
      show_point(white, 2);
      show_score();
   }
  }
   if (current_position == 31){
    p1_score++;
    if (p1_score > 0 || p2_score > 0) {
      show_point(white, 1);
      show_score();
   }
  }

}

void show_point(RgbColor win, int player)
{
  if (player == 1){
    for (int i=0; i <= PixelCount/2; i++) {
      strip.SetPixelColor(i, win);
    }
    strip.Show();
    delay(slow_animation);
  }
    if (player == 2){
    for (int i=PixelCount; i >= PixelCount/2; i--) {
      strip.SetPixelColor(i, win);
    }
    strip.Show();
    delay(slow_animation);
  }
}

void check_win()
{
  if (p1_score >= max_score) {
    celebration(red);
  }
  if (p2_score >= max_score) {
    celebration(blue);
  }
}

void reset_score(){
  p1_score = 0;
  p2_score = 0;
}

void celebration(RgbColor winner)
{
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(winner);
    delay(slow_animation);
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(winner);
    delay(slow_animation);
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(winner);
    delay(slow_animation);
    set_all_leds(black);

    reset_score();
}

void switch_direction()
{
  speed_update();
  if (which_way == 1 && current_position > 15) {
    which_way = 2;
    col = blue;
  }
  if (which_way == 2 && current_position < 15) {
    which_way = 1;
    col = red;
  }
}

void speed_update(){
  //how close to player?
  if (current_position > 15){
    int dist_from_mid = current_position - 15;
    dot_speed = initial_velo - dist_from_mid * velo_increment;
  }
    else {
    int dist_from_mid = 15 - current_position;
    dot_speed = initial_velo - dist_from_mid * velo_increment;
  }
}

void move_once(RgbColor color, int move_speed, int dir)
{
  strip.SetPixelColor(current_position, black);
  if (dir == 1)
  {
    strip.SetPixelColor(current_position+1, color);
    current_position++;
  }
    if (dir == 2)
  {
    strip.SetPixelColor(current_position-1, color);
    current_position--;
  }
  strip.Show();
  delay(move_speed);
}


void set_all_leds(RgbColor color)
{
   for (int i=0; i <= PixelCount; i++)
    {
      strip.SetPixelColor(i, color);
    }
    strip.Show();
}

void scan_leds(RgbColor color, int direction, bool stay)
{
  if (direction == 1)
  {
    for (int i=0; i <= PixelCount; i++)
    {
      strip.SetPixelColor(i, color);
      strip.Show();
      delay(fast_animation);
      if (stay == false)
      {
        strip.SetPixelColor(i, black);
      }
    }
  }
    if (direction == 2)
  {
    for (int i=PixelCount; i >= 0; i--)
    {
      strip.SetPixelColor(i, color);
      strip.Show();
      delay(fast_animation);
      if (stay == false)
      {
        strip.SetPixelColor(i, black);
      }
    }
  }
  
    
}

void bootup_animation()
{
    scan_leds(red, 1, false); 
    scan_leds(blue, 2, false);
    scan_leds(green, 1, true);
    scan_leds(red, 2, true);
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(white);
    delay(slow_animation);
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(white);
    delay(slow_animation);
    set_all_leds(black);
    delay(slow_animation);
    set_all_leds(white);
    delay(slow_animation);
    set_all_leds(black);
}
