# Window Guardian

## Projektübersicht

---

## Voraussetzungen

---

## Installation und Ausführung

### Setup ESP (Esp Frontend + PlatformIO)

1. Installiere ESP Frontend Abhängigkeiten
   ```bash
   cd frontend/espfe
   npm install
   ```

2. Build Frontend in Esp Verzeichnis
   ```bash
   npm run build
   ```

3. Navigiere ins ESP Verzeichnis
   ```bash
   cd ../../esp
   ```
   
4. Environment Variablen einrichten
    - Kopiere die Datei `.env.example` und benenne sie um in `.env`.
    - Passe die Umgebungsvariablen an, z. B. `WIFI_SSID`, `WIFI_PASSWORD`, etc.

5. Lade Frontend Daten auf den ESP
   ```bash
   pio run -t uploadfs
   ```
    > **Achtung:** Stelle sicher, dass der richtige **COM-Port** für die Verbindung zu deinem ESP eingestellt ist!

6. Führe ESP aus
   ```bash
   pio run -t upload -t monitor
   ```

### Public API und Frontend (Optional)
Die aktuellen Docker-Images für die Public API und das Frontend sind unter folgendem Link verfügbar: <br>
- API: <br> registry.thorge-engelmann.de/window_quardian_pubapi:latest
- Frontend: <br> registry.thorge-engelmann.de/window_quardian_pubfe:latest

---

## Projektstruktur

untitled/<br>
 ├── backend/ # Public ExpressJS Backend API<br>
 ├── esp/ # Esp PlatformIO Programm Dateien <br>
 ├── frontend/ # Vue3 Frontend Dateien <br>
 │ ├── espfe/ # Esp localhost Frontend<br>
 │ └── pubfe/ # Public Frontend <br>
 └── README.md # Projektbeschreibung (diese Datei)
