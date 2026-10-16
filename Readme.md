# Neato LDS 2.2 LiDAR Calibration & Arduino Interface

This repository documents the testing, calibration, and optimization results for a Neato LDS 2.2 LiDAR module driven by an microcontroller (tested on an Arduino Uno R4 WiFi).

## 🎯 Key Calibration Results
Contrary to common misconceptions and vague online documentation (which often suggest 3.3V or 5V depending on the generation), rigorous metrology using a **stable laboratory power supply** established the optimal parameters for this hardware:

* **Optimal Motor Supply Voltage**: **2.5 V**
* **Stabilized Rotation Speed**: **~296 RPM** (extremely close to the theoretical original target of 300 RPM, or 5 Hz).
* **Optical Precision**: Ultra-stable and repeatable distance measurements (e.g., a constant ~260 mm on a fixed target).

---

## ⚠️ Important Technical Pitfall: The Tachometer Illusion
During initial testing with code combining high-speed serial reading and network tasks (Wi-Fi/UDP), the Arduino experiences micro-saturations in its serial buffer. This causes intermittent loss of the `0xA0` synchronization packet (which marks the beginning of a full revolution).

* **Consequence**: The revolution duration calculation doubles, artificially halving the calculated RPM and creating fake, paradoxical speed drops even though the laboratory voltage remains perfectly stable.
* **Solution**: For reliable motor calibration, it is strongly recommended to isolate the network stack (using a 100% USB Serial mode) and apply a temporal consistency filter.

---

## 📂 Arduino Code (100% USB Serial + RPM Filtering)
The complete source code used to validate stability at 2.5 V is provided in the `neato_lidar_calibration.ino` file.
