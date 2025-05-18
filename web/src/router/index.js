import {createRouter, createWebHashHistory} from 'vue-router';
import Settings from '../views/Settings.vue';
import TextFormatter from '../views/TextFormatter.vue';

const routes = [
    {path: '/', redirect: '/settings'},
    {path: '/settings', component: Settings},
    {path: '/text-formatter', component: TextFormatter},
];

const router = createRouter({
    history: createWebHashHistory(),
    routes,
});

export default router;
