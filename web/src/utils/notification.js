// Filter out duplicate messages within a short time frame
let msgList = []

// Check if a message already exists
function hasMsg(msg) {
    return msgList.includes(msg)
}

// Add a message to the list
function pushMsg(msg) {
    msgList.push(msg)
}

// Remove a message from the list
function rmMsg(msg) {
    msgList = msgList.filter(item => item !== msg)
}

// Show a notification message
function showMsg(notification, msg, type, title) {
    if (hasMsg(msg)) {
        return;
    }
    pushMsg(msg);

    notification[type]({
        title: title,
        content: msg,
        duration: 4500,
        onAfterLeave: () => {
            rmMsg(msg) // Ensure only one identical message is displayed at a time
        }
    })

}

// Show an info notification
function showInfo(notification, msg) {
    showMsg(notification, msg, 'info', '信息');
}

// Show a warning notification
function showWarning(notification, msg) {
    showMsg(notification, msg, 'warning', '警告');
}

// Show an error notification
function showError(notification, msg) {
    showMsg(notification, msg, 'error', '错误');
}

// Show a success notification
function showSuccess(notification, msg) {
    showMsg(notification, msg, 'success', '成功');
}

export {showSuccess, showInfo, showWarning, showError};