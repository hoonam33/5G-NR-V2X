import { createStore } from 'vuex';
import { api } from './modules/api.module';
import { admin } from './modules/admin.module';
import { auth } from './modules/auth.module';
import { map } from './modules/map.module';

export default createStore({
    modules: {
        api, admin, auth, map
    }
})