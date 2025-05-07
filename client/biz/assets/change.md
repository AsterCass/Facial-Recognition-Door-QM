# 初始化

1. 进入`/oem/app`将里面文件夹下内容的`autorun.sh`改成`autorun.sh.bk`，当然，也可以都删了
2. 进入`/etc/init.d`将`S01_AIFace`和`S06_QFacialGate`删了
3. 重启
4. 时间矫正 `date 012012002025.00 &&  hwclock --systohc` # 设置时间为 2025-01-20 12:00
5. 将最新版本的对象文件放入`biz/asset/frd/bin/`下
6. 打包biz/asset传入，在根目录下解压，覆盖
7. 解压`tar -xzf /lib/libboost.tar.gz`
8. 改frpc配置，主要改`vi /data/frp/frpc.toml`，要生效需要正确配置服务端地址
9. 修改应用程序权限`chmod 755 /etc/init.d/* && chmod 755 /data/frd/bin/* && chmod 755 /data/frp/frpc`
10. 删除db内容`rm /data/frd/db/*`
11. 可以先使用 `/etc/init.d/S99zplay`中的
    `/data/frd/bin/FacialRecognitionDoor --width 800 --height 1280 --appWorkDir /data/frd/ &`确认没有问题之后再换回来
12. 手动修改进入db修改基本配置`sqlite3 /data/frd/db/common.db`，默认密码123456，其他可以在程序菜单中改，密码只能在db改
    `sqlite3 /data/frd/db/common.db "update common set v='localhost:5525' where k='serverAddress';"`
    `sqlite3 /data/frd/db/common.db "update common set v='123456' where k='managementPassword';"`
    `sqlite3 /data/frd/db/common.db "update common set v='adbcde' where k='signId';"`
    改完之后再次重启
13. 根据需求更改其他配置，比如获取任务时间间隔（同时影响断网/重连之后右上角云图标的反馈）等

## 注意事项

1. 不同版本机器嵌入位置可能不同，仅给出参考位置
2. frpc连接脚本示例，需要你的客户端服务器安装socat
    ```shell
    #!/bin/bash
    ssh -o 'proxycommand socat - PROXY:8.8.8.8:%h:%p,proxyport=9000' root@$1.domin.com
   ```