## 应用于Linux/Windows的人脸识别项目 - 客户端

### 前提需知

* 本客户端包括两个部分：一个是业务部分`biz`，用于实现实际的业务代码，一个是脚手架部分`airstrip`，用于存储boilerplate以及常用的工具代码，其他类似项目也可以从这个脚手架开始

### 编译流程

* 下载[Boost]((https://www.boost.org/))到本地
* 编译脚手架`airstrip`
* 下载[OpenCV](https://opencv.org/)、[InspireFace](https://github.com/HyperInspire/InspireFace)到本地
* 再编译业务部分`biz`

### mingw下boost的构建

* 下载源代码之后，进入`tools/build`文件夹
* 执行`bootstrap.bat mingw`
* 如果出现错误`res.rc:1: fatal error: when writing output to : Invalid argument`，我们进入`tools/build/src/engine`文件夹下，修改`config_toolset.bat`将
`set "B2_CXX="%%i" --input res.rc --output res.o && %B2_CXX% -Wl,res.o"`改成
`set "B2_CXX="%%i" --input res.rc --output res.o --use-temp-file && %B2_CXX% -Wl,res.o"` 即可
* `b2`构建完成后，将其所在文件夹加入环境变化，返回源代码根目录，执行`b2 --build-dir="build" --toolset=gcc --build-type=complete stage`
* 此时即可引入项目当中