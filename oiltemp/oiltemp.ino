#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

// Declare pins
const int tempInputPin = A6;
const int pressureInputPin = A7;
const int tempPowerOut = A4;
const int pressurePowerOut = A5;
const int modeSwitch = A3;
const int res = A2;
const int DC = A1;
const int CS = A0;

//============================================================================
// Constants - configure for your vehicle and sensors
//============================================================================
const bool demoMode = false; // Set true to sweep values

const long tempPullUpOhms = 1000;
const long maxAnalogVal = 1024; // Highest possible value from analog input

// Temp sensor config, map temp in C to expected ohms, from datasheet
const int arrSize = 12;
const int temps[12] = {40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150}; // In deg C
const int thresholds[12] = {925, 630, 438, 315, 224, 163, 120, 90, 80, 70, 50, 40}; // In ohms

// Pressure sensor config, from datasheet
const int pressureBaseVal = 102; // Analog value at 0 PSI (0.5 V)
const int pressureMaxVal = 922; // Analog value at max pressure (4.5 V)
const int pressureMaxPsi = 100; // Pressure in PSI at pressureMaxVal

// Display config
const int reportIntervalMillis = 1000; // How often the screen should update under nominal conditions
const int standbyMillis = 10000; // Go blank after this time unless alert
const int clickDebounceMillis = 500;

// Temp alert preferences
const int tempColdF = 104; // "Cold" shown below this temp
const int tempCoolF = 150; // "Cool" shown below this temp
const int tempHighAlertF = 280; // "HOT" flashes above this temp

// Pressure alert preferences
const int pressureLowAlertPsi = 15; // "ALERT" flashes below this value
const int pressureLowPsi = 35; // "LOW" shown below this value
const int pressureHighPsi = 85; // "HIGH" shown above this value

//============================================================================
// End Constants
//============================================================================

// Mode variables
enum Mode {STANDBY, UNITF, IDIOT, RAW};
int mode = STANDBY; // initial mode
long lastClickTimeMillis = 0;
long lastReportTimeMillis = 0;
bool alerting = false;

// Sample data buffers
int tempSamples[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int currTempSampleIndex = 0;

// Configure display for 4 wire SPI
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ CS, /* dc=*/ DC, /* reset=*/ res);

void setup() {
  pinMode(tempPowerOut, OUTPUT);
  pinMode(pressurePowerOut, OUTPUT);
  pinMode(modeSwitch, INPUT_PULLUP);

  Serial.begin(9600);
  u8g2.begin();

  // Fill buffers with initial data
  initTempSample();
}

void loop() {
  processClick();
  processData();
}

void processData() {
  // Get data for display
  float tempF = getTempF();
  float pressurePsi = getPressurePsi();

  // Update display if in alert mode or if reporting interval has lapsed
  alerting = isAlert(tempF, pressurePsi);
  if (!alerting && (millis() - lastReportTimeMillis < reportIntervalMillis)) {
    return; // Don't report
  } else {
    lastReportTimeMillis = millis();
  }

  // Write to screen
  switch (mode) {
    case STANDBY:
      drawStandbyMode(tempF, pressurePsi);
      break;
    case UNITF:
      drawUnitText(tempF, pressurePsi);
      break;
    case IDIOT:
      drawIdiotText(tempF, pressurePsi);
      break;
    case RAW:
      drawRawText();
      break;
  }
}

/** Return temp for display */
float getTempF() {
  if (demoMode) {
    return ((millis()/1000) % 10 + 1) * 30; // Demo loop 10 to 300
  } else {
    int tempValue = getSpotAverageTempValue();
    float resistance = getOhmicValue(tempValue);
    int tempC = getTempC(resistance);
    float tempF = cToF(tempC);
    return tempF;
  }
}

/** Return pressure for display */
float getPressurePsi() {
  if (demoMode) {
    return ((millis()/1000) % 10 + 1) * 10; // Demo loop 10 to 100
  } else {
    float pressureValue = getSpotAveragePressureValue();
    float pressurePsi = pressureValueToPsi(pressureValue);
    return pressurePsi;
  }
}

/** Prevents noisy reporting on threshold border */
int getMovingAverageTempValue() {
  sampleTemp(); 
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += tempSamples[i];
  }
  return sum / 10;
}

/** Update one value of temp sample buffer with most recent spot average */
void sampleTemp() {
  int tempValue = getSpotAverageTempValue();
  tempSamples[currTempSampleIndex] = tempValue;
  currTempSampleIndex = (currTempSampleIndex + 1) % 10;
}

/** Fill temp sample buffer */
void initTempSample() {
  sampleTemp();
  while (currTempSampleIndex != 0) {
    sampleTemp();
  }
}

/** Returns average pressure (out of 1024) over 50 ms */
int getSpotAveragePressureValue() {
  digitalWrite(pressurePowerOut, 1);
  delay(5);
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pressureInputPin);
  }
  digitalWrite(pressurePowerOut, 0);
  return sum / 10;
}

