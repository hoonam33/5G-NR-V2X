<script>
import { onMounted, reactive } from '@vue/runtime-core'
import { useStore } from 'vuex'
import { dateFormat } from '../utils/calender.util'
import * as yup from 'yup'
import HeaderView from '@/components/Common/HeaderView.vue'

export default {
    components: {
        HeaderView
    },
    setup() {
        const store = useStore()
        const state = reactive({
            //users table 
            users: [],
            userLog: [],
            //delete
            deleteDialog: {
                show: false,
                delete_id: null
            },
            //register
            registerDialog: {
                show: false,
                userId: '',
                userPw: '',
                userRole: null,
                teamId: null,
            },
            roleOption: [
                {text: 'Choose Role', value: null},
                {text: '관리자', value: 1},
                {text: '사용자', value: 2}
            ]
        })

        onMounted(() => {
            getUsers();
            detailUser(null);
        })

        //모든 사용자 업데이트
        const getUsers = () => {
            store.getters.getUsers.then(
                response => {
                    state.users = response.data.userList; 
                }/*, 
                error => {
                }*/
            );
        }

        //yup으로 필수 항목들을 정의
        const schema = yup.object().shape({
           user_id: yup.string().required(),
           user_pw: yup.string().required(),
           user_role: yup.number().required().positive().integer(),
        });
        //회원가입
        const register = async() => {
            let user = {
                user_id: state.registerDialog.userId,
                user_pw: state.registerDialog.userPw,
                user_role: state.registerDialog.userRole,
            }
            const isValid = await schema
            .isValid(user)

            if(!isValid)  {
                return;
            }

            //schema 체크에서 통과하면 회원 등록을 수행함
            store.dispatch('register', user)
             /* eslint no-unused-vars: ["error", { "args": "none" }] */
            .then(response => {
                //회원등록이 정상적으로 완료되면 사용자 리로딩
                getUsers();
                state.registerDialog.show = false;
            }, error => {
                //console.log(error)
            })
            
        }

        // const deleteUser = () => {
        //     store.dispatch('deleteUser', state.deleteDialog.delete_id).then(
        //         () => {
        //             state.deleteDialog.delete_id = null;
        //             state.deleteDialog.show = false;
        //             getUsers();
        //         }, 
        //         error => {
        //             getUsers();
        //         }
        //     ); 
        // }

        //사용자 로그 뷰
        const detailUser = (user_id) => {
            store.getters.getUserLog(user_id).then(
                response => {
                    state.userLog = Object.values(response.data.userLogList);
                }, 
                error => {
                }
            );
        }
        //return { state, store, register, deleteUser, detailUser, dateFormat }
        return { state, store, register, dateFormat, detailUser }
    }
}
</script>

