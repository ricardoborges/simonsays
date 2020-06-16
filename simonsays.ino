#define led5 5      //green button led
#define led6 6      //blue button led
#define led7 7      //yellow button led
#define led8 8      //red button led

#define button5  9  //green button
#define button6  10 //blue button
#define button7  11 //yellow button
#define button8  12 //red button

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define totalLevels 28
#define buzzer 3

#define endgame 660

#define BUFSIZE 10

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int simonSaid[totalLevels];
int tones[4] = {950, 850, 750, 650};
int tspeed[4] = {500, 400, 300, 200};
int tsilence[4] = {200, 150, 100, 80};

int option;
bool gameOver = 0;
int buttonPressed = 0;

char buf[BUFSIZE];

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
  
  //Config
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
   
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(("Falha no Display"));
    for(;;);
  }
  Serial.println("display genius");
  displayShow("GENIUS", 3);
  delay(2000);

  eeprom_read_string(0, buf, BUFSIZE);
  Serial.println(buf);
  
  displayShow(buf, 2);
  delay(2000);
  
  menuDificuldade();
}

void sort(){
  randomSeed(analogRead(0));

  for (int i = 0; i <= totalLevels; i++) {
      simonSaid[i] = random(5, 9);
      Serial.print(String(simonSaid[i]) + ", ");
  }
}

void loop() {
  sort();
  gameOver = 0;  
    
  while (!gameOver) {

    for (int i = 1; i < totalLevels; i++) {
      
      displayShow("Nivel " + String(i), (i < 10? 3: 2));
      delay(1400);

      playLevel(i);
     
        for (int g = 0; g < i; g++) {
  
          buttonPressed = readBtn();
  
          playBuzzer(buttonPressed, tspeed[option]);
  
          //Manter 100 mseconds 
          switch(option){
            case 2: delay(20); break;
            case 3: delay(40); break;
          }
            
          if (!(buttonPressed == simonSaid[g])) {

              displayShow("GAME OVER", "Total: " + String(i-1), 2);

              playGameOver();
              
              String myString = "Record: " + String(i-1);
              myString.toCharArray(buf, BUFSIZE); 
              eeprom_write_string(0, buf); 
              
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
    buttonPressed = 0;

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
      playBuzzer(simonSaid[g], tspeed[option]);
      delay(tsilence[option]); // pausa entre uma luz e outra
    }
}

void menuDificuldade(){
  displayShow("Grau?", 3);

  option = readBtn() - 5;

  switch(option){
    case 0: displayShow("Facil", 3); Serial.println("Facil"); break;
    case 1: displayShow("Normal", 3); Serial.println("Normal"); break;
    case 2: displayShow("Dificil", 3); Serial.println("Dificil"); break;
    case 3: displayShow("Insano", 3); Serial.println("Insano"); break;
  }

  playBuzzer(buttonPressed, 300);
  
  delay(1700);
}

void playGameOver(){

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



const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 511;

boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
  int i;
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }
  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }
  return true;
}

boolean eeprom_write_string(int addr, const char* string) {
  int numBytes; 
  numBytes = strlen(string) + 1;
  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
  byte ch;
  int bytesRead; 
  if (!eeprom_is_addr_ok(addr)) {
    return false;
  }

  if (bufSize == 0) { 
    return false;
  }

  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }
  
  bytesRead = 0;
  ch = EEPROM.read(addr + bytesRead); 
  buffer[bytesRead] = ch;
  bytesRead++;
  
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
  
    ch = EEPROM.read(addr + bytesRead);
    buffer[bytesRead] = ch; 
    bytesRead++;
  }
  
  
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }
  
  return true;
}
