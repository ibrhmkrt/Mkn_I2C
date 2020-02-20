#include <avr/io.h>
#include <avr/interrupt.h>
#include<Wire.h>                          //Library for I2C Communication functions

#define Yon1 9 //Driver << pinine bağlı
#define Yon2 10 //Drive >> pinine bağlı
#define BitisTetik 12
int asagiLimit = 2;
int yukariLimit = 3;

byte SlaveReceived = 0;
byte MotorReady = 90;
byte MotorBusy = 91;
int ByteSayisi = 0;
byte MankenYon = 0;
boolean ByteHata = false;
byte ByteHataMesaj = 51;
byte RequestHata = 41;
int MotorCalisiyor = 0;
int MotorKomut = 0;
byte MotorCalismayaBasladi = 95;
byte MotorCalismaDurumunda = 96;
byte MotorAsagiCalismayaBasladi = 98;
int MotorKomut2 = 0;
byte MotorYukariCalismayaBasladi = 100;
int MotorKomut3 = 0;
byte SifirlamaBaslandi = 102;
int MotorKomut4 = 0;
//boolean BitisTetikleme = false;

void setup()

{
  Serial.begin(115200);                     //Begins Serial Communication at 9600 baud rate
  Wire.begin(2);                          //Begins I2C communication with Slave Address as 8 at pin (A4,A5)
  Wire.onReceive(receiveEvent);           //Function call when Slave receives value from master
  Wire.onRequest(requestEvent);           //Function call when Master request value from Slave
  pinMode(Yon1, OUTPUT);
  pinMode(Yon2, OUTPUT);
  pinMode(BitisTetik, OUTPUT);
  digitalWrite(Yon1, LOW);
  digitalWrite(Yon2, LOW);
  pinMode(asagiLimit, INPUT);
  attachInterrupt(0, inerkenDurdur, FALLING); //2. pinde bağlı limit sensör
  pinMode(yukariLimit, INPUT);
  attachInterrupt(1, kalkarkenDurdur, FALLING); // 3. pinde bağlı limit sensör

  TCNT1 = 64750; // 1 saniye 15625 için >> 65535-15625, 64750 ~50 ms
  TCCR1A = 0x00;//Normal mode
  TCCR1B &= (0 << CS11); // 1024 prescler
  TCCR1B |= (1 << CS10); // 1024 prescler
  TCCR1B |= (1 << CS12); // 1024 prescler
  TIMSK1 |= (1 << TOIE1); // Timer1 taşma kesmesi aktif

  sei(); //Global kesmeler devrede

}

void loop(void)
{
  if (MotorKomut == 1)
  {
    int EksenSonundami1 = digitalRead(asagiLimit);
    if (EksenSonundami1 == HIGH) {
      while (1) {
        MotorCalisiyor = 1;
        digitalWrite(Yon1, HIGH);
        int asagiLimitDurumu = digitalRead(asagiLimit);
        if (asagiLimitDurumu == LOW) {
          while (1) {
            digitalWrite(Yon2, HIGH);
            int yukariLimitDurumu = digitalRead(yukariLimit);
            if (yukariLimitDurumu == LOW) {
              break;
            }
          }
          MotorKomut = 0;
          break;
        }
      }
    }
    else {
      MotorKomut = 0;
    }
  }
  else if (MotorCalisiyor == 1 && MotorKomut == 0 ) {
    MotorCalisiyor = 0;
  }



  if (MotorKomut2 == 1)
  {
    int EksenSonundami2 = digitalRead(asagiLimit);
    if (EksenSonundami2 == HIGH) {
      while (1) {
        MotorCalisiyor = 1;
        digitalWrite(Yon1, HIGH);
        int asagiLimitDurumu = digitalRead(asagiLimit);
        if (asagiLimitDurumu == LOW) {
          MotorKomut2 = 0;
          break;
        }
      }
    }
    else {
      MotorKomut2 = 0;
    }
  }
  else if (MotorCalisiyor == 1 && MotorKomut2 == 0 ) {
    MotorCalisiyor = 0;
  }



  if (MotorKomut3 == 1)
  {
    int EksenSonundami3 = digitalRead(yukariLimit);
    if (EksenSonundami3 == HIGH) {
      while (1) {
        MotorCalisiyor = 1;
        digitalWrite(Yon2, HIGH);
        int yukariLimitDurumu = digitalRead(yukariLimit);
        if (yukariLimitDurumu == LOW) {
          MotorKomut3 = 0;
          break;
        }
      }
    }
    else {
      MotorKomut3 = 0;
    }
  }
  else if (MotorCalisiyor == 1 && MotorKomut3 == 0 ) {
    MotorCalisiyor = 0;
  }


  if (MotorKomut4 == 1) {
    int EksenSonundami4 = digitalRead(yukariLimit);
    if (EksenSonundami4 == HIGH) {
      while (1) {
        MotorCalisiyor = 1;
        digitalWrite(Yon2, HIGH);
        int yukariLimitDurumu = digitalRead(yukariLimit);
        if (yukariLimitDurumu == LOW) {
          MotorKomut4 = 0;
          break;
        }
      }
    }
    else {
      MotorKomut4 = 0;
    }
  }
  else if (MotorCalisiyor == 1 && MotorKomut4 == 0 ) {
    MotorCalisiyor = 0;
  }


  /*if (BitisTetikleme == true) {
    digitalWrite(BitisTetik, HIGH);
    delay(50);
    digitalWrite(BitisTetik, LOW);

    BitisTetikleme = false;
    }*/


}

