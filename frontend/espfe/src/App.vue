<script setup>
import CardComponent from "@/components/CardComponent.vue";
import IconWindowOpen from "@/components/icons/IconWindowOpen.vue";
import IconWindowMid from "@/components/icons/IconWindowMid.vue";
import IconWindowClosed from "@/components/icons/IconWindowClosed.vue";
import Spinner from "@/components/Spinner.vue";
import SetupCompleteComponent from "@/components/SetupCompleteComponent.vue";
import SetupNeedsComponent from "@/components/SetupNeedsComponent.vue";
</script>

<template>
  <div class="bg-gray-100 dark:bg-gray-900 text-black dark:text-white flex flex-col items-center justify-center min-h-screen space-y-8 px-4">
    <div class="text-center">
      <h1 class="text-4xl font-extrabold">Window Guardian Setup</h1>
      <p class="text-lg text-gray-600 dark:text-gray-300 mt-2">
        Richten Sie Ihren Window Guardian sicher und schnell ein. Befolgen Sie die Anweisungen, um das Fenster zu kalibrieren.
      </p>
    </div>

    <Spinner v-if="loading" :size="'w-12 h-12'"/>

    <div v-else>

      <SetupCompleteComponent v-if="isSetupEmpty" />

      <SetupNeedsComponent v-else :setup-needs="setupNeeds"/>

    </div>

  </div>
</template>

<script>
export default {
  name: "WindowGuardianSetup",
  data() {
    return {
      loading: true,
      setupNeeds: [],
      error: ""
    }
  },
  computed: {
    isSetupEmpty() {
      return this.setupNeeds.length === 0;
    },
  },
  mounted() {
    this.fetchNeeds();
  },
  methods: {
    handleStep(step) {
      console.log(`Kalibrierung für Zustand ${step} gestartet.`);
    },
    async fetchNeeds() {
      try {
        const response = await fetch("/api/setup-needs", {
          method: "GET",
        });

        if (!response.ok) {
          throw new Error(`Fehler beim Abrufen: ${response.status}`);
        }

        this.setupNeeds = (await response.json())["needs"];

      } catch (err) {
        this.error = err.message;
      } finally {
        this.loading = false;
      }
    }
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