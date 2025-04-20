import axios from "axios"
import {showLoading as showSpin, hideLoading as hideSpin} from "./loading.js";
import {useNotification} from "naive-ui";

const notification = useNotification()
let loadingCount = 0
export let reqSuccessCode = 0

const showLoading = () => {
    if (loadingCount === 0) {
        showSpin()
    }
    loadingCount++
}

const hideLoading = () => {
    loadingCount--
    if (loadingCount === 0) {
        hideSpin()
    }
}

const showError = (msg) => {
    notification.error({
        content: msg,
        duration: 3000,
        keepAliveOnHover: true
    })
}
export const service = axios.create({ 
    baseURL: import.meta.env.VITE_BASE_API || '/',
    timeout: 10000, // 请求超时时间
})

service.interceptors.request.use(
    (config) => {
        showLoading();
        return config;
    },
    (error) => {
        hideLoading();
        return Promise.reject(error);
    }
)

service.interceptors.response.use(
    (response) => {
        hideLoading();

        if (response.config.responseType === 'arraybuffer') {
            if (!response.headers.hasOwnProperty('err-code')) {
                // 没有错误信息 直接返回二进制数据
                return response
            }
            // 有错误信息 将arraybuffer转为json
            const arrayBuffer = response.data
            const dataView = new DataView(arrayBuffer)
            const decoder = new TextDecoder('utf-8')
            const jsonString = decoder.decode(dataView)
            response.data = JSON.parse(jsonString)
        }
        if (response.data.code === reqSuccessCode) {
            return response.data
        }

        // console.log("data", response.data)
        showError(response.data.zhMsg === '' ? '未知错误' : response.data.zhMsg)

        return response.data
    },
    (error) => {
        hideLoading();
        if (error.response) {
            const {status, data} = error.response;
            if (status === 400) {
                showError(status + '：请求错误')
            } else if (status === 401) {
                showError(status + '：未授权')
            } else if (status === 403) {
                showError(status + '：拒绝访问')
            } else if (status === 404) {
                showError(status + '：请求地址出错')
            } else if (status === 500) {
                showError(status + '：服务器内部错误')
            } else {
                showError(status + '：请求失败')
            }
        } else {
            showError("请求失败，请检查网络")
        }

        return Promise.reject(error);
    }
)

export default {service, reqSuccessCode}
