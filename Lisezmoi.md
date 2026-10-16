# Neato LDS 2.2 LiDAR Calibration & Arduino Interface

Ce dépôt documente les résultats de tests, de calibration et d'optimisation d'un bloc LiDAR Neato (LDS 2.2) piloté par un Arduino (testé sur Arduino Uno R4 WiFi).

## 🎯 Résultats clés de la calibration
Contrairement aux idées reçues et aux documentations souvent vagues sur le web (qui préconisent 3.3V ou 5V selon les générations), une métrologie rigoureuse avec une **alimentation de laboratoire stable** a permis d'établir les paramètres parfaits pour cet exemplaire :

* **Tension d'alimentation optimale du moteur** : **2.5 V**
* **Vitesse stabilisée obtenue** : **~296 RPM** (très proche de la consigne théorique d'origine de 300 RPM, soit 5 Hz).
* **Précision optique** : Mesure de distance ultra-stable et répétable (ex: ~260 mm constants sur une cible fixe).

---

## ⚠️ Piège technique important : Le faux problème du compte-tours
Lors des tests initiaux avec un code combinant lecture série haute vitesse et tâches réseau (Wi-Fi/UDP), l'Arduino subit des micro-saturations du buffer. Cela provoque la perte intermittente du paquet de synchronisation `0xA0` (qui marque le début d'un tour complet). 

* **Conséquence** : Le calcul de la durée du tour double, ce qui fausse le RPM en le divisant par deux (donnant de faux ralentissements paradoxaux alors que la tension de laboratoire est stable).
* **Solution** : Pour une calibration fiable du moteur, il est fortement conseillé de s'affranchir de la pile réseau (mode 100% USB Serial) et d'appliquer un filtre de cohérence temporelle.

---

## 📂 Code Arduino (100% USB Serial + Filtrage RPM)
Le code source complet utilisé pour valider la stabilité à 2.5 V est disponible dans le fichier `neato_lidar_calibration.ino`.
