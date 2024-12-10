<template>
    <div>
        <HeaderView style="height: 7vh" />

        <v-container fluid id="container">
            <div class="map">
                <MapboxMap />
            </div>
        </v-container>
    </div>
</template>

<script>
import { onMounted, reactive } from '@vue/runtime-core'
import { useStore } from 'vuex'
import HeaderView from '@/components/Common/HeaderView.vue'
import MapboxMap from '@/components/Map/MapboxMap.vue'

export default {
    components: {
        HeaderView,
        MapboxMap,
    },
    setup() {
        const store = useStore()
        const state = reactive({

        })

        onMounted(() => {

        })

        return { store, state }
    }
}
</script>

<style lang="scss" scoped>
#container {
    position: relative;
    height: 93vh;
    width: 100%;
    overflow: hidden;
}
.map {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
}
</style>