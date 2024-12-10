import { createRouter, createWebHashHistory } from 'vue-router'
import LoginPage from '../views/LoginPage.vue'
import AdminPage from '../views/AdminPage.vue'
import CanViewPage from '../views/CanViewPage.vue'

const routerHistory = createWebHashHistory();

const router = createRouter({
    history: routerHistory,
    routes: [
        {
            path: '/',
            component: LoginPage
        },
        {
            path: '/admin',
            component: AdminPage
        },
        {
            path: '/canview',
            component: CanViewPage
        }
    ]
})

export default router