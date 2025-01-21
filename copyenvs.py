Import("env")
from pathlib import Path
import os


def load_env_file(env_file_path):
    """Liest eine .env-Datei und lädt die Variablen in ein Dictionary."""
    print(f"Lade Umgebungsvariablen aus {env_file_path} ...")
    if not Path(env_file_path).is_file():
        print(f"Fehler: Datei {env_file_path} nicht gefunden!")
        return {}

    variables = {}
    with open(env_file_path, "r") as file:
        for line in file:
            # Entfernt Leerzeichen oder Kommentare
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            try:
                # Zerlegt KEY=VALUE Paare
                key, value = line.split("=", 1)
                variables[key.strip()] = value.strip()
            except ValueError:
                print(f"Ungültige Zeile in .env-Datei: {line}")
    return variables


def add_env_variables_to_build(env_file_path):
    """Lädt Umweltvariablen aus der .env-Datei und fügt sie zum Build hinzu."""
    env_vars = load_env_file(env_file_path)
    if not env_vars:
        print("Keine gültigen Variablen gefunden.")
        return

    for key, value in env_vars.items():
        print(f"Setze {key} = {value} als CPPDEFINE...")
        # Fügt jeden Eintrag aus der .env-Datei als Build-Makro hinzu
        env.Append(CPPDEFINES=[(key, env.StringifyMacro(value))])


# Legt den Pfad zu deiner .env-Datei fest
env_file_path = ".env"

# Führt die Funktion aus, bevor der Build startet
add_env_variables_to_build(env_file_path)
