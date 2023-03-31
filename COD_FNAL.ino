#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include <LiquidCrystal_I2C.h> //inclui display 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); //Define o endereco I2C do display e qtde de colunas e linhas

#define pinsensor1 14 // Entrada
#define pinsensor2 27 // Saida
#define cap_max 250 //definicção da capacidade máxima 
#define BUZZER_PIN 18 //define o pino 18 para acionar o buzzer 
#define bomba 5
#define trava1 17
#define trava2 16

//Array que desenha o simbolo de grau
byte grau[8] = {B00110, B01001, B01001, B00110,
                B00000, B00000, B00000, B00000,
               };

int cap_atual = 0;
double temp_amb;
double temp_obj;
double temp_REF = 35;

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor de temperatura MLX90614");

  //Inicializa o display LCD I2C
  lcd.init();  // inicializa o display
  lcd.backlight();  // liga a iluminação do display
  lcd.clear();  // limpa o display

  //Sensor de presença
  pinMode(pinsensor1, INPUT);
  pinMode(pinsensor2, INPUT);
  //buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  //relés
  pinMode(bomba, OUTPUT);
  pinMode(trava1, OUTPUT);
  pinMode(trava2, OUTPUT);
  //Inicializa o MLX90614
  mlx.begin();
  // desliga os relés inicialmente
  digitalWrite(bomba, HIGH);
  digitalWrite(trava1, HIGH);
  digitalWrite(trava2, HIGH);
}

void loop() {
  bool estSensor1 = digitalRead(pinsensor1);
  lcd.clear(); // limpa o display
  lcd.setCursor(0, 0);
  lcd.print("AGUARDANDO...");
  lcd.setCursor(0, 1);
  lcd.print("DENTRO:");
  lcd.print(cap_atual);

  if (estSensor1 == LOW) {
    unsigned long startTime = millis();
    if (cap_atual < cap_max) {
      lcd.clear(); // limpa o display
      lcd.setCursor(0, 0);
      lcd.print("LENDO A TEMP!");

      while (millis() - startTime < 6000) {
        temp_obj = mlx.readObjectTempC();
        delay(800);
      }

      Serial.print("*CtObjeto = ");
      Serial.print(temp_obj);
      Serial.println("*C");

      if (temp_obj <= temp_REF) {
        cap_atual = cap_atual + 1;

        lcd.clear(); // limpa o display
        lcd.setCursor(0, 0);
        lcd.print("TEMP:");
        lcd.print(temp_obj);
        lcd.setCursor(0, 1);
        lcd.print("ENTRE!");

        digitalWrite(bomba, LOW);
        digitalWrite(trava1, LOW);
        delay(3000);
        digitalWrite(bomba, HIGH);
        digitalWrite(trava1, HIGH);

        Serial.println("Entrada");
        delay(2000);


      }
      else if (temp_obj > temp_REF) {
        unsigned long buzzTime = millis();

        lcd.clear(); // limpa o display
        lcd.setCursor(0, 0);
        lcd.print("TEMP ALTA:");
        lcd.print(temp_obj);
        lcd.setCursor(0, 1);
        lcd.print("ENTRADA BLOQ!");

        while (millis() - buzzTime < 3200) {
          digitalWrite(BUZZER_PIN, HIGH); // turn on the buzzer
          delay(300); // wait for 1 second
          digitalWrite(BUZZER_PIN, LOW); // turn off the buzzer
          delay(300); // wait for 1 second
        }
        Serial.println("Entrada Bloqueada");
        delay(2000);

      }
    }
    else if ((cap_atual == cap_max)) {
      lcd.clear(); // limpa o display
      lcd.setCursor(0, 0);
      lcd.print("OCUPACAO MAX!");
      lcd.setCursor(0, 1);
      lcd.print("ENTRADA BLOQ!");
      Serial.println("Ocupação MAX");
      delay(2000);
    }
  }


  bool estSensor2 = digitalRead(pinsensor2);
  if (estSensor2 == LOW) {
    cap_atual = cap_atual - 1;
    if (cap_atual < 0) {
      cap_atual = 0;
    }
    lcd.clear(); // limpa o display
    lcd.setCursor(0, 0);
    lcd.print("SAIDA...");
    lcd.setCursor(0, 1);
    lcd.print("DENTRO:");
    lcd.print(cap_atual);
    delay(2000);
    digitalWrite(trava2, LOW);
    delay(3000);
    digitalWrite(trava2, HIGH);
    Serial.println("Saída");
  }

  if (cap_atual < 0) {
    cap_atual = 0;
  }
  else if (cap_atual > cap_max) {
    cap_atual = cap_max;

  }
  Serial.println(cap_atual);
  delay(500);

}
