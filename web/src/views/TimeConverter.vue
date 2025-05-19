<template>
  <n-space vertical :size="24" class="frame">
    <n-card>
      <div class="section">
        <div class="item-title">时间转换工具</div>
        <n-space vertical>
          <n-alert type="info">
            支持时间戳和字符串时间之间的相互转换，并可查看各个时区的时间。
          </n-alert>

          <n-tabs type="line" animated>
            <!-- 时间戳转字符串时间 -->
            <n-tab-pane name="timestamp-to-date" tab="时间戳转时间">
              <n-space vertical>
                <n-form-item label="时间戳 (秒)">
                  <n-input-number
                    v-model:value="timestamp"
                    placeholder="请输入Unix时间戳（秒）"
                    :show-button="false"
                    style="width: 100%; text-align: left;"
                    @update:value="onTimestampChange"
                  />
                </n-form-item>
                <n-space justify="space-between">
                  <n-button @click="useCurrentTimestamp">使用当前时间</n-button>
                  <n-button @click="clearTimestamp">清空</n-button>
                </n-space>

                <n-divider />

                <n-form-item label="时区选择">
                  <n-select
                    v-model:value="selectedTimezone"
                    :options="timezoneOptions"
                    @update:value="onTimezoneChange"
                  />
                </n-form-item>

                <n-form-item label="日期时间">
                  <n-input
                    v-model:value="dateTimeString"
                    placeholder="转换后的日期时间"
                    readonly
                    class="text-align-left"
                  />
                </n-form-item>

                <n-form-item label="ISO 8601 格式">
                  <n-input
                    v-model:value="isoString"
                    placeholder="ISO 8601 格式的日期时间"
                    readonly
                    class="text-align-left"
                  />
                </n-form-item>
              </n-space>
            </n-tab-pane>

            <!-- 字符串时间转时间戳 -->
            <n-tab-pane name="date-to-timestamp" tab="时间转时间戳">
              <n-space vertical>
                <n-form-item label="日期时间">
                  <n-date-picker
                    v-model:value="dateTimeValue"
                    type="datetime"
                    clearable
                    style="width: 100%; text-align: left;"
                    @update:value="onDateTimeChange"
                  />
                </n-form-item>

                <n-form-item label="时区选择">
                  <n-select
                    v-model:value="selectedTimezone"
                    :options="timezoneOptions"
                    @update:value="onTimezoneChange"
                  />
                </n-form-item>

                <n-divider />

                <n-form-item label="Unix 时间戳 (秒)">
                  <n-input
                    v-model:value="convertedTimestamp"
                    placeholder="转换后的Unix时间戳"
                    readonly
                    class="text-align-left"
                  />
                </n-form-item>

                <n-form-item label="Unix 时间戳 (毫秒)">
                  <n-input
                    v-model:value="convertedTimestampMs"
                    placeholder="转换后的Unix时间戳(毫秒)"
                    readonly
                    class="text-align-left"
                  />
                </n-form-item>
              </n-space>
            </n-tab-pane>

            <!-- 时区时间对照 -->
            <n-tab-pane name="timezone-comparison" tab="时区对照">
              <n-space vertical>
                <n-form-item label="参考时间">
                  <n-date-picker
                    v-model:value="referenceTime"
                    type="datetime"
                    clearable
                    style="width: 100%; text-align: left;"
                    @update:value="updateTimezoneComparison"
                  />
                </n-form-item>

                <n-button @click="useCurrentTimeForComparison">使用当前时间</n-button>

                <n-divider />

                <div class="timezone-table">
                  <n-table :bordered="false" :single-line="false">
                    <thead>
                      <tr>
                        <th>时区</th>
                        <th>时间</th>
                      </tr>
                    </thead>
                    <tbody>
                      <tr v-for="tz in timezoneComparisonList" :key="tz.value">
                        <td>{{ tz.label }}</td>
                        <td>{{ tz.time }}</td>
                      </tr>
                    </tbody>
                  </n-table>
                </div>
              </n-space>
            </n-tab-pane>
          </n-tabs>
        </n-space>
      </div>
    </n-card>
  </n-space>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useNotification } from 'naive-ui'

