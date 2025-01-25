<script setup>

import Spinner from "@/components/Spinner.vue";
</script>

<template>
  <div
      :class="[
      'bg-white dark:bg-gray-800 shadow-lg rounded-xl p-6 flex flex-col h-full transform transition-all duration-300',
      disabled ? 'hover:-translate-y-0' : 'hover:shadow-2xl hover:-translate-y-2'
    ]"
  >
    <!-- Icon -->
    <div class="text-blue-500 dark:text-blue-400 flex justify-center mb-6">
      <component :is="icon" class="h-16 w-16"></component>
    </div>

    <!-- Header und Text -->
    <div class="flex flex-col flex-grow justify-between text-center">
      <h2 class="text-2xl font-bold mb-2 dark:text-white">{{ title }}</h2>
      <p
          v-if="!loading"
          class="text-gray-600 dark:text-gray-300 mb-6"
      >
        {{ description }}
      </p>
      <div
          v-else
          class="flex mb-6 text-center text-red-500 font-semibold justify-center items-center"
      >
        Bitte warten, die Kalibrierung läuft...
      </div>
      <button
          @click="emitClick"
          :disabled="disabled"
          class="flex justify-center items-center px-6 py-3 bg-blue-500 hover:bg-blue-600 text-white font-semibold rounded-lg shadow-md dark:bg-blue-700 dark:hover:bg-blue-800 transition-transform transform hover:scale-105
          disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:scale-100"
      >
        <Spinner v-if="loading" :size="'w-6 h-6'" class="mx-auto" />

        <span v-else>{{ buttonText }}</span>
      </button>
    </div>
  </div>
</template>

<script>
export default {
  name: "CardComponent",
  props: {
    icon: {
      type: Object,
      required: true,
    },
    title: {
      type: String,
      required: true,
    },
    description: {
      type: String,
      required: true,
    },
    buttonText: {
      type: String,
      default: "Kalibrierung starten",
    },
    disabled: {
      type: Boolean,
      default: false
    },
    loading: {
      type: Boolean,
      default: false
    }
  },
  methods: {
    emitClick() {
      this.$emit("button-click");
    },
  },
};
</script>