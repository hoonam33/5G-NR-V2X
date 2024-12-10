import axios from 'axios';

const API_URL = process.env.VUE_APP_API_URL + '/auth/';

class authService {
    login(user) {
        return axios
        .post(API_URL + 'login', {
            user_id: user.user_id,
            user_pw: user.user_pw,
            user_ip: user.user_ip
        })
        .then(response => {
            return response.data
        })
    }
    logout() {
        localStorage.removeItem('user');
    }
}

export default new authService();