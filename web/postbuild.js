// Use import instead of require
import {exec} from 'child_process'; // Import exec from 'child_process'

const platform = process.platform; // 获取当前操作系统
let command;

if (platform === 'win32') {
    // Windows 平台命令
    command = 'powershell -Command "Compress-Archive -Path "dist" -DestinationPath "dist.zip" -Force"';
} else if (platform === 'darwin') {
    // macOS 平台命令
    command = 'echo macOS Post Build Command';
} else if (platform === 'linux') {
    // Linux 平台命令
    command = 'echo Linux Post Build Command';
} else {
    console.log('Unsupported platform');
    process.exit(1);
}

exec(command, (error, stdout, stderr) => {
    if (error) {
        console.error(`exec error: ${error}`);
        return;
    }
    if (stderr) {
        console.error(`stderr: ${stderr}`);
        return;
    }
    console.log(`stdout: ${stdout}`);
});
