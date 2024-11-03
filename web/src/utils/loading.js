import {h, render} from 'vue'
import {NSpin} from 'naive-ui'

function showLoading() {
    // 创建遮罩元素
    const loadingOverlay = document.createElement('div');
    loadingOverlay.id = 'loading-overlay';
    loadingOverlay.style.position = 'fixed';
    loadingOverlay.style.top = '0';
    loadingOverlay.style.left = '0';
    loadingOverlay.style.width = '100%';
    loadingOverlay.style.height = '100%';
    loadingOverlay.style.display = 'flex';
    loadingOverlay.style.justifyContent = 'center';
    loadingOverlay.style.alignItems = 'center';
    loadingOverlay.style.backgroundColor = 'rgba(255, 255, 255, 0.8)';
    loadingOverlay.style.zIndex = '9999';

    // // 创建加载动画元素
    // const spinner = document.createElement('div');
    // spinner.className = 'spinner';
    // spinner.style.width = '50px';
    // spinner.style.height = '50px';
    // spinner.style.border = '5px solid rgba(0, 0, 0, 0.1)';
    // spinner.style.borderTop = '5px solid #000';
    // spinner.style.borderRadius = '50%';
    // spinner.style.animation = 'spin 1s linear infinite';
    //
    // // 将加载动画添加到遮罩
    // loadingOverlay.appendChild(spinner);

    // 创建Vue实例并渲染NSpin组件
    const spinner = h(NSpin, {size: 'large'});
    render(spinner, loadingOverlay);

    // 将遮罩添加到body
    document.body.appendChild(loadingOverlay);
}

// 添加CSS动画
const style = document.createElement('style');
style.innerHTML = `
  @keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
  }
`;
document.head.appendChild(style);

function hideLoading() {
    const loadingOverlay = document.getElementById('loading-overlay');
    if (loadingOverlay) {
        document.body.removeChild(loadingOverlay);
    }
}

export {showLoading, hideLoading}