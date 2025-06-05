## 应用于Linux/Windows的人脸识别项目 - 客户端

### 前提需知

* 本客户端包括两个部分：一个是业务部分`biz`，用于实现实际的业务代码，一个是脚手架部分`airstrip`，
  用于存储boilerplate以及常用的工具代码，其他类似项目也可以从这个脚手架开始
* 本客户端可以在`Windows`和`Linux`环境下编译运行。以下内容，默认`Windows`为开发环境，`Linux`为交叉编译生产环境。当然，你可以将
  `Linux`也作为开发环境，这样更简单，但是需要改变部分预编译代码
* 编译`airstrip`和`biz`前，记得修改`CMakeList.txt`中关于所需库位置的环境变量
* 本代码在X86_64 Windows（测试）和ARM Linux（正式）正常运行，其他平台未测试，需要修改部分预编译代码

### Windows开发环境编译流程

* 下[Qt](https://www.qt.io/)到本地，并构建，设置环境变量
* 下载[Boost]((https://www.boost.org/))到本地，并构建
* 下载[SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)到本地，并构建
* 下载[OpenCV](https://opencv.org/)到本地，并构建
* 下载[libfacedetection](https://github.com/ShiqiYu/libfacedetection)到本地，这里目前Windows没有用到，可以只使用头文件
* 下载[InspireFace](https://github.com/HyperInspire/InspireFace)到本地，这里目前Windows没有用到，可以只使用头文件
* 编译脚手架`airstrip`
* 再编译业务部分`biz`

#### 编译提示

* 使用`CLion`跑测试环境可以参考，可以参考[使用CLion开发Qt应用的基本方法](https://www.astercasc.com/article/detail?articleId=AT175475147755096064)
* Windows下使用`MinGW`编译`Boost`
  ，这里我将可能遇到的问题放在了[Windows下构建使用MinGW构建Boost](https://www.astercasc.com/article/detail?articleId=AT188739606681985433)
  可以参考，其他环境比较简单，参考官方文档即可
* `SQLiteCpp`就是标准的构建流程，直接拉源码，进入文件夹，
  `mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=../_install .. && cmake --build . -j $(nproc) && cmake --install . `
  即可
* OpenCV可以参考
  [Windows下使用Qt引用opencv库进行二维码识别](https://www.astercasc.com/article/detail?articleId=AT1727636818897424)

### 生产环境编译流程

交叉编译部分，我这里整体梳理了一下，记录在:

* [C++常用库交叉编译方法（一）（环境构建和Qt以及Boost）](https://www.astercasc.com/article/detail?articleId=AT188949143438677196)
* [C++常用库交叉编译方法（二）（SQLiteCpp和Breakpad）](https://www.astercasc.com/article/detail?articleId=AT189279994756125900)
* [C++常用库交叉编译方法（三）（OpenCV等视觉库）](https://www.astercasc.com/article/detail?articleId=AT190118321072192307)
* 以上提供交叉编译的构建流程，我这里已经构建完成上传到docker仓库了，使用
  `docker pull astercass/arm-gcc-8.3.0-dev-toolchain:1.0.0`拉取

