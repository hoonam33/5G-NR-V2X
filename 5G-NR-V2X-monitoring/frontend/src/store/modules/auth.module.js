import authService from "../../services/auth.service";

export const auth = {
    state: {
        user: {user_id: null, user_role: null, accessToken: null}
    },
    actions: {
        login({ commit }, user) {
            return authService.login(user).then(
                user => {
                    commit('loginSuccess', user);
                    return Promise.resolve(user);
                },
                error => {
                    throw error;
                    //console.log(error)
                }
            )
        },
        logout({state}) { 
            state.user = {user_id: null, user_role: null, accessToken: null};
        }
    },
    mutations: {
        loginSuccess(state, user) {
            state.user = user;
            state.user.user_role = user.user_role.trim();
        },
    }, 
    getters: {
        getUser: (state) => {
            return state.user;
        },
        getLoggedin: (state) => {
            if(state.user.accessToken)
                return true;
            else
                return false;
        }
    },
    
};