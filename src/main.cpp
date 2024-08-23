/*
    Project: Infrared Sensors Demonstration
    Author: Travis DeShazo
    Date: October 2023
    Description: 
        This Arduino sketch is designed to demonstrate the functionality of various 
        infrared sensors using an RP2040 microcontroller and a 3.5" TFT SPI display.
        The project incorporates several sensors, each showcasing different infrared 
        applications, to provide a comprehensive overview and practical demonstration 
        to colleagues. The user interface allows for real-time visualization of the 
        sensor data on the TFT display.
        
    Components:
        - RP2040 Microcontroller: Serves as the central processing unit for managing 
          sensors and handling display updates.
          
        - 3.5" TFT SPI Display: Utilized to display real-time data and visualizations 
          from the various sensors.
        
        - JESSINIE PAJ7620U2 Gesture Recognition Sensor: A sensor offering proximity and gesture recognition. (0x73)
          
        - Adafruit MLX90640 IR Thermal Camera: An infrared thermal camera that provides 
          a 32x24 pixel array of infrared temperature readings.
          
        - Adafruit AS7341 10-Channel Light/Color Sensor: Used for precise light and 
          color measurement across multiple channels.
          
        - Adafruit VL53L4CX Time of Flight Distance Sensor: Provides accurate distance 
          measurements up to 6m through time-of-flight calculations of infrared signals.
          
        - uxcell Infrared Through-Beam Reflection Optical Sensor: An optical sensor 
          that detects objects by measuring reflected infrared light.
          
    Functionality:
        The sketch initializes and manages communications with all integrated sensors.
        It reads data from the sensors and updates the display in real-time to visualize 
        the readings. The user interface on the TFT display allows users to navigate 
        through different sensors and view their data. Sensor readings and visualizations 
        are updated on the display, providing a dynamic and interactive experience for 
        demonstrating the capabilities of the infrared sensors.
        
    Note:
        Ensure that all the necessary libraries are installed and updated to the latest 
        versions. Wiring diagrams and additional documentation can be found at [Location/URL].
        
    Usage:
        Power the system and observe the readings from different sensors on the TFT display.
        The display provides visual feedback and representations of the sensor data, offering 
        insights into the functionalities and applications of the various infrared sensors.
        
    Additional Notes:
        This code is meant for educational and demonstration purposes and may require 
        modifications for production or applied usage.
*/

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <DFRobot_PAJ7620U2.h>
#include <vl53l4cx_class.h>
#include <Adafruit_AS7341.h>
TFT_eSPI tft = TFT_eSPI();

// Constants
const int NUM_POSITIONS = 10;
const int ADC_PIN = 29;
uint16_t displayPixelWidth, displayPixelHeight;

// Variables
int position;
int currentPosition = 0;
const int adcValues[NUM_POSITIONS] = {913, 800, 686, 572, 457, 344, 230, 117, 3, 1023};
unsigned long lastSwitchPollTime = 0;


void drawHeader(const char* string);
void drawCenterText(const char* string);

// The Sensor class that serves as an abstraction for individual sensors
class Sensor {
public:
    virtual void setup() = 0;             // Initialization code for the sensor
    virtual void prepare() = 0;           // Prepare between selections (clear display, add new header)
    virtual void updateDisplay() = 0;     // Update the TFT display with the sensor's data
};

// IR Fork - Simple
class IRFork1 : public Sensor {
private:
    int priorVal = HIGH;
public:
  void setup() override {

  }

    void prepare() override {
      drawHeader("IR Break Beam");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
    }
  
  void updateDisplay() override {
    uint16_t readings[12];
    int sensorVal = digitalRead(14);

    if (sensorVal != priorVal) {
      if (sensorVal == LOW) {
        drawCenterText("BLOCKED");
        Serial.println("BLOCKED");
      } else {
        drawCenterText("CLEAR");
        Serial.println("CLEAR");
      }
      priorVal = sensorVal;
    }
  }
};

