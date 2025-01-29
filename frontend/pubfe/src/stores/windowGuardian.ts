import { defineStore } from "pinia";
import axios from "axios";
import {isApiReachable} from "@/utils/apiUtils.ts";

export type WindowSaveStates = "ON" | "UNKNOWN" | "OFF" | "MID";
export type WindowStates = "SETUP" | "UNKNOWN";

export type WindowGuardian = WindowGuardianIp & {
    saveState: WindowSaveStates;
    state: WindowStates;
}

export type WindowGuardianIp = {
    id: string;
    ip: string;
}

export const useWindowGuardianStore = defineStore("windowGuardian", {
    state: () => ({
        windowStates: [] as WindowGuardian[],
        intervalId: null as ReturnType<typeof setInterval> | null,
        ips: [] as string[], // Neuen State für fehlende IPs hinzufügen
    }),

    actions: {
        // HTTP-Abfrage für Fensterzustände
        async fetchWindowStates() {
            try {
                const response = await axios.get("https://window.nicolasfritz.dev/api/esp");
                this.windowStates = response.data as WindowGuardian[];
                console.log("Fensterzustände erfolgreich geladen:", this.windowStates);
            } catch (error) {
                console.error("Fehler beim Abrufen der Fensterzustände:", error);
            }
        },

        // Abfrage für IPs machen und sie mit windowStates vergleichen
        async fetchIpsAndCheck() {
            try {
                const response = await axios.get("https://window.nicolasfritz.dev/api/esp/ips");
                const ipsFromApi: string[] = response.data;

                const existingIps = this.windowStates.map((window) => window.ip);

                this.ips = [];
                for (const ip of ipsFromApi) {
                    if (!existingIps.includes(ip)) {
                        const reachable = await isApiReachable(ip); // Globale Funktion aufrufen
                        if (reachable) {
                            this.ips.push(ip);
                        } else {
                            console.warn(`API unter http://${ip}/api nicht erreichbar.`);
                        }
                    }
                }

                console.log("Erreichbare, fehlende IPs gespeichert:", this.ips);
            } catch (error) {
                console.error("Fehler beim Abrufen der IPs:", error);
            }
        },

        // Startet die periodische HTTP-Abfrage
        startFetching(intervalMs: number = 3000) {
            if (this.intervalId !== null) {
                console.warn("Intervall ist bereits aktiv.");
                return;
            }

            // Initialer Abruf
            this.fetchWindowStates();
            this.fetchIpsAndCheck();

            // Wiederholte Abfragen mit setInterval
            this.intervalId = setInterval(() => {
                this.fetchWindowStates();
                this.fetchIpsAndCheck();
            }, intervalMs);
        },

        // Stoppt die periodischen HTTP-Abfragen
        stopFetching() {
            if (this.intervalId !== null) {
                clearInterval(this.intervalId);
                this.intervalId = null;
            }
        },
    },
});