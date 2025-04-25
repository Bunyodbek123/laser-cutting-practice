#include <Servo.h>

// Пины подключения датчика цвета и сервоприводов
const int S2 = 4;
const int S3 = 5;
const int sensorOut = 6;

const int conveyorMotorPin = 7;

Servo servo1;
Servo servo2;

// Переменные для хранения частот цвета
int redFrequency, greenFrequency, blueFrequency, clearFrequency;

// Пороговые значения для калибровки
int redMin = 10000, redMax = 0;
int greenMin = 10000, greenMax = 0;
int blueMin = 10000, blueMax = 0;

// Значения RGB после нормализации
int redValue, greenValue, blueValue;

// Определённый цвет
String detectedColor = "Unknown";

void setup() {
  Serial.begin(9600);

  // Настройка пинов
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(conveyorMotorPin, OUTPUT);

  // Подключение сервоприводов
  servo1.attach(8);
  servo2.attach(9);

  // Инициализация в нейтральной позиции
  servo1.write(90);
  servo2.write(90);

  // Калибровка датчика
  calibrateSensor();

  // Запуск конвейера
  startConveyor();
}

void loop() {
  readColorFrequencies();  // Считывание частот
  detectColor();           // Определение цвета
  classifyObject();        // Управление в зависимости от цвета
  delay(100);              // Пауза
}

// Считывание частот с датчика цвета
void readColorFrequencies() {
  digitalWrite(S2, LOW); digitalWrite(S3, LOW);   // Красный
  redFrequency = pulseIn(sensorOut, LOW);

  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH); // Зелёный
  greenFrequency = pulseIn(sensorOut, LOW);

  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);  // Синий
  blueFrequency = pulseIn(sensorOut, LOW);

  digitalWrite(S2, HIGH); digitalWrite(S3, LOW);  // Прозрачный
  clearFrequency = pulseIn(sensorOut, LOW);

  // Отладка
  Serial.print("R: "); Serial.print(redFrequency);
  Serial.print(" G: "); Serial.print(greenFrequency);
  Serial.print(" B: "); Serial.print(blueFrequency);
  Serial.print(" C: "); Serial.println(clearFrequency);
}

// Калибровка датчика
void calibrateSensor() {
  Serial.println("Калибровка сенсора...");

  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    readColorFrequencies();
    redMin = min(redMin, redFrequency); redMax = max(redMax, redFrequency);
    greenMin = min(greenMin, greenFrequency); greenMax = max(greenMax, greenFrequency);
    blueMin = min(blueMin, blueFrequency); blueMax = max(blueMax, blueFrequency);
  }

  Serial.println("Калибровка завершена:");
  Serial.print("Red: "); Serial.print(redMin); Serial.print("-"); Serial.println(redMax);
  Serial.print("Green: "); Serial.print(greenMin); Serial.print("-"); Serial.println(greenMax);
  Serial.print("Blue: "); Serial.print(blueMin); Serial.print("-"); Serial.println(blueMax);
}

// Определение цвета по нормализованным значениям
void detectColor() {
  redValue = map(redFrequency, redMin, redMax, 0, 255);
  greenValue = map(greenFrequency, greenMin, greenMax, 0, 255);
  blueValue = map(blueFrequency, blueMin, blueMax, 0, 255);

  if (redValue > greenValue && redValue > blueValue)
    detectedColor = "Red";
  else if (greenValue > redValue && greenValue > blueValue)
    detectedColor = "Green";
  else if (blueValue > redValue && blueValue > greenValue)
    detectedColor = "Blue";
  else
    detectedColor = "Unknown";

  Serial.print("RGB: ");
  Serial.print(redValue); Serial.print(" ");
  Serial.print(greenValue); Serial.print(" ");
  Serial.print(blueValue); Serial.print(" - ");
  Serial.println(detectedColor);
}

// Классификация объекта и управление сервоприводами
void classifyObject() {
  if (detectedColor == "Red") {
    servo1.write(0);
    servo2.write(90);
  } else if (detectedColor == "Green") {
    servo1.write(180);
    servo2.write(90);
  } else if (detectedColor == "Blue") {
    servo1.write(90);
    servo2.write(0);
  } else {
    servo1.write(90);
    servo2.write(90); // Нейтральная позиция
  }
}

// Управление лентой
void startConveyor() {
  digitalWrite(conveyorMotorPin, HIGH);
  Serial.println("Конвейер запущен");
}

void stopConveyor() {
  digitalWrite(conveyorMotorPin, LOW);
  Serial.println("Конвейер остановлен");
}
