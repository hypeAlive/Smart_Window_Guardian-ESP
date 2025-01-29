<script setup lang="ts">
import CardComponent from "@/components/CardComponent.vue";
import IconWindowMid from "@/components/icons/IconWindowMid.vue";
import IconWindowClosed from "@/components/icons/IconWindowClosed.vue";
import IconWindowOpen from "@/components/icons/IconWindowOpen.vue";
import { useWindowGuardianStore } from "@/stores/windowGuardian";
import { onMounted, onBeforeUnmount } from "vue";

const store = useWindowGuardianStore();

const getIcon = (state: string) => {
  switch (state) {
    case "ON":
      return IconWindowOpen;
    case "OFF":
      return IconWindowClosed;
    case "MID":
      return IconWindowMid;
    default:
      return null;
  }
};

onMounted(() => {
  store.startFetching(10000);
});

onBeforeUnmount(() => {
  store.stopFetching();
});

</script>

<template>
  <div class="bg-gray-100 dark:bg-gray-900 text-black dark:text-white flex flex-col items-center justify-center min-h-screen space-y-16 px-6 sm:px-8 lg:px-12">
    <!-- Header -->
    <div class="text-center space-y-4">
      <h1 class="text-4xl font-extrabold tracking-tight">Window Guardian</h1>
      <p class="text-lg text-gray-600 dark:text-gray-300 leading-relaxed">
        Behalten Sie den Überblick über den Zustand all Ihrer Fenster – für mehr Sicherheit und ein gutes Gefühl, egal wo Sie sind.
      </p>
    </div>

    <!-- Grid mit Card-Komponenten -->
    <div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-8 w-full max-w-7xl">
      <CardComponent
          v-for="(state, index) in store.windowStates"
          :key="index"
          :icon="getIcon(state.saveState)"
          :name="state.id"
          :ip="state.ip"
          :state="state.saveState"
      />
      <CardComponent
          v-for="(state, index) in store.ips"
          :key="index"
          :icon="null"
          name="Window"
          :ip="state"
      />
    </div>

    <!-- Beschreibung und Einrichtung -->
    <div class="text-center space-y-6">
      <h2 class="text-2xl font-semibold tracking-normal">Einrichtung Ihres Window Guardians</h2>
      <p class="text-md text-gray-600 dark:text-gray-300 leading-relaxed max-w-2xl mx-auto">
        Verbinden Sie Ihren <span class="font-semibold">Window Guardian</span> mit dem Internet und konfigurieren Sie ihn bequem hier in dieser Anwendung.
      </p>
    </div>
  </div>
</template>

<style scoped>
</style>