<script setup>
import CardComponent from "@/components/CardComponent.vue";
import IconWindowOpen from "@/components/icons/IconWindowOpen.vue";
import IconWindowMid from "@/components/icons/IconWindowMid.vue";
import IconWindowClosed from "@/components/icons/IconWindowClosed.vue";
</script>

<template>
  <div class="bg-gray-100 dark:bg-gray-900 text-black dark:text-white flex flex-col items-center justify-center min-h-screen space-y-8 px-4">
    <div class="text-center">
      <h1 class="text-4xl font-extrabold">Window Guardian Setup</h1>
      <p class="text-lg text-gray-600 dark:text-gray-300 mt-2">
        Richten Sie Ihren Window Guardian sicher und schnell ein. Befolgen Sie die Anweisungen, um das Fenster zu kalibrieren.
      </p>
    </div>

    <div class="grid grid-cols-1 md:grid-cols-3 gap-6 w-full max-w-6xl">
      <!-- Karte: Fenster Offen -->
      <CardComponent
          :icon="IconWindowOpen"
          title="Fenster Offen"
          description="Öffnen Sie das Fenster komplett und drücken Sie 'Kalibrierung starten', um den offenen Zustand einzurichten."
          buttonText="Kalibrierung starten"
          @click="handleStep(1)"
      />

      <!-- Karte: Fenster Kipp -->
      <CardComponent
          :icon="IconWindowMid"
          title="Fenster Gekippt"
          description="Kippen Sie das Fenster und drücken Sie 'Kalibrierung starten', um den gekippten Zustand einzurichten."
          buttonText="Kalibrierung starten"
          @click="handleStep(2)"
      />

      <!-- Karte: Fenster Geschlossen -->
      <CardComponent
          :icon="IconWindowClosed"
          title="Fenster Geschlossen"
          description="Schließen Sie das Fenster komplett und drücken Sie 'Kalibrierung starten', um den geschlossenen Zustand einzurichten."
          buttonText="Kalibrierung starten"
          @click="handleStep(3)"
      />
    </div>
  </div>
</template>

<script>
export default {
  name: "WindowGuardianSetup",
  methods: {
    handleStep(step) {
      console.log(`Kalibrierung für Zustand ${step} gestartet.`);
    },
  },
};
</script>

<style scoped>
header {
  line-height: 1.5;
}

.logo {
  display: block;
  margin: 0 auto 2rem;
}

@media (min-width: 1024px) {
  header {
    display: flex;
    place-items: center;
    padding-right: calc(var(--section-gap) / 2);
  }

  .logo {
    margin: 0 2rem 0 0;
  }

  header .wrapper {
    display: flex;
    place-items: flex-start;
    flex-wrap: wrap;
  }
}
</style>