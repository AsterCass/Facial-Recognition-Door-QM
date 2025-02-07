## 应用于Linux/Windows的人脸识别项目 - 客户端

### 前提需知

* 本客户端包括两个部分：一个是业务部分`biz`，用于实现实际的业务代码，一个是脚手架部分`airstrip`，用于存储boilerplate以及常用的工具代码，其他类似项目也可以从这个脚手架开始

### 编译流程

* 下载[Boost]((https://www.boost.org/))到本地，并构建
* 下载[SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)到本地，并构建
* 编译脚手架`airstrip`
* 下载[OpenCV](https://opencv.org/)、[InspireFace](https://github.com/HyperInspire/InspireFace)到本地
* 再编译业务部分`biz`

#### 编译提示

* Windows下使用`MinGW`编译`Boost`
  ，这里我将可能遇到的问题放在了[Windows下构建使用MinGW构建Boost](https://www.astercasc.com/article/detail?articleId=AT188739606681985433)
  可以参考，其他环境比较简单，参考官方文档即可
* `SQLiteCpp`就是标准的构建流程，直接拉源码，进入文件夹，
  `mkdir build && cd build && cmake .. && cmake --build ./ && cmake --install ./ --prefix  ../_install`即可
* 编译`airstrip`和`biz`前，记得修改`CMakeList.txt`中关于所需库位置的环境变量