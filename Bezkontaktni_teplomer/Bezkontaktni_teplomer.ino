// --- KÓD --- JIŘÍ BIELIK, LEDEN 2021 

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

Adafruit_SSD1306 display(-1); // display(128,64, &Wire, -1);

float roomTemp;
float objectTemp, stemp;

int readcount = 0;
float threshold = 5.6;

// --- Ultrazvukový senzor vdálenosti ---

#define echoPin 11
#define trigPin 12
int maximumRange = 30; //vzdálenost, do které to měří
int minimumRange = 0; //vzdálenost, od které to měří
long duration, distance; //vypočítání vzdálenosti
int dtime;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  //nastavení OLED displeje
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2; //vypočítá vzdálenost (v cm) podle rychlosti vzduchu

  //snímání teploty
  objectTemp = threshold + mlx.readObjectTempC();
  roomTemp = mlx.readAmbientTempC();

  //výpis do serial port
  Serial.println("Object:" + String(objectTemp) + ", Ambient:" + String(roomTemp));
  Serial.println(distance);

  //OLED displej
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,25);
  display.print("Dis:" + String(distance) + "cm");
  display.setCursor(65,25);
  display.print("Room:" + String(roomTemp).substring(0, 4) + "C");
  display.display();
  display.setTextSize(2);
  display.setCursor(0, 0);
  if (distance > maximumRange) {
      display.print("GET CLOSER");
    }
  if (distance < minimumRange) {
      display.print("TOO CLOSE!");
    }
  if ((distance >= minimumRange) && (distance <= maximumRange))
  {
      if (readcount == 5)
      { //snímání 5 hodnot
        disptemp();
      } else {
          display.print("HOLD ON");
          stemp = stemp + objectTemp;
          readcount++;
          dtime = 200;  // do 1 sec (5 * 200ms)
      }
      } else {  //když vyjde z určené vzdálenosti
          dtime = 100;
          readcount = 0;
          stemp = 0;
        }
        display.display();
        delay(dtime);
        Serial.println("count :" + String(readcount));
    }
    void disptemp()
    {
        objectTemp = stemp / 5;    //průměrná hodnota
        display.setTextSize(1);
        display.print("YOUR TEMP: ");
        display.setTextSize(2);
        display.setCursor(60,5);
        display.print(String(objectTemp).substring(0, 4) + "C");
        display.display();
        readcount = 0;
        stemp = 0;
        if (objectTemp >= 37.5) {
            play_alert();
        } else {
              play_ok();
        }
        dtime = 5000;
    }
    void play_ok() //3 jiné tóny když objekt je pod 37.5°C
    {
        tone(3, 600, 1000);  //pin (3), frekvence(600), doba(1000ms)
        delay(200);
        tone(3, 750, 500); 
        delay(200);
        tone(3, 1000, 500); 
        delay(200);
        noTone(3);
    }
    void play_alert()   //3 pípne když objekt je nad 37.5°C
    {
        tone(3, 1000, 500); 
        delay(1000);
        tone(3, 2000, 500); 
        delay(1000);
        tone(3, 3000, 500); 
        delay(1000);
        noTone(3);
    }