// IR Fork - Count
class IRFork2 : public Sensor {
private:
    int priorVal = HIGH;
    int count = 0;

public:
  void setup() override {
  
  }

    void prepare() override {
      drawHeader("IR Fork - Counts");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
    }
  
  void updateDisplay() override {
    uint16_t readings[12];
    int sensorVal = digitalRead(14);

    if (sensorVal != priorVal) {
      if (sensorVal == LOW) {
        count = count + 1;
        char countStr[16];
        snprintf(countStr, sizeof(countStr), "%d", count);
        drawCenterText(countStr);
        Serial.println(countStr);
      }
      priorVal = sensorVal;
    }
  }
};

class IRFork3 : public Sensor {
private:
    int priorVal = HIGH;
    unsigned long lastDetectionTime = 0;
    unsigned long currentDetectionTime = 0;

public:
    void setup() override {
        // Initialization code here, if needed
    }

    void prepare() override {
        drawHeader("IR Fork - Timer");
        tft.fillRect(80, 50, 480, 320, TFT_BLACK);
    }

    void updateDisplay() override {
        int sensorVal = digitalRead(14);

        if (sensorVal != priorVal) {
            if (sensorVal == LOW) {
                currentDetectionTime = millis();

                // Calculate time elapsed since last detection
                if (lastDetectionTime != 0) {
                    unsigned long timeElapsed = currentDetectionTime - lastDetectionTime;
                    char timeElapsedStr[32];
                    snprintf(timeElapsedStr, sizeof(timeElapsedStr), "%lu ms", timeElapsed);
                    drawCenterText(timeElapsedStr);
                    Serial.println(timeElapsedStr);
                }

                // Update the last detection time
                lastDetectionTime = currentDetectionTime;
            }
            priorVal = sensorVal;
        }
    }

    // Additional helper functions (like drawCenterText, drawHeader) go here
};

// MLX90640 Thermal Camera
class ThermalSensor : public Sensor {
private:
    // ... [Properties for MLX90640 here]
  Adafruit_MLX90640 mlx; // MLX905040 instance
  float frame[32 * 24];  
  static const uint16_t camColors[256];
  const int MINTEMP = 20;
  const int MAXTEMP = 35;
  const int DISPLAY_START_X = 80;
  const int DISPLAY_START_Y = 50;

public:
  void setup() override {
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire1)) {
        Serial.println("MLX90640 failed to initialize!");
    } else {
        Serial.println("MLX90640 initialized!");
    }

    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_18BIT);
    mlx.setRefreshRate(MLX90640_8_HZ);
  }

    void prepare() override {
      drawHeader("Thermal Camera");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
      Wire1.setClock(1000000); // 1MHz clock speed
    }
  
  void updateDisplay() override {
    if (mlx.getFrame(frame) != 0) {
      Serial.println("Camera Failed");
      return;
    }

    for (uint8_t h = 0; h < 24; h++) {
        for (uint8_t w = 0; w < 32; w++) {
            float t = frame[h * 32 + w];
            t = constrain(t, MINTEMP, MAXTEMP);
            uint8_t colorIndex = map(t, MINTEMP, MAXTEMP, 0, 255);
            tft.fillRect(DISPLAY_START_X + displayPixelWidth * w, DISPLAY_START_Y + displayPixelHeight * h,
                        displayPixelWidth, displayPixelHeight, camColors[colorIndex]);
        }    
    }
  }
};

// Spectral Sensor
class SpectralSensor : public Sensor {
private:
    Adafruit_AS7341 as7341;
    int priorVal = HIGH;
    bool stateLED = false;
    unsigned long previousMillis = 0;  // will store last time screen was updated
    const long interval = 500;  // interval at which to update screen (milliseconds)

public:
  void setup() override {
    if (!as7341.begin(AS7341_I2CADDR_DEFAULT,&Wire1)){
      Serial.println("Could not find AS7341");
    } else {Serial.println("AS7341 Connected!");
      }
      as7341.setATIME(100);
      as7341.setASTEP(100);
      as7341.setGain(AS7341_GAIN_256X);
      as7341.setLEDCurrent(4); // 4mA // set this to turn on/off based on switch later!
  }