void receiveEvent (int howMany)                    //This Function is called when Slave receives value from master
{
  ByteSayisi = howMany;
  if (ByteSayisi <= 2) {
    if (ByteSayisi == 1) {
      SlaveReceived = Wire.read(); //Used to read value received from master and store in variable SlaveReceived
    }
    else if (ByteSayisi == 2) {
      SlaveReceived = Wire.read(); //Used to read value received from master and store in variable SlaveReceived
      MankenYon = Wire.read();
    }
    else {
      ByteHata = true;
      while (Wire.available()) {
        Wire.read();
      }
    }
  }
  else  {
    ByteHata = true;
    while (Wire.available()) {
      Wire.read();
    }
  }
}

void requestEvent()                                //This Function is called when Master wants value from slave
{

  if (ByteHata) {
    ByteHata = false;
    Wire.write(ByteHataMesaj);//51
  }
  else if (SlaveReceived == 80 ) {
    if (MotorCalisiyor == 1) {
      Wire.write(MotorBusy);//Motor Meşgul 91
    }
    else {
      Wire.write(MotorReady);//Motor Müsait 90
    }
  }
  else if (SlaveReceived == 81 && MotorCalisiyor == 0) {
    MotorKomut = 1;
    Wire.write(MotorCalismayaBasladi);//Motor Calismaya Başladı Durumu 95
  }
  else if (SlaveReceived == 81 && MotorCalisiyor == 1) {
    Wire.write(MotorCalismaDurumunda);//Motor Calisma Durumunda 96
  }
  else if (SlaveReceived == 82 && MankenYon == 1 && MotorCalisiyor == 0 ) {
    MotorKomut2 = 1;
    //MankenYon = 0;
    Wire.write(MotorAsagiCalismayaBasladi);// Motor Asagi Calismaya Başladı Durumu 98
  }
  else if (SlaveReceived == 82 && MankenYon == 1 && MotorCalisiyor == 1) {
    MankenYon = 0;
    Wire.write(MotorCalismaDurumunda);//Motor Calisma Durumunda 96
  }
  else if (SlaveReceived == 82 && MankenYon == 2 && MotorCalisiyor == 0 ) {
    MotorKomut3 = 1;
    //MankenYon = 0;
    Wire.write(MotorYukariCalismayaBasladi);// Motor Yukari Calismaya Başladı Durumu 100
  }
  else if (SlaveReceived == 82 && MankenYon == 2 && MotorCalisiyor == 1) {
    MankenYon = 0;
    Wire.write(MotorCalismaDurumunda);//Motor Calisma Durumunda 96
  }
  else if (SlaveReceived == 84 && MotorCalisiyor == 0 ) {
    MotorKomut4 = 1;
    Wire.write(SifirlamaBaslandi);//Motor sıfırlamaya başlıyor 102
  }
  else if (SlaveReceived == 84 && MotorCalisiyor == 1 ) {
    Wire.write(MotorCalismaDurumunda);//Motor Calisma Durumunda 96
  }
  else {
    Wire.write(RequestHata);//41
  }
}

void inerkenDurdur() {
  digitalWrite(Yon1, LOW);
}
void kalkarkenDurdur() {
  digitalWrite(Yon2, LOW);
  //BitisTetikleme = true;
  digitalWrite(BitisTetik,HIGH);
  TCNT1 = 64750;// 65535-64750(1024 prescaler)=~50 ms --- 1 sn 15625
  TIFR1 |= (1 << TOV1) ;//timer1 taşma bayragı sıfırlanır.
  TIMSK1 |= (1 << TOIE1) ;// Timer1 taşma kesmesi aktif
}
ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
  TIMSK1 &= (0 << TOIE1) ;   // Timer1 taşma kesmesi devredışı (TOIE1)
  digitalWrite(BitisTetik, LOW);      //Merkez tetikleme pin deaktif
}