void clearScreen() {
  u8g2.firstPage();
  do {} while ( u8g2.nextPage() );
}

/* Blank screen after standby period unless in alert mode */
void drawStandbyMode(int temp, int pressure) {
  if (millis() < standbyMillis || alerting) {
    drawIdiotText(temp, pressure);
  } else {
    clearScreen();
  }
}

void drawUnitText(int tempF, int pressurePsi) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub20_tr);
    
    // Print temp
    u8g2.setCursor(0, 32);
    u8g2.print("T: ");
    if (tempF < cToF(temps[0])) {
       u8g2.print("COLD"); // Show COLD below minimum value
    } else if (tempF < tempHighAlertF || (millis() / 100) % 2 == 0) { // Flash alert if not nominal
      char str[32];
      snprintf(str, 32, "%d F", (int)tempF);
      u8g2.print(str);
    }

    // Print pressure
    u8g2.setCursor(0, 60);
    u8g2.print("P: ");
    
    if (pressurePsi > pressureLowAlertPsi || (millis() / 100) % 2 == 0) { // Flash alert if not nominal
        u8g2.print(pressurePsi);
        u8g2.print(" PSI"); 
    }
    
  } while ( u8g2.nextPage() );
}

void drawIdiotText(int tempF, int pressurePsi) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub20_tr);

    // Print temp
    u8g2.setCursor(0, 32);
    u8g2.print("T: ");
    if (tempF < tempColdF) {
      u8g2.print("COLD");  
    } else if (tempF < tempCoolF) {
      u8g2.print("COOL");
    } else if (tempF >= tempHighAlertF) {
      // Flash alert
      if ((millis() / 100) % 2 == 0) {
        u8g2.print("HOT");  
      }
    } else {
      u8g2.print("OK");
    }
  
  // Print pressure
    u8g2.setCursor(0, 60);
    u8g2.print("P: ");
    if (pressurePsi < pressureLowAlertPsi) {
      // Flash alert
      if ((millis() / 100) % 2 == 0) {
        u8g2.print("ALERT");  
      }
    } else if (pressurePsi < pressureLowPsi) {
      u8g2.print("LOW");  
    } else if (pressurePsi > pressureHighPsi) {
      u8g2.print("HIGH");
    } else {
      u8g2.print("OK");
    }
  } while ( u8g2.nextPage() );
}

void drawRawText() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub20_tr);
    
    // Print temp
    u8g2.setCursor(0, 32);
    u8g2.print("T: ");
    u8g2.print(getSpotAverageTempValue());

    // Print pressure
    u8g2.setCursor(0, 60);
    u8g2.print("P: ");
    u8g2.print(getSpotAveragePressureValue());
    
  } while ( u8g2.nextPage() );
}

/** Returns average temp value (out of 1024) over 50 ms */
int getSpotAverageTempValue() {
  digitalWrite(tempPowerOut, 1);
  delay(5);
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(tempInputPin);
  }
  digitalWrite(tempPowerOut, 0);
  return sum / 10;
}

/** Convert analog value (out of 1024) to ohms */
float getOhmicValue(long digitalValue) {
  long numerator = tempPullUpOhms * digitalValue;
  long denominator = maxAnalogVal - digitalValue;
  return ((float)numerator)/((float)denominator);
}

/** Convert ohms to temp in C using lookup table */
int getTempC(float ohms) {
  if (ohms > thresholds[0]) {
    return 0;
  }
  for (int i = 1; i < arrSize; i++) {
    if (ohms > thresholds[i]) {
      return temps[i - 1];
    }
  }
  
  return temps[arrSize - 1]; // Max temp
}

/** Convert analog pressure value (out of 1024) to PSI */
float pressureValueToPsi(int pressureValue) {
  int range = pressureMaxVal - pressureBaseVal;
  int adjustedValue = pressureValue - pressureBaseVal;
  float psi = (((float) adjustedValue) / ((float) range)) * pressureMaxPsi;
  return psi;
}

float cToF(float tempC) {
  return ((tempC * 9.0) / 5.0) + 32.0;
}

void processClick() {
  bool clicked = !digitalRead(modeSwitch);
  if (clicked) {
    long currTime = millis();
    if (currTime - lastClickTimeMillis > clickDebounceMillis) {
      lastClickTimeMillis = currTime;
      switchMode();
    }
  }
}

/** Cycles to next mode */
void switchMode() {
  mode = (mode + 1) % 3;
  lastReportTimeMillis = 0; // Report immediately
}

/** Returns true if temp exceeds alert threshold or pressure falls below alert threshold */
bool isAlert(int tempF, int pressurePsi) {
  return tempF > tempHighAlertF || pressurePsi < pressureLowAlertPsi;
}