    void prepare() override {
      drawHeader("Spectrometer");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
      Wire1.setClock(400000); // 1MHz clock speed
      as7341.enableLED(false);

      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(4);
      tft.setCursor(120, 90);
        tft.print("445nm: ");

        tft.setCursor(120, 130);
        tft.print("555nm: ");

        tft.setCursor(120, 170);
        tft.print("680nm: ");

        tft.setCursor(120, 210);
    }
  
  void updateDisplay() override {
    uint16_t readings[12];
    int sensorVal = digitalRead(14);
    unsigned long currentMillis = millis();

    if (sensorVal != priorVal) {
      if (sensorVal == LOW) {
        stateLED = !stateLED;
        as7341.enableLED(stateLED);
      }
      priorVal = sensorVal;
    }
    
    if (!as7341.readAllChannels(readings)){
      Serial.println("Error reading all channels!");
      return;
    }
    
    int padding = tft.textWidth("123nm: 12345");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextPadding(padding);
    tft.setTextSize(4);
    
    // for (int i = 0; i <= 11; i++) {
    //     tft.setCursor(160, i * 25 + 30);
    //     tft.print(readings[i]);
    // }

    if (currentMillis - previousMillis >= interval) {
      // save the last time you updated the screen
      previousMillis = currentMillis;
        //tft.fillRect(120+tft.textWidth("123 nm:"), 90, tft.textWidth("12345"), 210, TFT_BLACK);

        


        tft.setCursor(120+tft.textWidth("123 nm:"), 90);
        tft.print("     "); //blue
        tft.setCursor(120+tft.textWidth("123 nm:"), 90);
        tft.print(readings[1]); //blue
        tft.setCursor(120+tft.textWidth("123 nm:"), 130);
        tft.print("     "); // green
        tft.setCursor(120+tft.textWidth("123 nm:"), 130);
        tft.print(readings[4]); // green
        tft.setCursor(120+tft.textWidth("123 nm:"), 170);
        tft.print("     "); //red
        tft.setCursor(120+tft.textWidth("123 nm:"), 170);
        tft.print(readings[7]); //red

        Serial.print("445nm: ");
        Serial.print(readings[1]); // F3 Blue
        Serial.print(", ");
        Serial.print("555nm: ");
        Serial.print(readings[4]); // F7 red
        Serial.print(", ");
        Serial.print("680nm: ");
        Serial.print(readings[7]); // F6 orange
        Serial.print(";");
        Serial.println();
    }
  }
};

// PAJ Sensor
class GestureSensor : public Sensor {
private:
  DFRobot_PAJ7620U2 paj;
  unsigned long lastGestureTime = 0; // Timestamp to track last gesture
public:
    void setup() override {
      delay(300);
      if(paj.begin() != 0){
        Serial.println("PAJ7620U2 failed to initialize!");
      }
      else Serial.println("PAJ7620U2 initialized!");

      // Gesture mode will be entered once proximity mode senses something close
      paj.setGestureHighRate(true);
      
    }

    void prepare() override {
      drawHeader("Gesture Sensor");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
      Wire1.setClock(400000); // Default clock speed. Needs to be set b/c Thermal Camera is 1 MHz
      paj.begin();
    }

