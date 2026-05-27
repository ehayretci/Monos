# Monos — Wearable Cycling Navigation System

Monos is a haptic-first navigation system for cyclists. Instead of glancing at a screen, riders receive turn signals through vibration motors embedded in a pair of smart gloves, while a servo-driven compass module physically points toward the destination — all coordinated in real time via Google Maps.

---

## How It Works

A Python script on the rider's phone or laptop fetches a cycling route from the Google Maps Directions API. As the ride progresses, it fires HTTP requests to three ESP32 microcontrollers — one in each glove and one in the compass module — at exactly the right moment based on the rider's speed.

- **Left turn** → left glove vibrates
- **Right turn** → right glove vibrates
- **Arrival** → both gloves pulse three times + a long buzz
- **Compass** → servo motor physically rotates to face the next waypoint; green LED = on track, red LED = off track

---

## Hardware

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Dev Board | 3 | One per glove + compass |
| Vibration motor | 2 | One per glove (pin 23) |
| LED strip | 2 | Turn-signal indicator, one per glove (pin 2) |
| Touch sensor | 2 | Manual LED trigger, one per glove (pin 4) |
| Servo motor | 1 | Compass needle (pin 18) |
| Red + Green LED | 1 each | Direction indicator on compass (pins 25, 26) |

---

## Repository Structure

```
monos/
├── left_glove.ino       # ESP32 firmware for the left glove
├── right_glove.ino      # ESP32 firmware for the right glove
├── Geocompass.ino       # ESP32 firmware for the servo compass module
├── gizmo_lights.ino     # LED controller firmware
├── gizmo_compass.ino    # Compass auxiliary firmware
├── demo1.py             # Navigation script v1 — basic turn signals
├── demo2.py             # Navigation script v2 — turn signals + live location to compass
└── Gizmo_portfolio.pdf  # Full project documentation & portfolio deck
```

---

## Getting Started

### 1. Flash the ESP32s

Open each `.ino` file in the Arduino IDE and flash it to the corresponding ESP32. Before flashing, update the Wi-Fi credentials at the top of each file:

```cpp
const char* ssid = "YourNetworkName";
const char* password = "YourPassword";
```

**Required Arduino libraries:**
- `WiFi` (built-in)
- `ESPAsyncWebServer`
- `ESP32Servo` (Geocompass only)

### 2. Find the IP Addresses

After each ESP32 boots and connects to Wi-Fi, it prints its IP address to the Serial Monitor. Note these down — you'll need them in the Python scripts.

### 3. Configure the Python Scripts

In `demo1.py` or `demo2.py`, replace the placeholder IP addresses with the ones from your ESP32s:

```python
# Replace these placeholders in the scripts
'http://<RIGHT_GLOVE_IP>/right'
'http://<LEFT_GLOVE_IP>/left'
'http://<COMPASS_IP>/currentlocation?latitude=...&longitude=...'
```

Also add your Google Maps API key:

```python
google_api_key = 'YOUR_GOOGLE_MAPS_API_KEY'
```

> The Google Maps API key must have **Geocoding API** and **Directions API** enabled.

### 4. Run

```bash
pip install requests
python demo2.py
```

Enter your origin, destination, and current speed in m/s. The script will handle the rest.

---

## Demo Scripts

**`demo1.py`** — Sends left/right vibration signals and an arrival pulse based on timed step distances.

**`demo2.py`** — Extends demo1 by also sending live GPS coordinates to the compass module at each turn, so the servo updates its heading in real time.

---

## Notes

- All three ESP32s must be on the same Wi-Fi network as the machine running the Python script.
- Speed is entered manually — a future version could pull live speed from the device's GPS.
- The compass uses 5-decimal-place rounding (~1.1 m geosensitivity) for smooth servo updates.

---

*Built by Erim Hayretci*