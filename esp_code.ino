#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#define I2CAddressESPWifi 8

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Variabile
const char* ssid = "Mireasa pentru wifi-ul meu";
const char* password = "66402077";
String key = "br4kF-uwj7P4cVdBzryYn8";
String event_name = "trimitere_date"; 
String value1 = "";
String value2 = "";
String value3 = "";

const int pin_meniu = 0;
const int pin_ok = 2;
const int pin_salvare = 14;
const int pin_sus = 12;
const int pin_jos = 13;
int buton_meniu;
int buton_ok;
int buton_salvare;
int buton_sus;
int buton_jos;

int ora_aux;
int minut_aux;
int zi_aux;
int luna_aux;
String ora;
String minut;
String zi;
String luna;

int optiune = 0;
int pe_rand = 0;
int pe_ultimul = 0;

String vector_puls[5];
String vector_timp[5];
int poz = 0;
int poz_viz = 0;
int poz_aux = 0;

String timp_actual = "";
String IncomingString = "";
int BPM;

bool meniu_principal=0;
bool meniu_masurare=0;
bool masurare=0;
bool salvare=0;
bool meniu_vizualizare=0;
bool meniu_internet=0;
bool meniu_bluetooth=0; 

int timezone = 3;
int dst = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(2,0);
    lcd.print("Conectare la");
    lcd.setCursor(0,1);
    lcd.print(ssid);
    delay(1000);
    lcd.clear();
  }
  lcd.setCursor(0,0);
  lcd.print("Conexiune");
  lcd.setCursor(6,1);
  lcd.print("realizata!");
  delay(3000);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    delay(1000);
  }
  lcd.clear();

  Wire.begin(4,5);
  
  pinMode(pin_meniu, INPUT_PULLUP);
  pinMode(pin_ok, INPUT_PULLUP);
  pinMode(pin_salvare, INPUT_PULLUP);
  pinMode(pin_sus, INPUT_PULLUP);
  pinMode(pin_jos, INPUT_PULLUP);

}

