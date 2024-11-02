import {createRouter, createWebHistory} from 'vue-router';
import Settings from '../views/Settings.vue';

const routes = [
    {path: '/', redirect: '/settings'},
    {path: '/settings', component: Settings},
];

const router = createRouter({
    history: createWebHistory(),
    routes,
});

export default router;
