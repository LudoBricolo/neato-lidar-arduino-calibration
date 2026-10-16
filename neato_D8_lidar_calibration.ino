// --- Configuration du LiDAR Neato (100% USB Serial) ---
static uint8_t index = 0;
static uint8_t buffer[22];

// --- Variables pour le calcul de la vitesse (RPM) ---
unsigned long lastA0Time = 0;
float currentRPM = 0.0;
float smoothedRPM = 0.0;

// --- Variables pour le calcul de la distance de référence ---
long targetDistSum = 0;
int targetPointCount = 0;

void setup() {
  Serial.begin(115200);   // Moniteur Série USB principal
  Serial1.begin(115200);  // Liaison série avec le LiDAR
  
  Serial.println("Arduino prêt. Mode 100% USB Serial (sans Wi-Fi).");
}

void loop() {
  if (Serial1.available() > 0) {
    uint8_t incomingByte = Serial1.read();
    
    if (index == 0 && incomingByte != 0xFA) {
      return; 
    }
    
    buffer[index++] = incomingByte;
    
    if (index == 22) {
      index = 0;
      
      int packetIndex = buffer[1];
      
      // DÉTECTION DU PAQUET 0xA0 (Fin d'un tour / Début du suivant)
      if (packetIndex == 0xA0) {
        unsigned long currentTime = millis();
        if (lastA0Time > 0) {
          unsigned long duration = currentTime - lastA0Time; // Durée du tour en ms
          
          // Filtre de cohérence pour un tour complet
          if (duration >= 100 && duration <= 400) {
            currentRPM = 60000.0 / duration;
            
            // Lissage de la vitesse
            if (smoothedRPM == 0.0) {
              smoothedRPM = currentRPM;
            } else {
              smoothedRPM = (0.7 * smoothedRPM) + (0.3 * currentRPM);
            }

            // Calcul de la distance moyenne de la cible pour ce tour
            float avgTargetDist = 0.0;
            if (targetPointCount > 0) {
              avgTargetDist = (float)targetDistSum / targetPointCount;
            }

            // Affichage propre dans le Moniteur Série
            Serial.print("Vitesse : ");
            Serial.print(smoothedRPM, 1);
            Serial.print(" RPM | Cible ");
            if (targetPointCount >= 5) {
              Serial.print("trouvée : ");
              Serial.print(avgTargetDist, 1);
              Serial.println(" mm");
            } else {
              Serial.println(": non détectée");
            }
          }
        }
        lastA0Time = currentTime;
        
        // Remise à zéro pour le tour suivant
        targetDistSum = 0;
        targetPointCount = 0;
      }

      // Traitement des angles et distances
      if (packetIndex >= 0xA0 && packetIndex <= 0xEF) {
        int baseAngle = (packetIndex - 0xA0) * 4;

        for (int i = 0; i < 4; i++) {
          int offset = 4 + (i * 4);
          uint8_t byte0 = buffer[offset];
          uint8_t byte1 = buffer[offset + 1]; 
          
          if (byte1 & 0x80) continue; // Erreur de mesure

          uint16_t rawDistance = (byte1 << 8) | byte0;
          uint16_t distanceMM = rawDistance & 0x3FFF;

          if (distanceMM == 0 || distanceMM > 4000) continue;

          // Détection de la cible (190mm à 310mm)
          if (distanceMM >= 190 && distanceMM <= 310) {
            targetDistSum += distanceMM;
            targetPointCount++;
          }
        }
      }
    }
  }
}
