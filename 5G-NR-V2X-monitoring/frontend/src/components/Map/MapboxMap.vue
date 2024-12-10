<template>
    <div class="map_container">
        <div id="map" />
        <CanDataList />
    </div>
</template>

<script>
import { onMounted, reactive } from 'vue'
import { useStore } from 'vuex'
import mapboxgl from 'mapbox-gl'
import 'mapbox-gl/dist/mapbox-gl.css'
import CanDataList from '@/components/CAN/CanDataList.vue'

export default {
    components: {
        CanDataList
    },
    setup() {
        let mapVars = null
        const store = useStore()

        const state = reactive({

        })

        onMounted(() => {
            mapboxgl.accessToken = 'pk.eyJ1Ijoia2tkc2VvIiwiYSI6ImNtMzZ2ejdwODA5emwycW9sdDR6aHhnODUifQ.loCjM4YChROp0E8YZwmUng'

            mapVars = store.getters['getMap']()
            mapVars.map = new mapboxgl.Map({
                container: 'map',
                style: 'mapbox://styles/kkdseo/cm36xtxd8004z01r833usef81',
                center: [128.403167, 35.646021],
                zoom: 14,
                pitch: 0,
                bearing: 0,
                antialias: true,
                attributionControl: false,
                fadeDuration: 0
                // MaxBounds // 렌더링 영역 제한
            })

            //map.triggerRepaint(); 지도 강제 다시 렌더링

            mapVars.map.on('load', () => {
                mapVars.map.setFog({
                    range: [8, 9],
                    'horizon-blend': 0.3,
                    color: 'white',
                    'high-color': '#add8e6',
                    'space-color': '#d8f2ff',
                    'star-intensity': 0,
                })

                const PG_B2 = require('@/assets/hdmap/pg_b2_surfacelinemark_240805.geojson')
                DrawB2SURFACELINE(PG_B2)
            })
        })

        const DrawA2Link = (a2geojson) => {

        }

        const DrawB2SURFACELINE = (b2geojson) => {
            if (mapVars.map) {
                if (mapVars.map.getLayer(b2geojson.name + '_layer')) {
                    mapVars.map.removeLayer(b2geojson.name + '_layer')
                    mapVars.map.removeSource(b2geojson.name + '_source')
                }

                mapVars.map.addSource(b2geojson.name + '_source', {
                    type: 'geojson',
                    data: b2geojson
                })

                mapVars.map.addLayer({
                    id: b2geojson.name + '_layer',
                    type: 'line',
                    source: b2geojson.name + '_source',
                    layout: {
                        'line-join': 'round',
                        'line-cap': 'round'
                    },
                    paint: {
                        'line-color': [
                            'match',
                            ['get', 'Type'],
                            ['111', '112', '113', '114', '121', '123', '124'],
                            '#ffcc00',
                            ['311', '312', '313', '314', '321', '322', '323', '324'],
                            '#0066ff',
                            '#ffffff',
                        ],
                        'line-dasharray': [
                            'match',
                            ['get', 'Type'],
                            ['112', '122', '212', '222', '312', '322'],
                            ['literal', [5, 5]],
                            ['literal', [1, 0]],
                        ],
                        'line-width': [
                            'match',
                            ['get', 'Type'],
                            [
                                '121',
                                '122',
                                '123',
                                '124',
                                '221',
                                '222',
                                '223',
                                '224',
                                '321',
                                '322',
                                '323',
                                '324',
                            ],
                            ['literal', 2],
                            ['literal', 1],
                        ],
                    }
                })
            }
        }

        return { state, store }
    }
}
</script>

<style lang="scss" scoped>
#map {
    position: absolute;
    left: 0;
    right: 0;
    bottom: 0;
    top: 0;
    width: 100%;
    height: 100%;
}

.mapContainer {
    position: relative;
    overflow: hidden;
}
</style>