import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import naive from 'naive-ui'
import router from './router'
import { setMessage } from './utils/request'

const app = createApp(App)
app.use(router)
app.use(naive)

// 设置message实例
const message = app.config.globalProperties.$message
setMessage(message)

app.mount('#app')