    void updateDisplay() override {
      
      DFRobot_PAJ7620U2::eGesture_t gesture = paj.getGesture();
      if(gesture != paj.eGestureNone){
        lastGestureTime = millis();  // Update the timestamp
        switch (gesture) {
          case paj.eGestureRight:
            drawCenterText("Right");
            Serial.println("Right");
            break;
          case paj.eGestureLeft: 
            drawCenterText("Left");
            Serial.println("Left");
            break;
          case paj.eGestureUp:
            drawCenterText("Up");
            Serial.println("Up");
            break;
          case paj.eGestureDown:
            drawCenterText("Down");
            Serial.println("Down");
            break;
          case paj.eGestureForward:
            drawCenterText("Forward");
            Serial.println("Forward");
            break;
          case paj.eGestureBackward:
            drawCenterText("Backward");
            Serial.println("Backward");
            break;
          case paj.eGestureClockwise:
            drawCenterText("CW");
            Serial.println("CW");
            break;
          case paj.eGestureAntiClockwise:
            drawCenterText("CCW");
            Serial.println("CCW");
            break;
        }
      }

      // Check if it's been more than 5 seconds since the last gesture
      if (millis() - lastGestureTime > 2000) {
        tft.fillRect(70, 50, 480, 320, TFT_BLACK); // Clear the gesture text
        lastGestureTime = millis();  // Reset the timestamp to prevent multiple clearings
      }
    }
};

// VL53L4CX ToF Sensor
class DistanceSensor : public Sensor {
private:
    VL53L4CX sensor_vl53l4cx_sat;
    unsigned long previousMillis = 0;  // will store last time screen was updated
    const long interval = 100;  // interval at which to update screen (milliseconds)


public:
    void setup() override {
        // Configure VL53L4CX satellite component
        sensor_vl53l4cx_sat.setI2cDevice(&Wire1);
        sensor_vl53l4cx_sat.begin();

        // Initialize VL53L4CX satellite component
        VL53L4CX_Error error = sensor_vl53l4cx_sat.InitSensor(VL53L4CX_DEFAULT_DEVICE_ADDRESS);
        // if (error != VL53L4CX_ERROR_NONE) {
        //     Serial.print("Error Initializing Sensor: ");
        //     Serial.println(error);
        //     while (true) delay(10);
        // }

        // Start Measurements
        sensor_vl53l4cx_sat.VL53L4CX_StartMeasurement();
    }

    void prepare() override {
      drawHeader("Time of Flight Distance");
      tft.fillRect(80, 50, 480, 320, TFT_BLACK);
      Wire1.setClock(400000); // Default clock speed. Needs to be set b/c Thermal Camera is 1 MHz
    }

    void updateDisplay() override {
      unsigned long currentMillis = millis();
      VL53L4CX_MultiRangingData_t MultiRangingData;
      uint8_t NewDataReady = 0;
      int status;

    
    do {
      status = sensor_vl53l4cx_sat.VL53L4CX_GetMeasurementDataReady(&NewDataReady);
    } while (!NewDataReady);

    if ((!status) && (NewDataReady != 0)) {
      status = sensor_vl53l4cx_sat.VL53L4CX_GetMultiRangingData(&MultiRangingData);
      int no_of_object_found = MultiRangingData.NumberOfObjectsFound;

      for (int j = 0; j < no_of_object_found; j++) {
        if (MultiRangingData.RangeData[j].RangeStatus == VL53L4CX_RANGESTATUS_RANGE_VALID ||
            MultiRangingData.RangeData[j].RangeStatus == VL53L4CX_RANGESTATUS_RANGE_VALID_MERGED_PULSE) {
          char distanceStr[16];
          snprintf(distanceStr, sizeof(distanceStr), "%d mm", MultiRangingData.RangeData[j].RangeMilliMeter);
          
          if (currentMillis - previousMillis >= interval) {
            // save the last time you updated the screen
            previousMillis = currentMillis;
            drawCenterText(distanceStr);
            Serial.println(distanceStr);
          }
        }
      }

      if (status == 0) {
        status = sensor_vl53l4cx_sat.VL53L4CX_ClearInterruptAndStartMeasurement();
      }
    }
  
    }
};