const notification = useNotification()

// 时区选项
const timezoneOptions = [
  { label: '北京时间 (UTC+8)', value: 'Asia/Shanghai' },
  { label: '格林威治标准时间 (UTC)', value: 'UTC' },
  { label: '东京时间 (UTC+9)', value: 'Asia/Tokyo' },
  { label: '纽约时间 (UTC-5/UTC-4)', value: 'America/New_York' },
  { label: '伦敦时间 (UTC+0/UTC+1)', value: 'Europe/London' },
  { label: '悉尼时间 (UTC+10/UTC+11)', value: 'Australia/Sydney' },
  { label: '洛杉矶时间 (UTC-8/UTC-7)', value: 'America/Los_Angeles' },
]

// 时间戳转时间
const timestamp = ref('')
const selectedTimezone = ref('Asia/Shanghai')
const dateTimeString = ref('')
const isoString = ref('')

// 时间转时间戳
const dateTimeValue = ref(null)
const convertedTimestamp = ref('')
const convertedTimestampMs = ref('')

// 时区对照
const referenceTime = ref(null)
const timezoneComparisonList = ref([])

// 当时间戳变化时更新日期时间字符串
function onTimestampChange(value) {
  if (!value) {
    dateTimeString.value = ''
    isoString.value = ''
    return
  }

  try {
    const date = new Date(value * 1000)

    // 检查日期是否有效
    if (isNaN(date.getTime())) {
      notification.error({
        title: '转换错误',
        content: '无效的时间戳',
        duration: 3000
      })
      return
    }

    // 格式化为本地时间字符串
    const options = {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit',
      timeZone: selectedTimezone.value
    }

    dateTimeString.value = new Intl.DateTimeFormat('zh-CN', options).format(date)
    isoString.value = new Date(date.toLocaleString('en-US', { timeZone: selectedTimezone.value })).toISOString()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: error.message,
      duration: 3000
    })
  }
}

// 当选择的时区变化时更新时间显示
function onTimezoneChange() {
  if (timestamp.value) {
    onTimestampChange(timestamp.value)
  }
  if (dateTimeValue.value) {
    onDateTimeChange(dateTimeValue.value)
  }
  if (referenceTime.value) {
    updateTimezoneComparison()
  }
}

// 使用当前时间戳
function useCurrentTimestamp() {
  timestamp.value = Math.floor(Date.now() / 1000)
  onTimestampChange(timestamp.value)
}

// 清空时间戳
function clearTimestamp() {
  timestamp.value = ''
  dateTimeString.value = ''
  isoString.value = ''
}

// 当日期时间选择变化时更新时间戳
function onDateTimeChange(value) {
  if (!value) {
    convertedTimestamp.value = ''
    convertedTimestampMs.value = ''
    return
  }

  try {
    // 将选择的日期转换为时间戳
    const date = new Date(value)
    convertedTimestamp.value = Math.floor(date.getTime() / 1000).toString()
    convertedTimestampMs.value = date.getTime().toString()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: error.message,
      duration: 3000
    })
  }
}

// 更新时区对照表
function updateTimezoneComparison() {
  if (!referenceTime.value) {
    timezoneComparisonList.value = []
    return
  }

  const date = new Date(referenceTime.value)

  timezoneComparisonList.value = timezoneOptions.map(tz => {
    const options = {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit',
      timeZone: tz.value
    }

    return {
      label: tz.label,
      value: tz.value,
      time: new Intl.DateTimeFormat('zh-CN', options).format(date)
    }
  })
}

// 使用当前时间进行时区对照
function useCurrentTimeForComparison() {
  referenceTime.value = Date.now()
  updateTimezoneComparison()
}

// 组件挂载时初始化
onMounted(() => {
  // 默认使用当前时间
  useCurrentTimeForComparison()
})
</script>

<style scoped>
.frame {
  padding: 20px;
}

.section {
  margin-bottom: 20px;
}

.item-title {
  font-size: 18px;
  font-weight: bold;
  margin-bottom: 16px;
}

.timezone-table {
  margin-top: 16px;
}

.text-align-left {
  text-align: left;
}
</style>
