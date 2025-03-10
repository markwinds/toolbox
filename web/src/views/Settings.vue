<template>
  <n-space vertical :size="24" class="frame">
    <n-card>
      <div class="section">
        <div class="item-title">软件版本</div>
        <div class="version-info">
          <div class="item">当前版本: {{ currentVersion }}</div>
          <div class="item">最新版本: {{ latestVersion }}</div>
          <n-button type="primary" class="update-btn" @click="updateSoftware">更新</n-button>
        </div>
      </div>
    </n-card>

    <n-card>
      <div class="section">
        <div class="item-title">设置项</div>
        <n-form :model="settings" label-placement="left" label-width="120">
          <n-form-item label="日志等级">
            <n-select v-model="settings.logLevel" :options="logLevelOptions"/>
          </n-form-item>
          <n-form-item label="数据存放目录">
            <n-input v-model="settings.dataDirectory"/>
          </n-form-item>
          <n-form-item label="远程访问" label-width="120">
            <n-space align="center" justify="space-between">
              <n-switch v-model:value="settings.remoteAccess"/>
              <n-tooltip trigger="hover" placement="right">
                <template #trigger>
                  <n-icon size="16" style="cursor: help;">
                    <question-circle-outlined/>
                  </n-icon>
                </template>
                开启远程访问可能带来安全风险，建议仅在必要时开启，并设置强密码
              </n-tooltip>
            </n-space>
          </n-form-item>
          <n-form-item label="Web 监听端口">
            <n-input-number v-model="settings.webPort"/>
          </n-form-item>
          <n-form-item label="代理地址">
            <n-input v-model="settings.proxyAddress"/>
          </n-form-item>
        </n-form>
      </div>
    </n-card>

    <n-card>
      <div class="section">
        <div class="item-title">操作按钮</div>
        <n-space justify="center" :size="20">
          <n-button type="error" @click="exitProgram">退出程序</n-button>
          <n-button type="warning" @click="restartProgram">重启程序</n-button>
          <n-button type="primary" @click="saveAndRestart">保存参数并重启</n-button>
          <n-button type="info" @click="restoreDefaultsAndRestart">恢复默认参数并重启</n-button>
        </n-space>
      </div>
    </n-card>
  </n-space>
</template>

<script setup>
import {onMounted, ref} from 'vue'
import {useNotification, NSpace, NButton, NForm, NFormItem, NSelect, NInput, NInputNumber, NDivider, NCard, NSwitch, NTooltip, NIcon} from 'naive-ui'
import { QuestionCircleOutlined } from '@vicons/antd' // 替换图标引入
import {showInfo} from "../utils/notification.js"
import {reqSuccessCode, service} from "../utils/request.js";

const notification = useNotification()

const baseUrl = 'setting'
const currentVersion = ref('1.0.0')
const latestVersion = ref('1.1.0')
const settings = ref({
  logLevel: 'info',
  dataDirectory: '/path/to/data',
  webPort: 8080,
  proxyAddress: 'http://proxy.example.com',
  remoteAccess: false  // 新增远程访问配置项
})
const logLevelOptions = [
  {label: 'Debug', value: 'debug'},
  {label: 'Info', value: 'info'},
  {label: 'Warn', value: 'warn'},
  {label: 'Error', value: 'error'},
]

onMounted(() => {
  getVersion()
})

async function getVersion() {
  let res = await service({
    url: baseUrl + '/version',
  })
  if (res.code !== reqSuccessCode) {
    return
  }
  currentVersion.value = res.result.currentVersion
}

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
  width: 600px;
  margin: 40px auto;
}

.section {
  padding: 10px;
}

.item-title {
  font-size: 1.2em;
  font-weight: 600;
  color: #18a058;
  margin-bottom: 24px;
  border-left: 4px solid #18a058;
  padding-left: 10px;
}

.version-info {
  display: flex;
  align-items: center;
  gap: 20px;
  padding-left: 20px;
}

.item {
  margin: 0;
  color: #666;
}

.update-btn {
  margin-left: auto;
}

:deep(.n-card) {
  border-radius: 8px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.08);
}

:deep(.n-form-item) {
  margin-bottom: 18px;
}

:deep(.n-input) {
  width: 100%;
}

:deep(.remote-access-item .n-form-item-label) {
  display: inline-flex;
  align-items: center;
}
</style>