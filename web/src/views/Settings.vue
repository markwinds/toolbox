<template>
  <n-space vertical :size="24" class="frame">
    <n-card>
      <div class="section">
        <div class="item-title">软件版本</div>
        <div class="version-info">
          <div class="item">当前版本: {{ currentVersion }}</div>
          <div class="item">最新版本: {{ latestVersion }}</div>
          <n-button v-if="hasNewVersion" type="primary" class="update-btn" @click="updateSoftware">更新</n-button>
        </div>
      </div>
    </n-card>

    <n-card>
      <div class="section">
        <div class="item-title">设置项</div>
        <n-form :model="settings" label-placement="left" label-width="120">
          <n-form-item label="日志等级">
            <n-select v-model:value="settings.logLevel" :options="logLevelOptions" />
          </n-form-item>
          <n-form-item label="数据存放目录">
            <n-input v-model:value="settings.dataDirectory" />
          </n-form-item>
          <n-form-item label="远程访问" label-width="120">
            <n-space align="center" justify="space-between">
              <n-switch v-model:value="settings.remoteAccess" />
              <n-tooltip trigger="hover" placement="right">
                <template #trigger>
                  <n-icon size="16" style="cursor: help;">
                    <question-circle-outlined />
                  </n-icon>
                </template>
                开启远程访问可能带来安全风险，建议仅在必要时开启
              </n-tooltip>
            </n-space>
          </n-form-item>
          <n-form-item label="Web 监听端口">
            <n-input-number v-model:value="settings.webPort" />
          </n-form-item>
          <n-form-item label="代理地址">
            <n-input v-model:value="settings.proxyAddress" />
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
import { onMounted, ref, computed } from 'vue'
import { useNotification, NSpace, NButton, NForm, NFormItem, NSelect, NInput, NInputNumber, NDivider, NCard, NSwitch, NTooltip, NIcon } from 'naive-ui'
import { QuestionCircleOutlined } from '@vicons/antd' // 替换图标引入
import { reqSuccessCode, service } from "../utils/request.js";

const notification = useNotification()

const baseUrl = 'setting'
const currentVersion = ref('1.0.0')
const latestVersion = ref('0.0.0')
const hasNewVersion = computed(() => compareVersions(latestVersion.value, currentVersion.value) > 0)
const settings = ref({
  logLevel: 'debug',
  dataDirectory: './data',
  webPort: 6666,
  proxyAddress: '',
  remoteAccess: false
})
const logLevelOptions = [
  { label: 'Debug', value: 'debug' },
  { label: 'Info', value: 'info' },
  { label: 'Warn', value: 'warn' },
  { label: 'Error', value: 'error' },
]

onMounted(async () => {
  await getVersion()
  await getLatestVersion()
  await getConfig()
  
  // Check for updates on page load
  if (hasNewVersion.value) {
    notification["warning"]({
      title: "发现新版本",
      content: `当前版本 ${currentVersion.value}，最新版本 ${latestVersion.value}，建议更新到最新版本。`,
      duration: 5000,
    })
  }
})

async function getVersion() {
  let res = await service({
    url: baseUrl + '/version',
  })
  if (res.code !== reqSuccessCode) {
    return
  }
  currentVersion.value = res.result
}

async function getLatestVersion() {
  let res = await service({
    url: baseUrl + '/latestVersion',
  })
  if (res.code !== reqSuccessCode) {
    return
  }
  latestVersion.value = res.result
}

async function updateSoftware() {
  let res = await service({
    url: baseUrl + '/update',
  })
  if (res.code !== reqSuccessCode) {
    return
  }
  notification["success"]({
    content: "更新成功",
    duration: 2000,
  })
}

async function exitProgram() {
  let res = await service({
    url: baseUrl + '/exit',
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "程序退出失败",
      duration: 2000,
    })
    return
  }
  notification["success"]({
    content: "程序已退出",
    duration: 2000,
  })
}

async function restartProgram() {
  let res = await service({
    url: baseUrl + '/restart',
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "重启失败",
      duration: 2000,
    })
    return
  }
  notification["success"]({
    content: "正在重启程序...",
    duration: 2000,
  })
}

async function saveAndRestart() {
  // 先保存配置
  if (!await saveConfig()) {
    return
  }

  // 然后重启
  let res = await service({
    url: baseUrl + '/restart',
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "重启失败",
      duration: 2000,
    })
    return
  }
  notification["success"]({
    content: "保存参数成功，正在重启...",
    duration: 2000,
  })
}

async function restoreDefaultsAndRestart() {
  // 重置配置
  let res = await service({
    url: baseUrl + '/resetConfig',
    method: 'post'
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "恢复默认参数失败",
      duration: 2000,
    })
    return
  }

  // 重新获取配置
  await getConfig()

  // 重启
  res = await service({
    url: baseUrl + '/restart',
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "重启失败",
      duration: 2000,
    })
    return
  }

  notification["success"]({
    content: "已恢复默认参数，正在重启...",
    duration: 2000,
  })
}

// 获取配置
async function getConfig() {
  let res = await service({
    url: baseUrl + '/config',
  })
  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "获取配置失败",
      duration: 2000,
    })
    return false
  }

  // 将后端配置映射到前端设置对象
  const config = res.result
  settings.value = {
    logLevel: mapLogLevel(config.logLevel),
    dataDirectory: config.dataDir,
    webPort: config.port,
    proxyAddress: config.proxyUrl,
    remoteAccess: config.remoteAccess
  }

  return true
}

// 保存配置
async function saveConfig() {
  // 将前端设置对象映射到后端配置格式
  const config = {
    logLevel: mapLogLevelToBackend(settings.value.logLevel),
    dataDir: settings.value.dataDirectory,
    port: settings.value.webPort,
    proxyUrl: settings.value.proxyAddress,
    remoteAccess: settings.value.remoteAccess
  }

  let res = await service({
    url: baseUrl + '/saveConfig',
    method: 'post',
    data: config
  })

  if (res.code !== reqSuccessCode) {
    notification["error"]({
      content: "保存配置失败",
      duration: 2000,
    })
    return false
  }

  notification["success"]({
    content: "配置已保存",
    duration: 2000,
  })
  return true
}

// 将后端日志级别映射到前端选项
function mapLogLevel(level) {
  const map = {
    0: 'debug',
    1: 'info',
    2: 'warn',
    3: 'error'
  }
  return map[level]
}

// 将前端日志级别映射到后端值
function mapLogLevelToBackend(level) {
  const map = {
    'debug': 0,
    'info': 1,
    'warn': 2,
    'error': 3
  }
  return map[level]
}

function cleanVersion(version) {
  // 匹配 x.y.z 格式的版本号，忽略后面的其他信息
  const match = version.match(/(\d+\.\d+\.\d+)/)
  return match ? match[1] : version
}

function compareVersions(v1, v2) {
  // 清理版本号，只保留 x.y.z 格式
  const cleanV1 = cleanVersion(v1)
  const cleanV2 = cleanVersion(v2)
  
  const parts1 = cleanV1.split('.').map(Number)
  const parts2 = cleanV2.split('.').map(Number)
  
  for (let i = 0; i < 3; i++) {
    if (parts1[i] > parts2[i]) return 1
    if (parts1[i] < parts2[i]) return -1
  }
  return 0
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