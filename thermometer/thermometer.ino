//LCD screen defines
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//Temperature sensor defines
#include <Wire.h>
#include <Adafruit_MLX90614.h>
 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

struct _temperature 
{
  double degC;
  double degF;
} temperatureObj;

//Battery monitoring

#define VBATPIN A13

float voltage() {
  float measuredvbat = analogRead(VBATPIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  measuredvbat = measuredvbat * (2.0 * 3.3 *1.1 ) / 4095;
  // return the value you read:
  return measuredvbat;
  
}

//Starting Animation 
void startAnimation()
{
  Triangles();
  delay(500);
  FilledTriangles();
  delay(500);
}

void setup() {

  int i;
  Serial.begin(115200);

  delay(10);
  
  tft.begin();
  tft.setRotation(2);

  Serial.print(F("Start Animation       "));
  startAnimation();

  double batt = voltage();
  String batt_status;
  
  Serial.println(batt);
  
  if (batt >= 4) // Freshly topped off
  {
  
    Serial.println("Batt Full");
    batt_status = "100 %";
    delay(1000);  
  }
  else if (voltage() >= 3.7) // full charge
  {
    Serial.println("Batt Healthy");
    batt_status = "Healthy";
    delay(1000);  
  }
  else if (voltage() >= 3.4) // linear
  {
    Serial.println("Batt Moderate");
    batt_status = "Moderate";
    delay(1000);  
  }
  else // dead
  {
    Serial.println("Batt Empty");
    batt_status = "Low";
    delay(1000);  
  }

  mlx.begin();
  
  for (i = 0; i < 10; i++)
  {

    Serial.print("*C\tObject = ");
    Serial.print(mlx.readObjectTempC());
    Serial.println("*C");
    temperatureObj.degC = mlx.readObjectTempC();

    Serial.print("*F\tObject = ");
    Serial.print(mlx.readObjectTempF());
    Serial.println("*F");
    temperatureObj.degF = mlx.readObjectTempF();

    lcdDisplay(temperatureObj, batt_status);
    delay(1000);
  }
}

void loop(void) {
  }

unsigned long lcdDisplay(struct _temperature t, String batt) {

  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.println();
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(3);
  tft.println("TEMPERATURE");
  tft.println();
  tft.println();
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_RED);
  tft.print(t.degC);
  tft.println(" deg C");
  tft.println();
  tft.setTextSize(3);
  tft.print(t.degF);
  tft.println(" deg F");
  tft.println();
  tft.println();
  tft.print("BATT:");
  tft.print(batt);
  return micros() - start;
}


void Triangles() {
  int n, i;
  int cx = tft.width()  / 2 - 1;
  int cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(cx, cy);
  for(i=0; i < n; i +=5) {
    tft.drawTriangle(
          cx, cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(i, i, i));
  }
}

void FilledTriangles() {
  int           i;
  int cx = tft.width()  / 2 - 1;
  int cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  for(i=min(cx,cy); i>10; i-=5) {
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i*10, i*10));
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i*10, i*10, 0));
    yield();
  }
}
