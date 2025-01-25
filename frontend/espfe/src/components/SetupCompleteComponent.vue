<script setup>
import {ref} from "vue";
import Spinner from "@/components/Spinner.vue";

const loading = ref(false);

const handleReset = async () => {
  loading.value = true;

  try {
    const response = await fetch("/api/reset", {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
      },
    });

    if (!response.ok) {
      throw new Error(`Fehler bei der Anfrage: ${response.statusText}`);
    }

    console.log("Zurücksetzen erfolgreich");
    window.location.reload();
  } catch (error) {
    console.error("Fehler beim Zurücksetzen:", error);
  } finally {
    loading.value = false;
  }
};
</script>

<template>
  <h1 class="text-2xl font-bold mb-6">
    Gerät bereits eingerichtet
  </h1>
  <button
      class="flex items-center mx-auto justify-center px-6 py-3 bg-blue-500 text-white font-semibold rounded-lg shadow-md hover:bg-blue-600 disabled:opacity-50 disabled:cursor-not-allowed"
      :disabled="loading"
      @click="handleReset"
  >
    <span v-if="!loading">Zurücksetzen</span>
    <Spinner v-if="loading" :size="'w-6 h-6'" class="mx-auto" />
  </button>
</template>