# 初始化

1. 进入`/oem/app`将里面文件夹下内容的`autorun.sh`改成`autorun.sh.bk`，当然，也可以都删了
2. 进入`/etc/init.d`将`S01_AIFace`和`S06_QFacialGate`删了
3. 重启
4. 时间矫正 `date 012012002025.00 &&  hwclock --systohc` # 设置时间为 2025-01-20 12:00
5. 将最新版本的对象文件放入`biz/asset/frd/bin/`下
6. 打包biz/asset传入，在根目录下解压，覆盖
7. 解压`tar -xzf /lib/libboost.tar.gz`
8. 参考 https://github.com/rockchip-linux/rknpu ，这里使用1.7.5的版本，将npu先换成mini驱动，再换成full驱动
   （原因不明，估计是出厂自带的驱动少东西，最好将github下载的压缩包直接传入机器，然后使用cp -r，而不是使用adb）
9. 传入 https://github.com/HyperInspire/InspireFace 中的 Gundam_RV1109 放置 /data/frd/model/下
10. 改frpc配置，主要改`vi /data/frp/frpc.toml`，要生效需要正确配置服务端地址
11. 修改应用程序权限`chmod 755 /etc/init.d/* && chmod 755 /data/frd/bin/* && chmod 755 /data/frp/frpc`
12. 删除db内容`rm /data/frd/db/*`
13. 可以先使用 `/etc/init.d/S99zplay`中的
    `/data/frd/bin/FacialRecognitionDoor --width 800 --height 1280 --appWorkDir /data/frd/ &`确认没有问题之后再换回来
14. 手动修改进入db修改基本配置`sqlite3 /data/frd/db/common.db`，默认密码123456，其他可以在程序菜单中改，密码只能在db改
    `sqlite3 /data/frd/db/common.db "update common set v='localhost:5525' where k='serverAddress';"`
    `sqlite3 /data/frd/db/common.db "update common set v='123456' where k='managementPassword';"`
    `sqlite3 /data/frd/db/common.db "update common set v='adbcde' where k='signId';"`
15. 创建`/data/frd/script/linux/common.origin.bk.db`放入相同的初始化值，也可以直接
    `cp common.db /data/frd/script/linux/common.origin.bk.db`用于提供用户恢复出场设置值
16. 根据需求更改其他配置，比如获取任务时间间隔（同时影响断网/重连之后右上角云图标的反馈）等
17. 创建定时任务
    ```shell
    cat <<'EOF' > /etc/init.d/S97cron
    #!/bin/sh
    
    case "$1" in
      start)
        rm -rf /data/frd/script/linux/live.txt
        if [ ! -d /data/cron/crontabs ]; then
            mkdir -p /data/cron/crontabs
        fi
        ln -sf /data/cron /var/spool/
        crond -b
        ;;
      stop)
        killall -q crond
        ;;
      restart)
        $0 stop
        $0 start
        ;;
      *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
    esac
    exit 0
    EOF
    chmod 755 /etc/init.d/S97cron
    ```
    重启后修改定时任务`crontab -e`
    最后输入`0 4 * * * sh /data/frd/script/linux/reboot_app.sh`以及`* * * * * sh /data/frd/script/linux/check_app_live.sh`
18.

## 注意事项

1. 不同版本机器嵌入位置可能不同，仅给出参考位置
2. frpc连接脚本示例，需要你的客户端服务器安装socat
    ```shell
    #!/bin/bash
    ssh -o 'proxycommand socat - PROXY:8.8.8.8:%h:%p,proxyport=9000' root@$1.domin.com
   ```