<template>
  <n-space vertical :size="24" class="frame">
    <n-card>
      <div class="section">
        <div class="item-title">文本格式化工具</div>
        <n-space vertical>
          <n-tabs type="line" animated>
            <n-tab-pane name="json" tab="JSON格式化">
              <n-space vertical>
                <n-space justify="space-between">
                  <n-button @click="formatJson" type="primary">格式化</n-button>
                  <n-button @click="copyFormatted" :disabled="!formattedText">复制结果</n-button>
                  <n-button @click="clearText">清空</n-button>
                </n-space>
                <n-grid :cols="2" :x-gap="12">
                  <n-grid-item>
                    <div class="editor-label">原始文本:</div>
                    <n-input
                      v-model:value="inputText"
                      type="textarea"
                      placeholder="请输入需要格式化的JSON文本"
                      :autosize="{ minRows: 20, maxRows: 40 }"
                      class="text-align-left"
                    />
                  </n-grid-item>
                  <n-grid-item>
                    <div class="editor-label">格式化结果:</div>
                    <n-input
                      v-model:value="formattedText"
                      type="textarea"
                      placeholder="格式化后的结果将显示在这里"
                      :autosize="{ minRows: 20, maxRows: 40 }"
                      readonly
                      class="text-align-left"
                    />
                  </n-grid-item>
                </n-grid>
              </n-space>
            </n-tab-pane>
            <n-tab-pane name="xml" tab="XML格式化">
              <n-space vertical>
                <n-space justify="space-between">
                  <n-button @click="formatXml" type="primary">格式化</n-button>
                  <n-button @click="copyFormatted" :disabled="!formattedText">复制结果</n-button>
                  <n-button @click="clearText">清空</n-button>
                </n-space>
                <n-grid :cols="2" :x-gap="12">
                  <n-grid-item>
                    <div class="editor-label">原始文本:</div>
                    <n-input
                      v-model:value="inputText"
                      type="textarea"
                      placeholder="请输入需要格式化的XML文本"
                      :autosize="{ minRows: 20, maxRows: 40 }"
                      class="text-align-left"
                    />
                  </n-grid-item>
                  <n-grid-item>
                    <div class="editor-label">格式化结果:</div>
                    <n-input
                      v-model:value="formattedText"
                      type="textarea"
                      placeholder="格式化后的结果将显示在这里"
                      :autosize="{ minRows: 20, maxRows: 40 }"
                      readonly
                      class="text-align-left"
                    />
                  </n-grid-item>
                </n-grid>
              </n-space>
            </n-tab-pane>
          </n-tabs>
        </n-space>
      </div>
    </n-card>
  </n-space>
</template>

<script setup>
import { ref } from 'vue'
import { useNotification } from 'naive-ui'

const notification = useNotification()
const inputText = ref('')
const formattedText = ref('')

// 格式化JSON
function formatJson() {
  if (!inputText.value.trim()) {
    return
  }

  try {
    // 解析JSON字符串为对象
    const jsonObj = JSON.parse(inputText.value)
    // 将对象转换回格式化的JSON字符串（使用2个空格缩进）
    formattedText.value = JSON.stringify(jsonObj, null, 2)
  } catch (error) {
    formattedText.value = '格式化失败: ' + error.message
  }
}

// 格式化XML
function formatXml() {
  if (!inputText.value.trim()) {
    return
  }

  try {
    // 使用浏览器内置的XML解析和格式化功能
    const xmlDoc = new DOMParser().parseFromString(inputText.value, 'text/xml')

    // 检查解析错误
    const parseError = xmlDoc.getElementsByTagName('parsererror')
    if (parseError.length > 0) {
      throw new Error('XML解析错误')
    }

    // 将XML转换为格式化的字符串
    const serializer = new XMLSerializer()
    let xmlString = serializer.serializeToString(xmlDoc)

    // 手动添加缩进和换行
    xmlString = formatXmlString(xmlString)

    formattedText.value = xmlString
  } catch (error) {
    formattedText.value = '格式化失败: ' + error.message
  }
}

// 手动格式化XML字符串
function formatXmlString(xml) {
  let formatted = ''
  let indent = ''
  const tab = '  ' // 两个空格作为缩进

  xml = xml.replace(/(>)(<)(\/)*/g, '$1\n$2$3')

  xml.split('\n').forEach(node => {
    if (node.match(/.+<\/\w[^>]*>$/)) {
      // 开始标签和结束标签在同一行
      formatted += indent + node + '\n'
    } else if (node.match(/^<\/\w/)) {
      // 结束标签
      indent = indent.substring(tab.length)
      formatted += indent + node + '\n'
    } else if (node.match(/^<\w[^>]*[^\/]>.*$/)) {
      // 开始标签
      formatted += indent + node + '\n'
      indent += tab
    } else {
      // 其他情况
      formatted += indent + node + '\n'
    }
  })

  return formatted.trim()
}

// 复制格式化后的文本
function copyFormatted() {
  if (!formattedText.value) {
    return
  }

  navigator.clipboard.writeText(formattedText.value)
    .then(() => {
      // 使用Naive UI的notification组件显示成功提示
      notification.success({
        content: "已复制到剪贴板",
        duration: 2000,
        keepAliveOnHover: true
      })
    })
    .catch(err => {
      notification.error({
        content: "复制失败: " + err.message,
        duration: 3000,
        keepAliveOnHover: true
      })
    })
}

// 清空文本
function clearText() {
  inputText.value = ''
  formattedText.value = ''
}
</script>

<style scoped>
.frame {
  padding: 24px;
  max-width: 1200px;
  margin: 0 auto;
}

.section {
  width: 100%;
}

.item-title {
  font-size: 1.2em;
  font-weight: 600;
  color: #18a058;
  margin-bottom: 24px;
  border-left: 4px solid #18a058;
  padding-left: 10px;
}

.editor-label {
  margin-bottom: 8px;
  font-weight: 500;
}

:deep(.n-card) {
  border-radius: 8px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.08);
}

.text-align-left :deep(textarea) {
  text-align: left;
  font-family: monospace;
  font-size: 14px;
  line-height: 1.5;
}


</style>
