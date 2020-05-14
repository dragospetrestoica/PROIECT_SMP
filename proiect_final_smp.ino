#include <Wire.h>
#define I2CAddressESPWifi 8

int pulsePin = A0;                 
int blinkPin = 13;                

// Folosite pentru rutina serviciului interrupt
volatile int BPM;                   
volatile int Signal;                // stocheaza valoarea bruta primita de la senzor
volatile int IBI = 600;             // stocheaza intervalul de timp dintre bataile inimii.
volatile boolean Pulse = false;       
volatile boolean QS = false;        // "true" cand senzorul detecteaza o bataie a inimii 

volatile int rate[10];                      // vector ce retine ultimele 10 valori IBI inregistrare
volatile unsigned long sampleCounter = 0;          // pentru determinarea sincronizarii pulsului
volatile unsigned long lastBeatTime = 0;           // pentru a gasi IBI
volatile int P = 512;                      // valoarea maxima trimisa de catre senzor 
volatile int T = 512;                     // valoarea minima trimisa de catre senzor
volatile int thresh = 525;                // momentul instant al bataii inimii
volatile int amp = 100;                   // amplitudinea semnalului de puls
volatile boolean firstBeat = true;       
volatile boolean secondBeat = false;      


void setup() {
  pinMode(blinkPin,OUTPUT);         // pin care se va activa atunci cand este detectata o bataie a inimii 
  Serial.begin(9600);             
  interruptSetup();                 // vom citi senzorul de puls la fiecare 2ms

  Wire.begin(I2CAddressESPWifi);          //deshidem canalul de comunicare intre esp (master) si arduino (slave) prin adresa apelata;
  Wire.onRequest(espWifiRequestEvent);    //atunci cand primim o cerere a BPM-ului de la esp, o trimitem catre acesta
}

void loop() {
  if (QS == true) {// daca o bataie a inimii a fost gasita... 
    serialOutputWhenBeatHappens();    
    QS = false;   
  }    
  delay(20); 
}


void interruptSetup() {     
  // Initializam intreruperile la fiecare 2ms.
  TCCR2A = 0x02;     
  TCCR2B = 0x06;     
  OCR2A = 0X7C;      
  TIMSK2 = 0x02;     
  sei();             
} 

void serialOutputWhenBeatHappens() {    
  Serial.print(" Heart-Beat Found ");  
  Serial.print("BPM: ");
  Serial.println(BPM);  
}


ISR(TIMER2_COMPA_vect) {  
  cli();                                      // Dezactivam intreruperile
  Signal = analogRead(pulsePin);              // citim valoarea de la senzorul de puls 
  sampleCounter += 2;                         // urmaresc timpul 
  int N = sampleCounter - lastBeatTime;       // monitorizez cat timp a trecut de la ultima bataie pentru a evita zgomotul
  if(Signal < thresh && N > (IBI/5)*4) {      // evit zgomotul prin a verifica daca a trecut un timp de 3/5 din IBI   
    if (Signal < T) {                   
      T = Signal;                             // semnalul minim inregistrat
    }
  }

  if(Signal > thresh && Signal > P) {          
      P = Signal;                             // semnalul maxim inregistrat
    }                                       

  if (N > 250) {                              
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ) {        
      Pulse = true;                               // setam flag-ul
      digitalWrite(blinkPin,HIGH);                
      IBI = sampleCounter - lastBeatTime;         // masor timpul intre batai
      lastBeatTime = sampleCounter;               // evidenta timpului pentru urmatorul impuls
      
      if(secondBeat) {                            
        secondBeat = false;
        for(int i=0; i<=9; i++) {             
            rate[i] = IBI;                      
        }
      }
      if(firstBeat) {                         
        firstBeat = false;                
        secondBeat = true;                   // setez flag-ul pentru al doiea beat
        sei();                               // activez intreruperile
        return;                              
      }   

      word runningTotal = 0;                  

      for(int i=0; i<=8; i++) {                
          rate[i] = rate[i+1];                  
          runningTotal += rate[i];              
      }

      rate[9] = IBI;                          // adaug cel mai recent IBI catre vectorul rate
      runningTotal += rate[9];                // adaug cel mai recent IBI
      runningTotal /= 10;                     // media ultimelor 10 valori 
      BPM = 60000/runningTotal;               // numarul de batai pe minut (BPM)
      QS = true;                              // setez flag-ul pentru trimitere
    }                       
  }

  if (Signal < thresh && Pulse == true) {   
    digitalWrite(blinkPin,LOW);            
    Pulse = false;                         
    amp = P - T;                           // masor amplitudinea 
    thresh = amp/2 + T;                    // setez pragul la 1/2 din amplitudine
    P = thresh;                            
    T = thresh;
  }

  if (N > 2500) {                           // daca au trecut 2.5 secunde fara nicio bataie a inimii
    thresh = 512;                          
    P = 512;                               
    T = 512;                               
    lastBeatTime = sampleCounter;          
    firstBeat = true;                      
    secondBeat = false;                    
  }

  sei();                                   // activez intreruperile
}

void espWifiRequestEvent()                // cand master-ul esp trimite o cerere de date, arduino raspunde cu bpm-ul masurat
{
  Wire.write(BPM);
}
