#include <SPI.h>         // SPI library
#include <MFRC522.h>     // RFID reader library
#include <QTRSensors.h>  // QTR Sensor Library

QTRSensors qtr;

const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

//Ultrosonic Sensor
const int trigPin = A4;
const int echoPin = A5;
unsigned long delay_time;
bool timmy = false;

float duration, distance;

//PID
float output;
unsigned long p_time;
bool turn = LOW;
unsigned long prev_time;

//P
bool state_ahead = HIGH;
bool state_back = LOW;
bool state_ahead_2 = HIGH;
bool state_back_2 = LOW;
int target_val = 1500;
int actual_val;
float error;
float p_error;
float p;
float Kp_scale = 0.1;
float m1, m2;
//I
unsigned long dT;
float i;
float Ki_scale = 0.025;
//D
float d;
float Kd_scale = 0.25;

//MPU6050
const int MPU = 0x68;
float GyroZ, accGyroZ, GyroErrorZ;
float elapsedTime_G, currentTime_G, previousTime_G;
unsigned long previousTime_Q;
float yaw, yaw_test;
int c = 0;

//RFID
const int SS_PIN = 10;
const int RST_PIN = 9;
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
int block = 2;
byte blockcontent[16] = { "Last-Minute-Engg" };
byte readbackblock[18];

//Write specific block
int writeBlock(int blockNumber, byte arrayAddress[]) {
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;  //determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber + 1) % 4 == 0) {
    Serial.print(blockNumber);
    Serial.println(" is a trailer block:");
    return 2;
  }
  Serial.print(blockNumber);
  Serial.println(" is a data block:");

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3;  //return "3" as error message
  }

  //writing the block
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;  //return "4" as error message
  }
  Serial.println("block was written");
}

//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]) {
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;  //determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3;  //return "3" as error message
  }

  //reading a block
  byte buffersize = 18;                                                  //we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size...
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);  //&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;  //return "4" as error message
  }
  Serial.println("block was read");
}

//Left Motor
const int In1 = 7;
const int In2 = 8;
const int EnA = 6;

//Right Motor
const int In3 = 2;
const int In4 = 4;
const int EnB = 5;

unsigned long motor_delay;
unsigned long pm_time;

int delay_motor;


void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ A0, A1, A2, A3 }, SensorCount);

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }

  //Motor Left
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(EnA, OUTPUT);

  //Motor Right
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);
  pinMode(EnB, OUTPUT);
}

void loop() {
  analogArray();
  motor();
  distance_timmy();

  if (distance < 30){
    timmy = true;
    if (millis() - pm_time > 10) {
      pm_time = millis();
      analogWrite(EnA, 100);
      analogWrite(EnB, 100);

      digitalWrite(In1, state_ahead);
      digitalWrite(In2, state_back);
      digitalWrite(In3, state_ahead_2);
      digitalWrite(In4, state_back_2);
    }

    if (millis() - pm_time > 20) {
      pm_time = millis();
      digitalWrite(In1, LOW);
      digitalWrite(In2, LOW);
      digitalWrite(In3, LOW);
      digitalWrite(In4, LOW);
    }
  }
  if (timmy == false){
    PID();
  }   
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println("card selected");

  //the blockcontent array is written into the card block
  writeBlock(block, blockcontent);

  //read the block back
  readBlock(block, readbackblock);

  //print the block contents
  Serial.print("read block: ");
  for (int j = 0; j < 16; j++) {
    Serial.write(readbackblock[j]);
  }
  Serial.println("");
}

void distance_timmy() {
  if(millis() - delay_time > 100){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration*.0343)/2;
    Serial.print("Distance: ");
    Serial.println(distance);
    delay_time = millis();
  }
}

void analogArray() {
  // line position
  uint16_t position = qtr.readLineBlack(sensorValues);
  actual_val = position;
}

void PID() {
  p_error = error;

  error = target_val - actual_val;
  error = map(error, -1500, 1500, -10, 10);
  dT = millis() - prev_time;
  p = Kp_scale * error;
  i = Ki_scale * (error * dT) + i;
  i = constrain(i, -15, 15);
  d = Kd_scale * ((error - p_error) / (dT));

  output = p + i + d;

  if(error < -4){
    m1 = 110;
    state_ahead = LOW;
    state_back = HIGH;
    m2 = 110;
  }
  else if(error > 4){
    m1 = 110;
    m2 = 110;
    state_ahead_2 = LOW;
    state_back_2 = HIGH;
  }
  else{
    m1 = 110 + output;
    m2 = 130 - output;
    state_ahead = HIGH;
    state_back = LOW;
    state_ahead_2 = HIGH;
    state_back_2 = LOW;
  }
  prev_time = millis();
}

void motor() {
  if (millis() - pm_time > 10) {
    pm_time = millis();
    analogWrite(EnA, m1);
    analogWrite(EnB, m2);

    digitalWrite(In1, state_ahead);
    digitalWrite(In2, state_back);
    digitalWrite(In3, state_ahead_2);
    digitalWrite(In4, state_back_2);
  }

  if (millis() - pm_time > 20) {
    pm_time = millis();
    digitalWrite(In1, LOW);
    digitalWrite(In2, LOW);
    digitalWrite(In3, LOW);
    digitalWrite(In4, LOW);
  }
}
