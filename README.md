<div align="center">
  <h1>🏡 WINDOW GUARDIAN 🛡️</h1>

  <p>
    <a href="https://www.espressif.com/"><img src="https://img.shields.io/badge/ESP32-Supported-blue.svg" alt="ESP32 Support"></a>
    <a href="https://www.espressif.com/"><img src="https://img.shields.io/badge/ESP8266-Supported-blue.svg" alt="ESP8266 Support"></a>
    <a href="https://vuejs.org/"><img src="https://img.shields.io/badge/Vue-3.5.13-brightgreen.svg" alt="Vue 3"></a>
    <a href="https://expressjs.com/"><img src="https://img.shields.io/badge/Express-4.16-orange.svg" alt="Express JS"></a>
    <a href="https://window.nicolasfritz.dev/"><img src="https://img.shields.io/badge/Demo-Live-success.svg" alt="Live Demo"></a>
  </p>

  <blockquote>
    <em><strong>Dein Fenster im Blick. Überall.</strong></em>
  </blockquote>

  <p>
    <a href="https://window.nicolasfritz.dev/">🔗 Live Demo ansehen</a>
  </p>
</div>

---

## 🌟 Projektübersicht

Window Guardian ist ein Smart-Home-Projekt, das deine Fenster überwacht und darstellt.

- 🔔 **Echtzeitdaten** über Fensteröffnungen
- 🌐 **Fernzugriff** auf alle Window Guardian Sensoren
- 🔄 **Einfache Kalibrierung** mit 3 klicks

---

## ⚙️ Voraussetzungen

| Komponente | Version          | Priorität |
|------------|------------------|-----------|
| Node.js & npm | ≥ v20.x          | Erforderlich |
| PlatformIO | Aktuelle Version | Erforderlich |
| Docker & Docker Compose | Aktuelle Version | Optional |
| ESP32/ESP8266 | -                | Hardware |

---

## 🚀 Installation und Ausführung

### 🔧 1. ESP Setup (ESP Frontend + PlatformIO)

<details>
<summary><b>Klicke hier für detaillierte Installationsschritte</b></summary>

#### 📦 1.1. ESP Frontend-Abhängigkeiten installieren
```bash
  cd frontend/espfe
  npm install
```

#### 🏗️ 1.2. Frontend für ESP bauen
```bash
  npm run build
```

#### 📂 1.3. Zum ESP-Verzeichnis navigieren
```bash
  cd ../../esp
```

#### ⚡ 1.4. Umgebungsvariablen konfigurieren
```bash
  cp .env.example .env
  # .env-Datei nach Bedarf anpassen (WiFi-Daten usw.)
```

#### 📤 1.5. Dateisystem auf den ESP hochladen
```bash
  pio run -t uploadfs
```

> 💡 **Pro-Tipp:** Überprüfe den COM-Port in der Platformio-Konfiguration!

#### 📡 1.6. ESP-Programm hochladen & starten
```bash
  pio run -t upload -t monitor
```
</details>

### 🐳 2. Docker Setup (Optional)

<details>
<summary><b>Starte das Projekt mit Docker</b></summary>

#### 🔄 API-Container starten:
```bash
  docker run -p 3000:3000 registry.thorge-engelmann.de/window_quardian_pubapi:latest
```

#### 🖥️ Frontend-Container starten:
```bash
  docker run -p 8080:80 registry.thorge-engelmann.de/window_quardian_pubfe:latest
```

#### 🔗 Zusammen mit Docker Compose:
```yaml
# docker-compose.yml
version: '3'
services:
  api:
    image: registry.thorge-engelmann.de/window_quardian_pubapi:latest
    ports:
      - "3000:3000"
  frontend:
    image: registry.thorge-engelmann.de/window_quardian_pubfe:latest
    ports:
      - "8080:80"
    depends_on:
      - api
```

```bash
  docker-compose up -d
```
</details>

---

## 📁 Projektstruktur

```
untitled/
│
├── 🔌 backend/                 # Express.js Backend-Server
│   │
│   ├── 🚀 app.js               # Haupteinstiegspunkt der Anwendung
│   └── 🛣️ routes/              # Routen für Speicherung und Übertragung: 
│                               # ESP → Server → Frontend
│
├── 🤖 esp/                     # ESP32
│   │
│   ├── 📂 data/                # SPIFFS-Dateisystem für Frontend-Assets
│   ├── 💻 src/                 # Quellcode
│   │   ├── 🌐 http/            # HTTP-Kommunikationsschicht
│   │   │   ├── 🛣️ routes/      # API-Routen Definitionen
│   │   │   │   ├── ApiRouter.cpp    # API für Sensor-Status und Steuerung
│   │   │   │   └── WebRouter.cpp    # Route für SPIFFS-Frontend-Auslieferung
│   │   │   ├── HttpClient.cpp  # HTTPS-Client für Backend-Kommunikation
│   │   │   ├── HttpServer.cpp  # ESP-HTTP-Server mit Router-Integration
│   │   │   └── Router.cpp      # Express-ähnlicher Router für Endpunkt-Registrierung
│   │   │
│   │   ├── 📡 sensor/          # Sensor-Implementierungen
│   │   │   ├── SensorSaveState.cpp   # Zustandsverwaltung für Fensterpositionen
│   │   │   └── Ultrasonicsensor.cpp  # Ultraschallsensor mit Entfernungsberechnung
│   │   │
│   │   ├── 🔐 cert.pem         # SSL-Zertifikat für sichere API-Kommunikation
│   │   ├── SpiffsManager.cpp   # Dateisystem-Verwaltung für Webinhalte
│   │   ├── StateManager.cpp    # Persistente Konfigurationsspeicherung (NVS)
│   │   ├── WifiManager.cpp     # WLAN-Verbindungsmanagement
│   │   └── main.cpp            # Hauptprogramm mit Initialisierung und Event-Loop
│   │
│   ├── 📚 include/             # Header-Dateien und Dokumentation
│   ├── ⚙️ platformio.ini       # PlatformIO-Konfiguration für Build und Deployment
│   ├── 💾 partitions.csv       # ESP32-Speicherpartitionierung für SPIFFS
│   ├── 🔄 copyenvs.py          # Build-Hook für Umgebungsvariablen-Integration
│   └── 🔑 .env                 # Konfigurationsparameter und Zugangsdaten
│                               # (aus .env.example erstellen)
│
├── 🎨 frontend/                # Vue.js Benutzeroberflächen
│   ├── 🖥️ espfe/               # Embedded Web-UI für den ESP32
│   │   └── 💻 src/             # Vue.js & JavaScript Quellcode
│   │
│   └── 🌐 pubfe/               # Server Web-Anwendung
│       ├── 💻 src/             # Vue.js & TypeScript Komponenten
│       └── 🐳 Dockerfile       # Containerisierte Bereitstellung via Nginx
│
└── 📝 README.md                # Du bist hier! 👋
```

---

<div align="center">

### 🔒 LET'S GUARD THOSE WINDOWS! 🔒

<i>Entwickelt von Nicolas Fritz</i>

</div>