// Array of available sensors
Sensor* sensors[NUM_POSITIONS] = {
    new IRFork1(),
    new IRFork2(),
    new IRFork3(),
    new GestureSensor(),
    new DistanceSensor(),
    new SpectralSensor(),
    new ThermalSensor(),
    // new ForkSensor1(),
    // new ForkSensor2(),
    // new ForkSensor3(),
    
    // ... [Add more sensors as needed]
};

const uint16_t ThermalSensor::camColors[256] = {0x480F,
  0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
  0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
  0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
  0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
  0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
  0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
  0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
  0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
  0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
  0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
  0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
  0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
  0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
  0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
  0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
  0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
  0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
  0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
  0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
  0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
  0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
  0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
  0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
  0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
  0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
  0xF080,0xF060,0xF040,0xF020,0xF800,}; // Color pallette

void setup() {
    Serial.begin(115200);
    initializeTFT();
    pinMode(14, INPUT_PULLUP);
    delay(300);
    for (int i = 0; i < NUM_POSITIONS; i++) {
        if (sensors[i]) {
            sensors[i]->setup();
        }
    }

    position = getSwitchPosition();
}

void loop() {
  uint32_t newPosition;
  // Check if there's new data from core1 about switch position
  if (rp2040.fifo.pop_nb(&newPosition)) {
      // Handle the new position
      updateSwitchDisplay(newPosition);
  }

  if (newPosition >= 1 && newPosition <= NUM_POSITIONS && sensors[newPosition - 1]) {
        sensors[newPosition - 1]->updateDisplay();
    }
}


void setup1() {
  delay(100);
}

void loop1()  {
    static int lastPosition = 0;
    int position = getSwitchPosition();

    if (position != lastPosition) {
        // Position changed, push the new position to core0
        while (!rp2040.fifo.push_nb((uint32_t)position)) {
            // Optionally, handle the situation where FIFO is full
            // For example, retry after a short delay
        }
        lastPosition = position;
    }

    // Add a short delay to prevent this loop from running too fast
    delay(10);
}


void initializeTFT() {
    displayPixelWidth = tft.width() / 32;
    displayPixelHeight = tft.width() / 32;
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    drawVerticalNumbers();
}

int getSwitchPosition() {
    int adcReading = analogRead(ADC_PIN);
    int closestPosition = 1;
    int minDifference = abs(adcReading - adcValues[0]);

    for (int i = 1; i < NUM_POSITIONS; i++) {
        int difference = abs(adcReading - adcValues[i]);
        if (difference < minDifference) {
            minDifference = difference;
            closestPosition = i + 1;
        }
    }
    return closestPosition;
}

void updateSwitchDisplay(int mode) {
    if (mode >= 1 && mode <= 10) {
        Serial.print("Mode ");
        Serial.println(mode);
        eraseArrow(currentPosition);
        
        if (sensors[mode - 1]) {
          sensors[mode - 1]->prepare();
        }
        drawArrow(mode);
        currentPosition = mode;
    }
}

void drawHeader(const char* string) {
    tft.fillRect(0, 0, 480, 30, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(string, tft.width() / 2, 5);
}

void drawVerticalNumbers() {
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    for (int i = 1; i <= 10; i++) {
        tft.setCursor(10, i * 30);
        tft.print(i);
    }
}

void drawArrow(int position) {
    int ypos = (position * 30) + 5;
    tft.fillTriangle(40, ypos, 50, ypos - 5, 50, ypos + 5, TFT_WHITE);
}

void eraseArrow(int position) {
    int ypos = (position * 30) + 5;
    tft.fillRect(34, ypos - 6, 22, 12, TFT_BLACK);
}

// Function to draw the text in the center of the TFT screen
void drawCenterText(const char *string) {
  int xpos = tft.width() / 2; // Center the text on X axis
  int ypos = tft.height() / 2; // Center the text on Y axis
  int padding = tft.textWidth("Forwards");
  tft.setTextDatum(TC_DATUM);  // Centre text on x,y position
  tft.setTextPadding(padding);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(8);
  tft.drawString(string, xpos, ypos);
}