void loop() {
  buton_meniu = digitalRead(pin_meniu);
  buton_ok = digitalRead(pin_ok);
  buton_salvare = digitalRead(pin_salvare);
  buton_sus = digitalRead(pin_sus);
  buton_jos = digitalRead(pin_jos);
  
  if ( meniu_principal == 0 && meniu_masurare == 0 && meniu_vizualizare == 0 && meniu_bluetooth == 0) {
    lcd.setCursor(3, 0);
    lcd.print("Bun venit!");
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);

    ora_aux = (p_tm->tm_hour);
    minut_aux = (p_tm->tm_min);
    zi_aux = (p_tm->tm_mday);
    luna_aux = (p_tm->tm_mon + 1);
    //delay(300);
    
    ora = String(ora_aux);
    minut = String(minut_aux);
    zi = String(zi_aux);
    luna = String(luna_aux);
     
    if (ora_aux < 10) {    
      ora = '0' + ora;
    }
    if (minut_aux < 10) {    
      minut = '0' + minut;
    }
    if (zi_aux < 10) {    
      zi = '0' + zi;
    }
    if (luna_aux < 10) {    
      luna = '0' + luna;
    }
    //delay(300);
    lcd.setCursor(1, 1);
    lcd.print(ora);
    lcd.print(":");
    lcd.print(minut);
  
    lcd.setCursor(10,1);
    lcd.print(zi);
    lcd.print("/");
    lcd.print(luna);
  }
  
  if (buton_meniu == LOW) {
    //Serial.println("BUTON MENIU APASAT!!!!");
    lcd.clear();
    delay(200);
    meniu_principal=1;
  }
  if (meniu_principal == 1) {
    if ( optiune == 0) {
      lcd.setCursor(0,0);
      lcd.print("1. Masurare");
      lcd.setCursor(3,1);
      lcd.print("puls.");
      
      if (buton_ok == LOW) {
        //Serial.println("URMEAZA SA INTRAM IN MENIUL DE MASURARE PULS!!!!");
        lcd.clear();
        meniu_masurare = 1;
        meniu_principal = 0;
        optiune = 0;
        buton_ok = HIGH;
        delay(200);
      }

      if (buton_jos == LOW) {
        //Serial.println("SE VA AFISA A DOUA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 1;
        buton_jos = HIGH;
        delay(200);
      }

      if (buton_sus == LOW) {
        //Serial.println("SE VA AFISA ULTIMA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 4;
        buton_sus = HIGH;
        delay(200);
      }
    }
    if (optiune == 1) {
      lcd.setCursor(0,0);
      lcd.print("2. Vizualizare");
      lcd.setCursor(3,1);
      lcd.print("date salvate.");

      if (buton_ok == LOW) {
        //Serial.println("URMEAZA SA INTRAM IN MENIUL DE VIZUALIZARE DATE!!!!");
        lcd.clear();
        meniu_vizualizare = 1;
        meniu_principal = 0;
        optiune = 0;
        buton_ok = HIGH;
        delay(200);
      }
      
      if (buton_jos == LOW) {
        //Serial.println("SE VA AFISA A TREIA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 2;
        buton_jos = HIGH;
        delay(200);
      }

      if (buton_sus == LOW) {
        //Serial.println("SE VA AFISA PRIMA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 0;
        buton_sus = HIGH;
        delay(200);
      }
    }
    if (optiune == 2) {
      lcd.setCursor(0,0);
      lcd.print("3. Transmitere");
      lcd.setCursor(3,1);
      lcd.print("in internet.");

      if (buton_ok == LOW) {
        //Serial.println("URMEAZA SA INTRAM IN MENIUL DE TRANSMITERE DATE IN INTERNET!!!!");
        lcd.clear();
        meniu_internet = 1;
        meniu_principal = 0;
        optiune = 0;
        buton_ok = HIGH;
        delay(200);
      }
      
      if (buton_jos == LOW) {
        //Serial.println("SE VA AFISA A PATRA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 3;
        buton_jos = HIGH;
        delay(200);
      }

      if (buton_sus == LOW) {
        //Serial.println("SE VA AFISA A DOUA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 1;
        buton_sus = HIGH;
        delay(200);
      }
    }

    if (optiune == 3) {
      lcd.setCursor(0,0);
      lcd.print("4. Transmitere");
      lcd.setCursor(3,1);
      lcd.print("prin BT.");

      if (buton_ok == LOW) {
        //Serial.println("URMEAZA SA INTRAM IN MENIUL DE TRANSMITERE DATE PRIN BT!!!!");
        lcd.clear();
        meniu_bluetooth = 1;
        meniu_principal = 0;
        optiune = 0;
        buton_ok = HIGH;
        delay(200);
      }
      
      if (buton_jos == LOW) {
        //Serial.println("SE VA AFISA A CINCEA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 4;
        buton_jos = HIGH;
        delay(200);
      }

      if (buton_sus == LOW) {
        //Serial.println("SE VA AFISA A TREIA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 2;
        buton_sus = HIGH;
        delay(200);
      }
    }

    if (optiune == 4) {
      lcd.setCursor(0,0);
      lcd.print("5. Reveniti la");
      lcd.setCursor(0,1);
      lcd.print("meniul anterior.");

      if (buton_ok == LOW) {
        //Serial.println("REVENIM LA PRIMUL MENIU!!!");
        lcd.clear();
        meniu_masurare = 0;
        meniu_principal = 0;
        optiune = 0;
        buton_ok = HIGH;
        delay(200);
      }
      
      if (buton_jos == LOW) {
        //Serial.println("SE VA AFISA PRIMA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 0;
        buton_jos = HIGH;
        delay(200);
      }

      if (buton_sus == LOW) {
        //Serial.println("SE VA AFISA A PATRA OPTIUNE DIN MENIU!!!");
        lcd.clear();
        optiune = 3;
        buton_sus = HIGH;
        delay(200);
      }
    }
    
  }
  
  if (meniu_masurare == 1) {
    IncomingString="";
    if (buton_meniu == LOW) {
        //Serial.println("A FOST APASAT BUTONUL PENRU MENIUL PRINCIPAL!!!");
        lcd.clear();
        meniu_principal = 1;
        masurare = 0;
        salvare = 0;
        meniu_masurare = 0;
        delay(50);
    }
    if (meniu_masurare == 1 && masurare==0 ) {  
      lcd.setCursor(0, 0);
      lcd.print("Apasati ok cand");
      lcd.setCursor(2,1);
      lcd.print("sunteti gata!");
    }
    
    Wire.requestFrom(I2CAddressESPWifi,1);
    IncomingString = "";
    while (Wire.available()) {
      delay(1);
      IncomingString += Wire.read();
//      //Serial.print(c);
    }
    BPM=IncomingString.toInt();
    //Serial.println(IncomingString);
    delay(100);


    if ( buton_ok == LOW ) {
      //Serial.println("BUTON OK APASAT!!!!");
      lcd.clear();
      delay(200);
      masurare=1;
    }

    if (masurare == 1) {
        lcd.setCursor(1,0);
        lcd.print("Pulsul actual");
        lcd.setCursor(3,1);
        lcd.print("este: ");
        if (BPM < 100) {
          IncomingString = '0' + IncomingString;
        }
        //Serial.println(BPM);
        lcd.print(IncomingString);               
    }

    if ( buton_salvare == LOW ) {
      masurare = 0;
      //Serial.println("BUTON SALVARE APASAT!!!!");
      lcd.clear();
      delay(200);
      salvare=1;
    }

    if ( salvare == 1) {
      lcd.setCursor(1,0);
      lcd.print("Datele au fost");
      lcd.setCursor(4,1);
      lcd.print("salvate!");
      meniu_masurare = 0;
      salvare = 0;
      timp_actual = luna + zi + ora + minut; 
      vector_puls[poz]=String(BPM);
      vector_timp[poz]=timp_actual;
      poz = poz + 1;
      //Serial.println(vector_puls[poz-1]);
      //Serial.println(vector_timp[poz-1]);
      //Serial.println(poz);
      delay(4000);
      lcd.clear();

    }

  }

  if ( meniu_vizualizare == 1 ) {
    if ( poz == 0 ) {
      lcd.setCursor(1,0);
      lcd.print("Nu exista date");
      lcd.setCursor(1,1);
      lcd.print("inregistrate!");
      meniu_vizualizare= 0;
      delay(3000);
      lcd.clear();
    }
    else {
      lcd.setCursor(0,0);
      lcd.print(poz_viz+1);
      lcd.print(".");
      lcd.setCursor(3,0);
      lcd.print("Pulsul:");
      lcd.print(vector_puls[poz_viz]);
      lcd.setCursor(0,1);
      lcd.print("Timp:");
      lcd.print(vector_timp[poz_viz][4]);
      lcd.print(vector_timp[poz_viz][5]);
      lcd.print(":");
      lcd.print(vector_timp[poz_viz][6]);
      lcd.print(vector_timp[poz_viz][7]);
      lcd.print(" ");
      lcd.print(vector_timp[poz_viz][2]);
      lcd.print(vector_timp[poz_viz][3]);
      lcd.print("/");
      lcd.print(vector_timp[poz_viz][0]);
      lcd.print(vector_timp[poz_viz][1]);
      
      if (buton_meniu == LOW) {
          //Serial.println("A FOST APASAT BUTONUL PENRU MENIUL PRINCIPAL!!!");
          lcd.clear();
          meniu_principal = 1;
          meniu_vizualizare = 0;
          poz_viz = 0;
          delay(50);
      }
      if (buton_jos == LOW) {
          //Serial.println("SE VA AFISA URMATOAREA OPTIUNE DIN MENIU!!!");
          lcd.clear();
          if (poz_viz == (poz-1))
            poz_viz = 0;
          else
            poz_viz = poz_viz + 1;
          buton_jos = HIGH;
          delay(200);
        }
  
        if (buton_sus == LOW) {
          //Serial.println("SE VA AFISA ANTERIOARA OPTIUNE DIN MENIU!!!");
          lcd.clear();
          if (poz_viz == 0)
            poz_viz = (poz-1);
          else
            poz_viz = poz_viz - 1;
          buton_sus = HIGH;
          delay(200);
        }
    } 
  }

  if (meniu_internet == 1) {
    poz_aux = poz;
    if ( poz == 0 ) {
      lcd.setCursor(1,0);
      lcd.print("Nu exista date");
      lcd.setCursor(1,1);
      lcd.print("inregistrate!");
      meniu_internet = 0;
      delay(3000);
      lcd.clear();
    }
    else {     
      lcd.setCursor(1,0);
      lcd.print("Trimit datele");
      lcd.setCursor(3,1);
      lcd.print("pe mail...");
      //Serial.println("Poz_aux=");
      //Serial.println(poz_aux);
      delay(3000);
      if(poz_aux % 3 == 0){
        pe_rand = (poz_aux / 3);
      }
      else {
        pe_rand = (poz_aux / 3) + 1;
      }
      pe_ultimul = poz_aux - (2*pe_rand);
      //Serial.println("Pe rand vor fi: ");
      //Serial.println(pe_rand);
      //Serial.println("Pe ultimul rand vor fi: ");
      //Serial.println(pe_ultimul);
      int i = 0;
      int j = 0;
      value1="";
      value2="";
      value3="";

      if (poz_aux >= 3) {
        //Serial.println("Am intrat pe ramuura poz_aux >= 3 !!!");
        for (i = 1; i <= 3; i++){
          if(i == 1) {
            for (j=1; j <= pe_rand; j++){
              value1 = value1+"Pulsul:"+vector_puls[j-1]+"_Data:"+vector_timp[j-1][4]+vector_timp[j-1][5]+":"+vector_timp[j-1][6]+vector_timp[j-1][7]+"_"+vector_timp[j-1][2]+vector_timp[j-1][3]+"/"+vector_timp[j-1][0]+vector_timp[j-1][1]+"___";
            }
          }
          else if(i == 2){ 
            for (j=1; j <= pe_rand; j++){
              value2 = value2+"Pulsul:"+vector_puls[pe_rand+j-1]+"_Data:"+vector_timp[pe_rand+j-1][4]+vector_timp[pe_rand+j-1][5]+":"+vector_timp[pe_rand+j-1][6]+vector_timp[pe_rand+j-1][7]+"_"+vector_timp[pe_rand+j-1][2]+vector_timp[pe_rand+j-1][3]+"/"+vector_timp[pe_rand+j-1][0]+vector_timp[pe_rand+j-1][1]+"___";
            }
          }
          else if(i == 3){
            for (j=1; j <= pe_ultimul; j++){
              value3 = value3+"Pulsul:"+vector_puls[(2*pe_rand)+j-1]+"_Data:"+vector_timp[(2*pe_rand)+j-1][4]+vector_timp[(2*pe_rand)+j-1][5]+":"+vector_timp[(2*pe_rand)+j-1][6]+vector_timp[(2*pe_rand)+j-1][7]+"_"+vector_timp[(2*pe_rand)+j-1][2]+vector_timp[(2*pe_rand)+j-1][3]+"/"+vector_timp[(2*pe_rand)+j-1][0]+vector_timp[(2*pe_rand)+j-1][1];
            }
          }
        }
      }
      else if( poz_aux == 2 ){
        //Serial.println("Am intrat pe ramuura poz_aux == 2 !!!");
        value1 = "Pulsul:"+vector_puls[0]+"_Data:"+vector_timp[0][4]+vector_timp[0][5]+":"+vector_timp[0][6]+vector_timp[0][7]+"_"+vector_timp[0][2]+vector_timp[0][3]+"/"+vector_timp[0][0]+vector_timp[0][1];
        value2 = "Pulsul:"+vector_puls[1]+"_Data:"+vector_timp[1][4]+vector_timp[1][5]+":"+vector_timp[1][6]+vector_timp[1][7]+"_"+vector_timp[1][2]+vector_timp[1][3]+"/"+vector_timp[1][0]+vector_timp[1][1];        
      }
      else if (poz_aux == 1){ 
        //Serial.println("Am intrat pe ramuura poz_aux == 1 !!!");
        value1 = "Pulsul:"+vector_puls[0]+"_Data:"+vector_timp[0][4]+vector_timp[0][5]+":"+vector_timp[0][6]+vector_timp[0][7]+"_"+vector_timp[0][2]+vector_timp[0][3]+"/"+vector_timp[0][0]+vector_timp[0][1];  
      }
      //Serial.println(value1);
      //Serial.println(value2);
      //Serial.println(value3);
      HTTPClient http;
      http.begin("http://maker.ifttt.com/trigger/"+event_name+"/with/key/"+key+"?value1="+value1+"&value2="+value2+"&value3="+value3);
      http.GET();
      http.end();
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Datele au fost");
      lcd.setCursor(3,1);
      lcd.print("transmise!");
      meniu_internet = 0;
      pe_rand = 0;
      pe_ultimul = 0;
      delay(3000);
      lcd.clear();
    }
  }

  if( meniu_bluetooth == 1) {
    if ( poz == 0 ) {
      lcd.setCursor(1,0);
      lcd.print("Nu exista date");
      lcd.setCursor(1,1);
      lcd.print("inregistrate!");
      meniu_bluetooth = 0;
      delay(3000);
      lcd.clear();
    }
    else {
      //delay(1000);
      lcd.setCursor(1,0);
      lcd.print("Trimit datele");
      lcd.setCursor(3,1);
      lcd.print("prin BT...");
      delay(3000);
      int i = 0;
      String data;
      for (i = 0; i < poz; i++) {
        data = "";
        data = String(vector_puls[i]);
        Serial.print(data);
        Serial.print(",");
        data = "";
        data =data+vector_timp[i][4]+vector_timp[i][5]+":"+vector_timp[i][6]+vector_timp[i][7]+"_"+vector_timp[i][2]+vector_timp[i][3]+"/"+vector_timp[i][1];
        
        Serial.print(data);
        
        if (i == (poz-1)) {
          Serial.print(";");
        }
        else
        {
          Serial.print(",");
        }
      }
      delay(20);
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Datele au fost");
      lcd.setCursor(3,1);
      lcd.print("transmise!");
      meniu_bluetooth = 0;
      delay(3000);
      lcd.clear();
    }
  }
    
  delay(100);
}
