#include <prismino.h>

#define IR_ENABLE 13
#define IR_SENSOR_1 A0 //capteur de droite
#define IR_SENSOR_2 A1 //capteur de gauche
#define SENSOR_DEPART A5 // capteur de depart

#define NB_SENSOR 2

#define TPS_MIN 0 //ms

int seuil[NB_SENSOR]={980, 980};  


typedef struct Sensor {
  int value; 
  int detection; 
  int tps; 
}SENSOR; 

typedef struct Moteur {
  int left; 
  int right; 
}MOTEUR; 

int depart(void); 
void detection_ligne_noire (SENSOR*); 
void prise_decision (SENSOR*, MOTEUR*); 

int moyenne (int*); 

void setup() {
  pinMode(IR_SENSOR_1, INPUT); 
  pinMode(IR_SENSOR_2, INPUT);
  pinMode(SENSOR_DEPART, INPUT);  
  pinMode(IR_ENABLE, OUTPUT); 

  Serial.begin(9600); 
}

void loop() {
  SENSOR sensors[NB_SENSOR]; 
  MOTEUR moteurs; 
  static int begun=0; 

 
  if (!begun)
    begun=depart();

  detection_ligne_noire (sensors); 
  prise_decision (sensors, &moteurs); 


  setSpeed(moteurs.left, moteurs.right); 
  //setSpeed(0, 100); 
 
  
  Serial.print(sensors[1].value); 
  Serial.print("   "); 
  Serial.print(sensors[1].detection); 
  Serial.print("   "); 
  Serial.print(sensors[0].value);
  Serial.print("   ");
  Serial.println(sensors[0].detection); 
}


//digitalRead(SENSOR_DEPART

int depart(void) {
  while ((analogRead(SENSOR_DEPART)>500)); 
  play(10000, 500); 
  while ((analogRead(SENSOR_DEPART)<500)) {
    //Serial.println
  }
  return 1; 
}


void detection_ligne_noire (SENSOR* sensors) {
  char sensor_name[NB_SENSOR]={A0, A1};
  int ambient[NB_SENSOR]; 
  int i=0; 

  for (i=0; i<NB_SENSOR; i++) {
    ambient[i]=analogRead(sensor_name[i]); 
  }

  digitalWrite(IR_ENABLE, HIGH); 
  delay(1);
  
  for (i=0; i<NB_SENSOR; i++) {
    sensors[i].value=-analogRead(sensor_name[i])+ambient[i]; 
    
  }

  digitalWrite(IR_ENABLE, LOW); 
  
  for (i=0; i<NB_SENSOR; i++) {
    if (sensors[i].value<seuil[i]) {
        if (sensors[i].detection!=1) {
          sensors[i].detection=1; 
          sensors[i].tps=millis(); 
        }
      }
      else {
        if ((millis()-sensors[i].tps)>100)
          sensors[i].detection=0; 
        }
    }

  
}

void prise_decision (SENSOR* sensors, MOTEUR* moteurs) {

  if ((sensors[0].detection)&&(sensors[1].detection)) {
    moteurs->left=100; 
    moteurs->right=100; 
  }
  else {
    if ((sensors[0].detection)&&((millis()-sensors[0].tps)>TPS_MIN)) { //capteur droit
      if (sensors[0].value>900) {
        moteurs->left=100; 
        moteurs->right=30; 
      }
      else {
        moteurs->left=100; 
        moteurs->right=-100; 
      }
    }
    else {
      if ((sensors[1].detection)&&((millis()-sensors[1].tps)>TPS_MIN)) { //capteur gauche   
        if (sensors[1].value>900) {
           moteurs->left=30; 
           moteurs->right=100; 
        }
        else {
          moteurs->left=-100; 
          moteurs->right=100; 
        }
      }
      else {
        moteurs->left=100; 
        moteurs->right=100; 
      }
    }
  }
  
}

