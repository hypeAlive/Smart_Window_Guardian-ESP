<template>
  <div
      @click="navigate"
      class="flex items-center justify-between p-6 bg-gradient-to-r from-white to-gray-100 dark:from-gray-800 dark:to-gray-700 shadow-2xl rounded-xl transform transition-all duration-300 hover:scale-105 hover:shadow-2xl"
  >
    <!-- Linker Bereich: Icon und Text -->
    <div class="flex items-center space-x-2"> <!-- Abstand reduziert: space-x-2 -->
      <!-- Icon links: Weiß, kleiner und zentriert -->
      <div
          class="flex items-center justify-center h-10 w-10"
      >
        <component
            :is="icon"
            class="h-10 w-10 text-white"
        ></component>
      </div>

      <!-- Text rechts daneben -->
      <div>
        <h2 class="text-2xl font-bold text-gray-800 dark:text-gray-100">
          {{ name }}
        </h2>
        <p
            :class="getStateDisplay(state).class"
            class="font-medium text-sm"
        >
          {{ getStateDisplay(state).text }}
        </p>
      </div>
    </div>

    <!-- Rechter Bereich: Neuer Button -->
    <button
        @click="navigate"
        class="flex items-center justify-center w-10 h-10 rounded-full bg-blue-100 dark:bg-gray-600 text-blue-500 dark:text-white hover:bg-gray-100 dark:hover:bg-gray-700 hover:text-gray-700 dark:hover:text-white transition-all duration-300 group shadow-md hover:shadow-lg"
        aria-label="Navigate"
    >
      <svg
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          stroke-width="2"
          stroke="currentColor"
          class="w-6 h-6 transform transition-transform duration-300 group-hover:translate-x-1"
      >
        <path
            stroke-linecap="round"
            stroke-linejoin="round"
            d="M9 5l7 7-7 7"
        />
      </svg>
    </button>
  </div>
</template>

<script lang="ts">
import { defineComponent } from "vue";
import type {WindowSaveStates} from "@/stores/windowGuardian.ts";

export default defineComponent({
  props: {
    icon: {
      type: [Object, null],
      required: true,
    },
    name: {
      type: String,
      default: "Beispiel Fenster",
    },
    state: {
      type: String,
      default: "UNKNOWN" as WindowSaveStates,
    },
    ip: {
      type: String,
      required: true
    }
  },
  methods: {
    navigate() {
      window.location.href = "http://" + this.ip;
    },
    getStateDisplay(state: string) {
      switch (state) {
        case "ON":
          return { class: "text-green-500", text: "Geöffnet" };
        case "OFF":
          return { class: "text-red-500", text: "Geschlossen" };
        case "MID":
          return { class: "text-yellow-500", text: "Gekippt" };
        case "UNKNOWN":
        default:
          return { class: "text-yellow-500", text: "Setup benötigt" };
      }
    },
  },
});
</script>

<style scoped>
/* Keine zusätzlichen Styles notwendig */
</style>