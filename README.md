# toolbox

toolbox是一个极简、资源占用少、功能全面的工具箱。因为使用c++开发且不存在任何冗余功能,toolbox的内存占用不到10MB。

## 致谢

首先感谢开源社区，本项目用到了drogon、curl等优秀开源项目。

Thanks to [JetBrains](https://www.jetbrains.com/) for providing a free open-source license for their excellent IDEs!

[![JetBrains logo.](https://resources.jetbrains.com/storage/products/company/brand/logos/jetbrains.svg)](https://jb.gg/OpenSourceSupport)


## 功能特性

预计会完成下面这些功能。

- [x] 配置界面
- [ ] 类似postman的httpClient调试助手
- [ ] tcp、udp收发调试助手
- [ ] 截图工具
- [ ] terminal工具
- [ ] 进制转换工具
- [ ] 图片处理工具
- [ ] tftp、ftp服务器
- [ ] json、xml格式化工具
- [ ] 支持插件

## 编译代码

项目代码分为前端代码和后端代码，最后会一起打到一个exe程序中，最终的编译成果就是一个exe文件，不携带额外的动态库或者文件。
改项目需要在windows环境编译，并安装clang等工具。

先编译前端
```shell
cd web
yarn install
yarn run build
```

编译后端
```shell
cd backEnd
./build.bat
```

支持的编译参数（待补充）

## 限制

该项目目前仅支持在windows环境编译，且只支持输出windows平台下的exe成果物。




