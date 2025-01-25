<script setup>

import { ref, onMounted } from "vue";
import { markRaw } from "vue";
import IconWindowOpen from "@/components/icons/IconWindowOpen.vue";
import IconWindowMid from "@/components/icons/IconWindowMid.vue";
import IconWindowClosed from "@/components/icons/IconWindowClosed.vue";
import CardComponent from "@/components/CardComponent.vue";

const props = defineProps({
  setupNeeds: {
    type: Array,
    required: true,
  },
});

const localSetupNeeds = ref([...props.setupNeeds || []]);

const cards = ref([
  {
    id: "ON",
    icon: markRaw(IconWindowOpen),
    title: "Fenster Offen",
    description:
        "Öffnen Sie das Fenster komplett und drücken Sie 'Kalibrierung starten', um den offenen Zustand einzurichten.",
    disabled: true,
    loading: false
  },
  {
    id: "MID",
    icon: markRaw(IconWindowMid),
    title: "Fenster Gekippt",
    description:
        "Kippen Sie das Fenster und drücken Sie 'Kalibrierung starten', um den gekippten Zustand einzurichten.",
    disabled: true,
    loading: false
  },
  {
    id: "OFF",
    icon: markRaw(IconWindowClosed),
    title: "Fenster Geschlossen",
    description:
        "Schließen Sie das Fenster komplett und drücken Sie 'Kalibrierung starten', um den geschlossenen Zustand einzurichten.",
    disabled: true,
    loading: false
  },
]);

const handleCalibration = async (card) => {
  console.log(`Kalibrierung gestartet für: ${card.id}`);
  card.loading = true;
  disableAllCards();

  try {
    const response = await fetch("/api/save-state", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        state: card.id,
      }),
    });

    if (!response.ok) {
      throw new Error(`Fehler bei der Kalibrierung: ${response.statusText}`);
    }

    const newNeeds = (await response.json())["needs"];

    localSetupNeeds.value = Array.isArray(newNeeds) ? newNeeds : [];

    if(localSetupNeeds.value.length === 0) {
      window.location.reload();
      return;
    }

    activateAllCards();
  } catch (error) {
    console.error("Fehler beim Abrufen der Kalibrierung:", error);
  } finally {
    card.loading = false;
  }
};

const disableAllCards = () => {
  cards.value.forEach((card) => {
    card.disabled = true;
  });
};

const activateAllCards = () => {
  cards.value.forEach((card) => {
    if (Array.isArray(localSetupNeeds.value) && localSetupNeeds.value.includes(card.id)) {
      card.disabled = false;
    }
  });
};

onMounted(() => {
  activateAllCards();
});

</script>

<template>
  <div class="grid grid-cols-1 md:grid-cols-3 gap-6 w-full max-w-6xl">
    <!-- Karte: Fenster Offen -->
    <CardComponent
        v-for="card in cards"
        :key="card.id"
        :icon="card.icon"
        :title="card.title"
        :description="card.description"
        :disabled="card.disabled"
        :loading="card.loading"
        @button-click="handleCalibration(card)"
    />
  </div>
</template>

<script>
export default {
  name: "SetupNeedsComponent",
};
</script>