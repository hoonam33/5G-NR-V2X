import axios from 'axios';
import authHeader from '../header/auth.header';

const API_URL = process.env.VUE_APP_API_URL + '/admin/';

class adminService {
    register(user) {
        return axios.
        post(API_URL + 'register', {
            user_id: user.user_id,
            user_pw: user.user_pw,
            user_role: user.user_role,
        }, {
            headers: authHeader()
        })
    }
    getUsers() {
        return axios.
        post(API_URL + 'get_users',{},{
            headers: authHeader()
        })
    }
    deleteUser(user_id) {
        return axios.
        post(API_URL + 'delete_user',{
            user_id: user_id
        },{
            headers: authHeader()
        })
    }
    getUserLog(user_id) {
        return axios.
        post(API_URL + 'get_user_log',{
            user_id: user_id
        },{
            headers: authHeader()
        })   
    }
}

export default new adminService();
