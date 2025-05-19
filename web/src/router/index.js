import {createRouter, createWebHashHistory} from 'vue-router';
import Settings from '../views/Settings.vue';
import TextFormatter from '../views/TextFormatter.vue';
import BaseConverter from '../views/BaseConverter.vue';
import TimeConverter from '../views/TimeConverter.vue';

const routes = [
    {path: '/', redirect: '/settings'},
    {path: '/settings', component: Settings},
    {path: '/text-formatter', component: TextFormatter},
    {path: '/base-converter', component: BaseConverter},
    {path: '/time-converter', component: TimeConverter},
];

const router = createRouter({
    history: createWebHashHistory(),
    routes,
});

export default router;
