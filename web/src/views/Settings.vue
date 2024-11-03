<template>
  <n-space vertical :size="24" class="frame">

    <div>
      <div class="item-title">软件版本</div>
      <div class="item">当前版本: {{ currentVersion }}</div>
      <div class="item">最新版本: {{ latestVersion }}</div>
      <n-button @click="updateSoftware">更新</n-button>
    </div>

    <n-divider/>

    <div>
      <div class="item-title">设置项</div>
      <n-form>
        <n-form-item label="日志等级">
          <n-select v-model="settings.logLevel" :options="logLevelOptions"/>
        </n-form-item>
        <n-form-item label="数据存放目录">
          <n-input v-model="settings.dataDirectory"/>
        </n-form-item>
        <n-form-item label="Web 监听端口">
          <n-input-number v-model="settings.webPort"/>
        </n-form-item>
        <n-form-item label="代理地址">
          <n-input v-model="settings.proxyAddress"/>
        </n-form-item>
      </n-form>
    </div>

    <n-divider/>

    <div>
      <div class="item-title">操作按钮</div>
      <n-space>
        <n-button @click="exitProgram">退出程序</n-button>
        <n-button @click="restartProgram">重启程序</n-button>
        <n-button @click="saveAndRestart">保存参数并重启</n-button>
        <n-button @click="restoreDefaultsAndRestart">恢复默认参数并重启</n-button>
      </n-space>
    </div>

  </n-space>
</template>

<script setup>
import {ref} from 'vue'
import {useNotification, NSpace, NButton, NForm, NFormItem, NSelect, NInput, NInputNumber, NDivider} from 'naive-ui'
import {showInfo} from "../utils/notification.js"
import {showLoading, hideLoading} from "../utils/loading.js";

const notification = useNotification()

const currentVersion = ref('1.0.0')
const latestVersion = ref('1.1.0')
const settings = ref({
  logLevel: 'info',
  dataDirectory: '/path/to/data',
  webPort: 8080,
  proxyAddress: 'http://proxy.example.com',
})
const logLevelOptions = [
  {label: 'Debug', value: 'debug'},
  {label: 'Info', value: 'info'},
  {label: 'Warn', value: 'warn'},
  {label: 'Error', value: 'error'},
]

function updateSoftware() {
  showInfo(notification, "good")

//   // 显示加载遮罩
//   showLoading();
//
// // 模拟加载过程，3秒后移除加载遮罩
//   setTimeout(hideLoading, 3000);
}

function exitProgram() {
  alert('正在退出程序...')
}

function restartProgram() {
  alert('正在重启程序...')
}

function saveAndRestart() {
  alert('正在保存参数并重启程序...')
}

function restoreDefaultsAndRestart() {
  alert('正在恢复默认参数并重启程序...')
}
</script>

<style scoped>

.frame {
  width: 500px;
  margin-left: 100px;
  margin-top: 30px;
}

.item-title {
  font-size: 1.17em;
  font-weight: bold;
  text-align: left;
  margin-bottom: 20px;
}

.item {
  text-align: left;
  margin-left: 20px;
}

</style>