<template>
  <v-app id="app">
    <v-main>
      <router-view/>
    </v-main>
  </v-app>
</template>

<script>

export default {
  name: 'App',

  data: () => ({
    //
  }),
}
</script>

<style lang="scss">
@import './assets/scss/base.scss';
@import './assets/scss/button.scss';
@import './assets/scss/table.scss';
@import './assets/scss/scroll.scss';
@import './assets/scss/modal.scss';
@import url("https://fonts.googleapis.com/icon?family=Gowun+Dodum");

#app {
    -webkit-font-smoothing: antialiased;
    font-family: 'Gowun Dodum';
    text-align: center;
    width: 100%;
    height: 100%;
}
</style>