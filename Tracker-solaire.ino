#include <Servo.h>

// définir des broches pour le tracker solaire
#include <Servo.h>
// Initialiser les variables
int mode = 0;
int axe = 0;            
int buttonState1 = 0;    
int buttonState2 = 0;   
int prevButtonState1 = 0;
int prevButtonState2 = 0; 
 
int ldrtopr = 0;                // LDR en haut à droite                          
int ldrtopl = 1;               // LDR en haut à gauche                          
int ldrbotr = 2;               // LDR en bas à droite                     
int ldrbotl = 3;               // LDR en bas à gauche                   
int topl = 0;
int topr = 0; 
int botl = 0;
int botr = 0;

// Déclarer deux servos
Servo servo_updown;
Servo servo_rightleft;

int threshold_value = 10;           // Sensibilité de mesure
//prototype de la fonction
  void manualsolartracker(); 
  void automaticsolartracker();

void setup()
{
  Serial.begin(9600);                                // Configuration de la connexion série  // Ouvre le port série, définit le débit de données à 9600 bps
  Serial.println("CLEARDATA");                       // Effacer toutes les données déjà placées
  Serial.println("LABEL,t,voltage,current,power,Mode");   // Définir les en-têtes de colonnes (commande PLX-DAQ)

  pinMode(12, INPUT);              // Bouton de changement de mode
  pinMode(11, INPUT);              // Changement d'axe
  pinMode(A4, INPUT);              // Potentiomètre pour le mouvement gauche-droite et pour le mouvement haut-bas
  
  servo_updown.attach(5);             // Servomoteur pour le mouvement haut-bas
  servo_rightleft.attach(6);          // Servomoteur pour le mouvement gauche-droite
}

void loop()
{
  // pv_power();
  char Mode;
  float volt = analogRead(A5) * 5.0 / 1023;
  float voltage = 2 * volt;                // Volt = (R1 / (R1 + R2)) * Voltage / R1 = R2 = 10 Ohms  => voltage = 2 * volt
  float current = voltage / 20;            // I = voltage / (R1 + R2) 
  float power = voltage * current;
  Serial.print("DATA,TIME,"); // Commande PLX-DAQ
  Serial.print(voltage);    // Envoyer la tension au port série
  Serial.print(",");
  Serial.print(current);    // Envoyer le courant au port série
  Serial.print(",");
  Serial.print(power);  // Envoyer la puissance au port série
  Serial.print(",");
  
  // Serial.println(Mode);      
  buttonState1 = digitalRead(12);
  if (buttonState1 != prevButtonState1)  {
    if (buttonState1 == HIGH) {
      // Changer de mode et allumer l'indicateur correct  
      if (mode == 1) {
        mode = 0;
      } else {
        mode = 1;
      }
    }
  }
  prevButtonState1 = buttonState1;
  delay(50); // Attendre 50 millisecondes
  
  if (mode == 0) {
    Mode = 'M';
    Serial.println(Mode);   // Envoyer le mode "Manuel" au port série    
    manualsolartracker();
  } else { // Mode automatique
    Mode = 'A';
    Serial.println(Mode);      
    automaticsolartracker(); // Envoyer le mode "Automatique" au port série
  } 
}

void automaticsolartracker() {
  
  // Capturer les valeurs analogiques de chaque LDR
  topr = analogRead(ldrtopr);         // Capturer la valeur analogique du LDR en haut à droite
  topl = analogRead(ldrtopl);         // Capturer la valeur analogique du LDR en haut à gauche
  botr = analogRead(ldrbotr);         // Capturer la valeur analogique du LDR en bas à droite
  botl = analogRead(ldrbotl);         // Capturer la valeur analogique du LDR en bas à gauche

    // Calculer la moyenne
    int avgtop = (topr + topl) / 2;     // Moyenne des LDR en haut
    int avgbot = (botr + botl) / 2;     // Moyenne des LDR en bas
    int avgleft = (topl + botl) / 2;    // Moyenne des LDR à gauche
    int avgright = (topr + botr) / 2;   // Moyenne des LDR à droite

    // Obtenir la différence  
    int diffelev = avgtop - avgbot;      // Obtenir la différence moyenne entre les LDR en haut et les LDR en bas
    int diffazi = avgright - avgleft;    // Obtenir la différence moyenne entre les LDR à droite et les LDR à gauche

    // Mouvement gauche-droite du tracker solaire
    if (abs(diffazi) >= threshold_value) {        // Changer de position seulement si la différence de lumière est supérieure à la valeur seuil
        if (diffazi > 0) {
            if (servo_rightleft.read() < 180) {
                servo_rightleft.write((servo_updown.read() + 2));
            }
        }
        if (diffazi < 0) {
            if (servo_rightleft.read() > 0) {
                servo_rightleft.write((servo_updown.read() - 2));
            }
        }
    }

    // Mouvement haut-bas du tracker solaire
    if (abs(diffelev) >= threshold_value) {    // Changer de position seulement si la différence de lumière est supérieure à la valeur seuil
        if (diffelev > 0) {
            if (servo_updown.read() < 180) {
                servo_updown.write((servo_rightleft.read() - 2));
            }
        }
        if (diffelev < 0) {
            if (servo_updown.read() > 0) {
                servo_updown.write((servo_rightleft.read() + 2));
            }
        }
    }       
}

void manualsolartracker() {
    buttonState2 = digitalRead(13);
    if (buttonState2 != prevButtonState2) {
        if (buttonState2 == HIGH) {
            // Changer de mode et allumer l'indicateur correct  
            if (axe == 1) {
                axe = 0;
            } else {
                axe = 1;
            }
        }
    }
    prevButtonState2 = buttonState2;
    delay(50); // Attendre 50 millisecondes
    if (axe == 0) {     // Contrôler le mouvement gauche-droite
        servo_rightleft.write(map(analogRead(A4), 0, 1023, 0, 180));
    } else { // Contrôler le mouvement haut-bas
        servo_updown.write(map(analogRead(A4), 0, 1023, 0, 180));
    } 
}