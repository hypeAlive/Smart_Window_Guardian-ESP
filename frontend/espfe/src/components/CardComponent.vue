<script setup>

</script>

<template>
  <div
      class="bg-white dark:bg-gray-800 shadow-lg hover:shadow-2xl hover:-translate-y-2 duration-300 rounded-xl p-6 flex flex-col h-full transform transition-all">
    <!-- Icon -->
    <div class="text-blue-500 dark:text-blue-400 flex justify-center mb-6">
      <component :is="icon" class="h-16 w-16"></component>
    </div>
    <!-- Header und Text -->
    <div class="flex flex-col flex-grow justify-between text-center">
      <h2 class="text-2xl font-bold mb-2 dark:text-white">{{ title }}</h2>
      <p class="text-gray-600 dark:text-gray-300 mb-6">
        {{ description }}
      </p>
      <!-- Button -->
      <button
          @click="onClick"
          class="mt-auto px-6 py-3 bg-blue-500 hover:bg-blue-600 text-white font-semibold rounded-lg shadow-md dark:bg-blue-700 dark:hover:bg-blue-800 transition-transform transform hover:scale-105">
        {{ buttonText }}
      </button>
    </div>
  </div>
</template>

<script>
export default {
  name: "CardComponent",
  props: {
    icon: {
      type: Object, // Erwartet ein Vue-Icon-Komponente
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
      default: "Weiter",
    },
  },
  methods: {
    onClick() {
      this.$emit("click");
    },
  },
};
</script>