<template>
    <div class="fecth">
        <v-container fluid style="padding:0px; height: 100%">
            <v-row dense style="height: 100%;">
                <v-col :cols="12">
                    <v-col :cols="12" class="time-text" style="display: table; padding: 0px">
                        <span class="fetch" style="display: table-cell;
                                vertical-align: bottom;">
                            {{ state.currentTime }}
                        </span>
                    </v-col>
                    <v-col :cols="12" class="fetch-text" style="padding: 0px">
                        <button @click="state.useAutoFetch = !state.useAutoFetch">fetch</button> {{ state.fetchTakenTime }} ms
                    </v-col>
                </v-col>
            </v-row>
        </v-container>
    </div>
</template>

<script>
import { reactive, onMounted, computed } from "vue"
import { useStore } from 'vuex'
import { dateFormat, serverTime } from '@/utils/calender.util.js'

export default {
    setup() {
        const store = useStore();

        const state = reactive({
            lastTime: null,
            milli_count: 0,
            milli_array: [],
            temp_milli_array: null,
            currentTime: null,
            useAutoFetch: false,
            fetchTakenTime: 0,
            timerId: null,
        });

        onMounted(() => {
            clickAutoFetch()
        })

        const clickAutoFetch = () => {
            state.useAutoFetch = !state.useAutoFetch;
            if (!state.useAutoFetch)
                clearInterval(state.timerId);
            if (state.useAutoFetch)
                state.timerId = Object.freeze(setInterval(process, 200));
        }

        const process = async() => {
            if(!state.useAutoFetch) 
                return;
            
            var st = serverTime();
            let fetchTime = new Date(st);

            var milli = new Date().getTime() % 1000;

            // 시간보정
            if(state.lastTime) {
                if(fetchTime.getSeconds() != new Date(state.lastTime).getSeconds()) {
                    state.milli_count = 0;
                }
                else if(state.milli_count < 4){
                    state.milli_count += 1;
                } 

                if(state.milli_array.length == 5) fetchTime.setMilliseconds(state.milli_array[state.milli_count]);
                else if(state.temp_milli_array) fetchTime.setMilliseconds(state.temp_milli_array[state.milli_count]);
            }
            if(state.milli_array.length < 5) {
                state.milli_array.push(milli);
                if(state.milli_array.length == 5) {
                    state.milli_array.sort(function(a,b) {
                      return a - b;
                    });
                }
            } else if(state.milli_array.length == 5 && isNaN(fetchTime.getTime())) {
                state.temp_milli_array = state.milli_array.slice();
                state.milli_array = [];
                fetchTime = new Date(st);
                fetchTime.setMilliseconds(state.temp_milli_array[state.milli_count]);
            }
            state.lastTime = fetchTime.getTime();

            state.currentTime = dateFormat(fetchTime, "Y-m-d H:i:s");
            state.fetchTakenTime = fetchTime.getTime() - store.getters.getFetchTime.getTime();
            
            store.commit('setFetchTime', fetchTime);
        }

        return { state, clickAutoFetch }
    }
}
</script>


<style lang="scss" scoped>
.fecth {
    position: absolute;
    font-size: 1.6vh;
    text-align: right;
    width: 25vw;
    height: 100%;
    top: 0px;
    right: 0px;
    z-index: 1;
    color: rgb(88, 117, 122);
    margin-right: 15px;

    .time-text {
        color: rgba(255, 255, 255, 0.801);
        font-size: 2.2vh;
        font-weight: bold;
    }

    .fetch-text {
        color: rgba(255, 255, 255, 0.445);
        font-size: 2vh;
    }
}

.contest-status {
    width: 100%;
    height: 100%;
    margin: 0.2vh 0.5vh;
    border-radius: .25em;
    //background-color:$primary-shade-70;
    background-image: linear-gradient(to bottom, 
                    rgba($primary-shade-70, 1.0), 
					rgba($primary-shade-70, 0.5), 
                    rgba($primary-shade-50, 0.1)
				);
    text-align: center;
    .title {
        width:100%;
        font-size: 1.7vh;
        background-color: $primary-50;
        border-radius: .25em .25em 0em 0em;
        color:rgba(255, 255, 255, 0.6);
    }
    .content {
        font-size: 2.4vh;
        color:rgba(255, 255, 255, 0.8);
    }
}
</style>