<template>
  <div
      :class="['border-4 border-blue-500 border-solid rounded-full animate-spin border-t-transparent', sizeClasses]"
  ></div>
</template>

<script>
export default {
  name: "Spinner",
  props: {
    size: {
      type: String,
      default: "w-5 h-5",
    },
  },
  computed: {
    sizeClasses() {
      return this.size;
    },
  },
};
</script>