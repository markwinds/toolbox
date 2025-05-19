<template>
  <n-space vertical :size="24" class="frame">
    <n-card>
      <div class="section">
        <div class="item-title">进制转换工具</div>
        <n-space vertical>
          <n-alert type="info">
            支持2进制、8进制、10进制、16进制之间的相互转换，输入任意一种进制的数值，其他进制的值将自动更新。
          </n-alert>
          <n-grid :cols="1" :x-gap="12">
            <n-grid-item>
              <n-form-item label="二进制 (Binary)">
                <n-input
                  v-model:value="binary"
                  placeholder="请输入二进制数值 (0, 1)"
                  @update:value="onBinaryChange"
                  :status="binaryValid ? undefined : 'error'"
                  class="text-align-left"
                />
              </n-form-item>
            </n-grid-item>
            <n-grid-item>
              <n-form-item label="八进制 (Octal)">
                <n-input
                  v-model:value="octal"
                  placeholder="请输入八进制数值 (0-7)"
                  @update:value="onOctalChange"
                  :status="octalValid ? undefined : 'error'"
                  class="text-align-left"
                />
              </n-form-item>
            </n-grid-item>
            <n-grid-item>
              <n-form-item label="十进制 (Decimal)">
                <n-input
                  v-model:value="decimal"
                  placeholder="请输入十进制数值 (0-9)"
                  @update:value="onDecimalChange"
                  :status="decimalValid ? undefined : 'error'"
                  class="text-align-left"
                />
              </n-form-item>
            </n-grid-item>
            <n-grid-item>
              <n-form-item label="十六进制 (Hexadecimal)">
                <n-input
                  v-model:value="hex"
                  placeholder="请输入十六进制数值 (0-9, A-F)"
                  @update:value="onHexChange"
                  :status="hexValid ? undefined : 'error'"
                  class="text-align-left"
                />
              </n-form-item>
            </n-grid-item>
          </n-grid>
          <n-space justify="end">
            <n-button @click="clearAll">清空</n-button>
          </n-space>
        </n-space>
      </div>
    </n-card>
  </n-space>
</template>

<script setup>
import { ref, computed } from 'vue'
import { useNotification } from 'naive-ui'

const notification = useNotification()

// 各进制的值
const binary = ref('')
const octal = ref('')
const decimal = ref('')
const hex = ref('')

// 验证各进制输入是否有效
const binaryValid = computed(() => {
  if (binary.value === '') return true
  return /^[01]+$/.test(binary.value)
})

const octalValid = computed(() => {
  if (octal.value === '') return true
  return /^[0-7]+$/.test(octal.value)
})

const decimalValid = computed(() => {
  if (decimal.value === '') return true
  return /^[0-9]+$/.test(decimal.value)
})

const hexValid = computed(() => {
  if (hex.value === '') return true
  return /^[0-9A-Fa-f]+$/.test(hex.value)
})

// 二进制输入变化时更新其他进制
function onBinaryChange(value) {
  if (value === '') {
    clearOthers('binary')
    return
  }

  if (!binaryValid.value) return

  try {
    const decValue = parseInt(value, 2)
    decimal.value = decValue.toString()
    octal.value = decValue.toString(8)
    hex.value = decValue.toString(16).toUpperCase()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: '无法将输入值转换为有效的二进制数',
      duration: 3000
    })
  }
}

// 八进制输入变化时更新其他进制
function onOctalChange(value) {
  if (value === '') {
    clearOthers('octal')
    return
  }

  if (!octalValid.value) return

  try {
    const decValue = parseInt(value, 8)
    binary.value = decValue.toString(2)
    decimal.value = decValue.toString()
    hex.value = decValue.toString(16).toUpperCase()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: '无法将输入值转换为有效的八进制数',
      duration: 3000
    })
  }
}

// 十进制输入变化时更新其他进制
function onDecimalChange(value) {
  if (value === '') {
    clearOthers('decimal')
    return
  }

  if (!decimalValid.value) return

  try {
    const decValue = parseInt(value, 10)
    binary.value = decValue.toString(2)
    octal.value = decValue.toString(8)
    hex.value = decValue.toString(16).toUpperCase()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: '无法将输入值转换为有效的十进制数',
      duration: 3000
    })
  }
}

// 十六进制输入变化时更新其他进制
function onHexChange(value) {
  if (value === '') {
    clearOthers('hex')
    return
  }

  if (!hexValid.value) return

  try {
    const decValue = parseInt(value, 16)
    binary.value = decValue.toString(2)
    octal.value = decValue.toString(8)
    decimal.value = decValue.toString()
  } catch (error) {
    notification.error({
      title: '转换错误',
      content: '无法将输入值转换为有效的十六进制数',
      duration: 3000
    })
  }
}

// 清空除了指定字段外的所有其他字段
function clearOthers(except) {
  if (except !== 'binary') binary.value = ''
  if (except !== 'octal') octal.value = ''
  if (except !== 'decimal') decimal.value = ''
  if (except !== 'hex') hex.value = ''
}

// 清空所有字段
function clearAll() {
  binary.value = ''
  octal.value = ''
  decimal.value = ''
  hex.value = ''
}
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

.text-align-left {
  text-align: left;
}
</style>
