<template>
    <div class="page login-background">
         <div class="container">
            <h2> CAN TEST PROGRAM <small>Login</small></h2>
            <div style="margin: 0px 30px 10px 30px" >
                <form>
                    <div class="group">      
                        <input type="text" v-model="state.user.user_id" required>
                        <span class="bar"></span>
                        <label>Name</label>
                    </div>
                    <div class="group">      
                        <input type="password"  v-model="state.user.user_pw" v-on:keyup.enter="handleLogin" required>
                        <span class="bar"></span>
                        <label>Password</label>
                    </div>                                                                                                                                                   
                </form>   
                <button class="btn-block-fill" style="margin-top: 20px" @click="handleLogin">
                    ENTER
                </button>     
                <div @click="collectIP" :disabled="!state.loading">
                    <a v-if="!state.loading" >
                        COLLATING IP (Retry with a click)
                    </a>
                </div>  
                <v-alert
                    prominent
                    type="error"
                    variant="outlined"
                    v-if="state.alert.show"
                    style="margin-top:20px">
                   {{state.alert.message}}
                </v-alert>           
            </div>
        </div>
    </div>
</template>

<script>
import { useStore } from 'vuex' 
import { useRouter } from 'vue-router'
import { onMounted, reactive } from 'vue'
import $ from 'jquery'

export default {
  setup() {
    const store = useStore();
    const router = useRouter();
    const state = reactive ({ 
        user: {user_id: undefined, user_pw: undefined},
        loading: false,
        alert: {
            show: false,
            message: null
        }
    })

    onMounted(async() => {
        //최초 페이딩 로딩시 사용자 IP를 수집함
        try {
            await collectIP();
        } catch (err){
            console.log(err);
        }
    })

    const collectIP = async() => {
        $.getJSON('https://api64.ipify.org?format=json', function(data) {
            state.user['user_ip'] = JSON.parse(JSON.stringify(data, null, 2)).ip;
        }).then(()=>{
            state.loading = true;
        }).catch(error => console.log(error));
    }
   
    //로그인 버튼 클릭
    const handleLogin = () => {
        if (state.user.user_id && state.user.user_pw) {
            store.dispatch('login', state.user).then(
                () => {
                    //상위 폴더 router/index.js 를 참고.....
                    let role = store.state.auth.user.user_role;
                    if(!role)
                        return;
                    
                    if(role == '관리자') {
                        router.push('/admin'); 
                    }
                    if(role == '사용자') {                    
                        router.push('/canview');    
                    }
                },
                error => {
                    state.alert = {
                        show: true,
                        message: error.message 
                    }
                }
            );
        } else {
            state.alert = {
                show: true,
                message: '입력값이 잘못되었습니다.'
            }
        } 
    }

    return {
      state, handleLogin, collectIP
    }
  }
}
</script>

<style lang="scss" scoped>
.login-background {
    background: url("../assets/image/login.jpg");
    background-size : cover;
    z-index:-1;
    background-position: center center;
    overflow-y: auto
}


@keyframes glowing {
    0% { background-color: $default-shade-30; box-shadow: 0 0 5vh $primary-30; }
    50% { background-color: $default-shade-30; box-shadow: 0 0 10vh $primary-20; }
    100% { background-color: $default-shade-30; box-shadow: 0 0 5vh $primary-30; }
} 

.container {
    position:relative; 
    float:left;
    width:40vh; 
    display:block; 
    backdrop-filter: blur(1vh);
    top: 50%;
	left: 50%;
	margin: -20vh 0 0 -20vh;
    padding: 1.7vh;
    animation: glowing 1500ms infinite;
    border-radius: 3vh;
}

h2 { 
    text-align:center; 
    font-size: 2.5vh;
    color: $primary-80; 
}
h2 small { 
    font-weight:normal; 
    color:$grey-1; 
    display:block; 
}

/* form starting stylings ------------------------------- */
.group { 
    position:relative; 
    margin-top: 2.8vh; 
}
input {
    font-size: 1.8vh;
    padding: 1.2vh 1.2vh 1.2vh 1vh;
    display: block;
    width: 100%;
    border: none;
    background: transparent;
    border-bottom: 1px solid $grey-1;
    color: $grey-1
}
input:focus { 
    outline:none; 
}

/* LABEL ======================================= */
label 				 {
    color: $grey-8; 
    font-weight: normal;
    position: absolute;
    pointer-events: none;
    left: 5px;
    font-size: 2vh;
    top: 1vh;
    transition: 0.2s ease all; 
    -moz-transition: 0.2s ease all; 
    -webkit-transition: 0.2s ease all; 
}

/* active state */
input:focus ~ label, input:valid ~ label 		{
  top: -2.3vh;
  font-size: 1.6vh;
  color: $primary-50;
}

/* BOTTOM BARS ================================= */
.bar 	{ position:relative; display:block; width:100%; }
.bar:before, .bar:after {
  content:'';
  height:0.2vh; 
  width:0;
  bottom:1px; 
  position:absolute;
  background: $primary-50; 
  transition:0.2s ease all; 
  -moz-transition:0.2s ease all; 
  -webkit-transition:0.2s ease all;
}
.bar:before {
  left:50%;
}
.bar:after {
  right:50%; 
}

/* active state */
input:focus ~ .bar:before, input:focus ~ .bar:after {
  width:50%;
}

.alert {
    border: 1px solid rgba(241, 6, 6, 0.81);
    background-color: rgba(73, 5, 1, 0.342);
    box-shadow: 0px 0px 2px #ff0303;
    color: #ff0303;
    text-shadow: 2px 1px #00040a;
    transition:0.5s;
    cursor:pointer;
    
    .alert-dismissible .btn-colse {
        color: white;
    }
}

</style>