<template>
    <div class="page">
        <HeaderView style="height: 7vh"/>
        
        <v-container fluid>
            <v-row>
                <v-col style="height: 85vh">
                    <v-card style="height: 100%; overflow-y: auto; " >
                        <button id="myBtn" class="btn-block-empty" @click="state.registerDialog.show=!state.registerDialog.show">Register</button>
                        <table class="table">
                            <tbody>
                                <tr>
                                    <td class="table-title">user_id</td>
                                    <td class="table-title">user_role</td>
                                    <td class="table-title">actions</td>
                                </tr>

                                <tr v-for="(user) in state.users" :key="user">
                                    <td>{{user.user_id}}</td>
                                    <td>{{state.roleOption.find(x=>x.value===user.user_role).text}}</td>
                                    <td style="padding:0px"> 
                                        <button id="myBtn" class="btn-clear" style="padding:0px; height:100%; width: 50%;" 
                                        :style="{ visibility: user.user_role == 1 ? 'hidden' : 'visible'}"
                                        @click="state.deleteDialog.show=!state.deleteDialog.show;
                                        state.deleteDialog.delete_id=user.user_id">DELETE</button>
                                        <button id="myBtn" class="btn-clear" style="padding:0px; height:100%; width: 50%;" @click="detailUser(user.user_id)">LOG</button>
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </v-card>
                </v-col>

                <v-col style="height: 85vh">
                    <v-card style="height: 100%; overflow-y: auto;">
                        <!-- <button id="myBtn" class="btn-block-empty" @click="detailUser(null)">DEFAULT</button> -->
                        <button id="myBtn" class="btn-block-empty">DEFAULT</button>
                        <table class="table-clear">
                            <tbody>
                                    <tr>
                                    <td class="table-title">user_id</td>
                                    <td class="table-title">user_ip</td>
                                    <td class="table-title">user_status</td>
                                    <td class="table-title">time</td>
                                </tr>

                                <tr v-for="(userLog) in state.userLog" :key="userLog">
                                    <td>{{userLog.user_id}}</td>
                                    <td>{{userLog.user_ip}}</td>
                                    <td>{{userLog.user_status}}</td>
                                    <td> 
                                        {{dateFormat(new Date(userLog.created_time), "Y.m.d H:i:s.v초\n")}}
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </v-card>
                </v-col>
            </v-row>
        </v-container>

        <!-- register dialog -->
        <v-dialog
            v-model="state.registerDialog.show"
        >
            <v-card class="dialog-card">
                <div class="modal-header">
                        <span class="modal-title">REGISTERATION</span>
                    </div>

                    <v-container>
                        <v-row>
                            <v-col>
                                <label for="inputId">Id</label>
                                <input type="text" v-model="state.registerDialog.userId" id="inputId" style="background-color: black;">
                            </v-col>
                        </v-row>
                        <v-row>
                            <v-col>
                                <label for="inputPassword">Password</label>
                                <input type="password" v-model="state.registerDialog.userPw" id="inputPassword" style="background-color: black;">
                            </v-col>
                        </v-row>
                        <v-row>
                            <v-col sm="5">
                                <label for="inputState" style="margin: 0px 20px 0px 0px">Role</label>
                                <select id="inputState" v-model="state.registerDialog.userRole" style="width: 150px; background-color: black;">
                                    <option v-for="option in state.roleOption" v-bind:key="option.value" v-bind:value="option.value">
                                        {{ option.text }}
                                    </option>
                                </select>
                            </v-col>
                        </v-row>
                        <v-row>
                            <v-col sm="6" style="padding-right:0px !important">
                                <button @click="register()" style="margin: 20px 0px 10px 0px" class="btn-block-empty okay">Sign in</button>
                            </v-col>
                            <v-col sm="6" style="padding-left:0px !important">
                                <button @click="state.registerDialog.show = false" style="margin: 20px 0px 10px 0px" class="btn-block-empty cancle">Close</button>
                            </v-col>
                        </v-row>
                    </v-container>
            </v-card>
        </v-dialog>
        <!-- ./register dialog -->

        <!-- delete dialog -->
        <v-dialog
            v-model="state.deleteDialog.show"
        >
            <v-card class="dialog-card">
                <div class="modal-header">
                    <span class="modal-title">DELETE</span>
                </div>
                <v-container>
                    <v-row>
                        <v-col>
                            Delete User : {{state.deleteDialog.delete_id}}
                        </v-col>
                    </v-row>
                    <v-row>
                        <v-col sm="6" style="padding-right:0px !important">
                            <button @click="deleteUser()" style="margin: 20px 0px 10px 0px" class="btn-block-empty okay">Delete</button>
                        </v-col>
                        <v-col sm="6" style="padding-left:0px !important">
                            <button @click="state.deleteDialog.show = false" style="margin: 20px 0px 10px 0px" class="btn-block-empty cancle">Close</button>
                        </v-col>
                    </v-row>
                </v-container>
            </v-card>
        </v-dialog>
        <!-- ./delete dialog -->
	</div>
</template>


<style lang="scss" scoped>
#admin-background {
    height: 100%;
    background-color: $default-shade;
}

#myModal {
    top:40px;
    width: 400px;
    left:50%;
    margin-left: -200px;
    z-index:2050;
}

#admin-alert-modal {
    top:40px;
    width: 400px;
    left:50%;
    margin-left: -200px;
    z-index:2050;
}

</style>