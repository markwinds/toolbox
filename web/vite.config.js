import {defineConfig} from 'vite'
import vue from '@vitejs/plugin-vue'
import dotenv from 'dotenv'

// 载入环境变量
dotenv.config()

// https://vite.dev/config/
export default defineConfig({
    base: '/web/', // 和部署有关系 url访问时带的base路径
    define: {
        'process.env': {} // 定义process.env变量，防止后续提示未定义
    },
    server: {
        host: process.env.VITE_LISTEN_IP,
        port: process.env.VITE_LISTEN_PORT,
        proxy: {
            // 请求的代理 解决跨域等问题
            [process.env.VITE_BASE_API]: {
                target: `${process.env.VITE_HOST}`,
                changeOrigin: true,
            },
        }
    },
    plugins: [vue()],
})
