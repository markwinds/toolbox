<template>
  <n-layout has-sider style="height: 100vh;">

    <n-notification-provider>
      <n-config-provider :theme="darkTheme">
        <n-layout-sider width="200" class="side">
          <n-scrollbar>
            <n-menu :options="menuOptions" @update:value="handleMenuChange"/>
          </n-scrollbar>
        </n-layout-sider>
      </n-config-provider>
    </n-notification-provider>

    <n-notification-provider>
      <n-config-provider :locale="zhCN" style="width: 100%">
        <n-layout-content class="content">
          <n-scrollbar trigger="hover">
            <router-view/>
          </n-scrollbar>
        </n-layout-content>
      </n-config-provider>
    </n-notification-provider>

  </n-layout>

</template>

<script setup>
import {useRouter} from 'vue-router'
import {darkTheme, NIcon, zhCN} from "naive-ui"
import {h} from "vue"
import {
  SettingsOutline,
  CodeOutline,
  SwapHorizontalOutline,
  TimeOutline
} from '@vicons/ionicons5'


const router = useRouter()

const menuOptions = [
  {label: '设置', key: 'settings', icon: renderIcon(SettingsOutline)},
  {label: '文本格式化', key: 'text-formatter', icon: renderIcon(CodeOutline)},
  {label: '进制转换', key: 'base-converter', icon: renderIcon(SwapHorizontalOutline)},
  {label: '时间转换', key: 'time-converter', icon: renderIcon(TimeOutline)},
]

function renderIcon(icon) {
  return () => h(NIcon, null, {default: () => h(icon)});
}

function handleMenuChange(key) {
  router.push(`/${key}`)
}
</script>

<style>
/* Ensure the layout covers the entire screen */
html, body, #app {
  height: 100%;
  width: 100%;
  margin: 0;
  padding: 0; /* Remove padding from the app element */
}

.side {
  height: 100%;
}

.content {
  height: 100%;
  background-color: #f0f2f5; /* 添加浅灰色背景 */
}

</style>