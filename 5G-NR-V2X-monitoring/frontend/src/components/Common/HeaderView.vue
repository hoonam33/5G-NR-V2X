<script>
import { computed, reactive } from '@vue/runtime-core';
import { useStore } from 'vuex'
import { useRouter } from 'vue-router'
import ApiView from '@/components/Common/ApiView.vue'

export default {
    components: {
        ApiView
    },
    setup() {
        const store = useStore();
        const router = useRouter();
        const state = reactive({
            //curUser: computed(() => store.state.auth.user),
            curUser: {
                user_role: '관리자'
            }
        });

        const logout = () => {
            store.dispatch('logout').then(
                () => {
                    router.push('/');
                },
                error => {
                    console.log(error);
                    state.message =
                        (error.response && error.response.data) ||
                        error.message ||
                        error.toString();
                }
            );
        }

        return { state, store, logout }
    }
}
</script>

<template>
    <div class="btn-4">
        <label class="octo-label">
            <a>
                CAN TEST
            </a>
            <a v-if="state.curUser.user_role == '관리자'">
                관리자 UI
            </a>
            <a v-if="state.curUser.user_role == '사용자'">
                사용자 UI
            </a>
        </label>
        <ApiView />
    </div>
</template>

<style lang="scss" scoped>
.octo-label {
    font-size: 3vh;
    text-transform: uppercase;
    /*text-shadow: 1px 1px 20px transparentize($teal, 0.4);*/
    margin: 0;
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    animation: text-line-glow 2s linear infinite;
}

@keyframes text-line-glow {
    0% {
        text-shadow: 0px 0px 0px #29cac8;
    }

    50% {
        text-shadow: 0px 0px 4px #29cac8;
    }
}

$main-bg: #363159;
$secondary-bg: #202026;
$main-fg: #eee;
$primary: #05C7F2;
$primary-dark: #05AFF2;
$secondary: #D63BD9;
$secondary-dark: #7B6CD9;
$notchSize: 40px;

.card-2 {
    overflow: hidden;
    position: relative;
    //background: $primary-shade-50;
    background-image: linear-gradient(to left,
            rgba($primary-shade-50, 0),
            rgba($primary-shade-50, 0.5),
            rgba(rgb(12, 59, 129), 0.7),
            rgba($primary-shade-50, 0.5),
            rgba($primary-shade-50, 0));
    backdrop-filter: brightness(1.1) blur(4px);
    //box-shadow: inset 0 0 7px $primary, 0 0 7px $primary;
    border-top: 0.2vh solid $primary-30;
    border-bottom: 0.2vh solid $primary-30;

    .card_title {
        font-weight: 700;
        animation: text-glow 1s ease-in-out infinite alternate;
    }

    .separator {
        height: 1px;
        width: 100%;
        background-color: $primary;
        margin: 20px 0;
        box-shadow: 0 0 10px $primary;
    }

    .card_content {
        text-shadow: 0px 0px 5px rgba(255, 255, 255, .2);
    }
}

@keyframes text-glow {
    from {
        text-shadow: 0 0 1px $primary;
    }

    to {
        text-shadow: 0 0 7px $primary;
    }
}

.btn-4 {
    border: none;
    position: relative;
    background-image: linear-gradient(to left,
            rgba($primary-shade-50, 0),
            rgba($primary-shade-50, 0.5),
            rgba(rgb(12, 59, 129), 0.7),
            rgba($primary-shade-50, 0.5),
            rgba($primary-shade-50, 0));
    overflow: hidden;
}

.btn-4:before {
    content: "";
    position: absolute;
    display: inline-block;
    top: -180px;
    left: 0;
    width: 80px;
    height: 100%;
    background-color: rgba(rgb(12, 59, 129), 0.7);
    animation: shiny-btn1 3s ease-in-out infinite;
}

.btn-4:active {
    box-shadow: 4px 4px 6px 0 rgba(255, 255, 255, 0.3),
        -4px -4px 6px 0 rgba(116, 125, 136, 0.2),
        inset -4px -4px 6px 0 rgba(255, 255, 255, 0.2),
        inset 4px 4px 6px 0 rgba(0, 0, 0, 0.2);
}

@keyframes shiny-btn1 {
    0% {
        -webkit-transform: scale(0) rotate(45deg);
        opacity: 0;
    }

    80% {
        -webkit-transform: scale(0) rotate(45deg);
        opacity: 0.5;
    }

    81% {
        -webkit-transform: scale(4) rotate(45deg);
        opacity: 1;
    }

    100% {
        -webkit-transform: scale(50) rotate(45deg);
        opacity: 0;
    }
}
</style>