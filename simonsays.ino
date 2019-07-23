#define led5 5      //red button led
#define led6 6      //green button led
#define led7 7      //yellow button led
#define led8 8      //blue button led

#define button5  9  //red button
#define button6  10 //green button
#define button7  11 //yellow button
#define button8  12 //blue button

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define totalLevels 30
#define buzzer 3

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int simonSaid[totalLevels];
int tones[4] = {950, 850, 750, 650};

void setconfig(){
  pinMode(led7, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led8, OUTPUT);

  digitalWrite(led7, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led8, LOW);

  pinMode(button7, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button8, INPUT_PULLUP);
}

void sort(){

  randomSeed(analogRead(0));

  for (int i = 0; i <= totalLevels; i++) {
      simonSaid[i] = random(5, 9);
      Serial.print(String(simonSaid[i]) + ", ");
  }
  Serial.println("");  
}

void displayShow(String text, int tsize){
  display.clearDisplay();
  display.setTextSize(tsize);
  display.setTextColor(WHITE);
  display.setCursor(1, 0);
  display.println(text);
  display.display();
}

void displayShow(String text, String text2, int tsize){
  display.clearDisplay();
  display.setTextSize(tsize);
  display.setTextColor(WHITE);
  display.setCursor(1, 0);
  display.println(text);
  display.print(text2);
  display.display();
}

void setup() {
  Serial.begin(9600);
  setconfig();
  sort();
  
  // Address 0x3C for 128x32
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Falha no Display"));
    for(;;);
  }

  displayShow("GENIUS", 3);
  delay(2000);
}

void loop() {

  bool gameOver = 0;
    
  while (!gameOver) {

    for (int i = 1; i < totalLevels; i++) {

      displayShow("Nivel " + String(i), (i < 10? 3: 2));
      delay(1400);

      playLevel(i);

      for (int g = 0; g < i; g++) {

        int buttonPressed = readBtn();

        Serial.println("button pressed = " + String(buttonPressed));
    
        playBuzzer(buttonPressed, 300);

        Serial.println("gabarito = " + String(simonSaid[g]));
        Serial.println("result = " + String(!buttonPressed == (simonSaid[g])));
        
        if (!(buttonPressed == simonSaid[g])) {
          displayShow("GAME OVER", "Total: " + String(i-1), 2);
          playGameOver();
          if (i > 10){
            displayShow("Brocou!", 3);
            delay(2000);
          }
          gameOver = 1;
          break;
        }
      }

      if (gameOver) break;
    }
  }
}

int readBtn(){
    int buttonPressed = 0;

    while (!buttonPressed) {
      if (!digitalRead(button8)){
        buttonPressed = 8;
      }
      if (!digitalRead(button7)){
        buttonPressed = 7;
      }
      if (!digitalRead(button6)){
        buttonPressed = 6;
      }
      if (!digitalRead(button5)){
        buttonPressed = 5;
      }                                          
    }

    return buttonPressed;
}

void playLevel(int i){
    for (int g = 0; g < i; g++) {
      playBuzzer(simonSaid[g], 300);
      delay(100); // pausa entre uma luz e outra
    }
}

void softReset() {
  asm volatile ("  jmp 0");
}

void playGameOver(){

    int endgame = 660;

    tone(buzzer, endgame);
    digitalWrite(led7, HIGH);
    digitalWrite(led6, HIGH);
    digitalWrite(led5, HIGH);
    digitalWrite(led8, HIGH);
    
    delay(250);
    noTone(buzzer);
    
    delay(50);
    tone(buzzer, endgame);          
    
    delay(250);
    noTone(buzzer);
    
    delay(650);
    digitalWrite(led7, LOW);
    digitalWrite(led6, LOW);
    digitalWrite(led5, LOW);
    digitalWrite(led8, LOW);

    delay(2000);

   // softReset();
}

void playBuzzer(int button, int mseconds) {
  
  digitalWrite(button, HIGH);

  tone(buzzer, tones[button - 5]);
  delay(mseconds);

  digitalWrite(button, LOW);

  noTone(buzzer);
}
