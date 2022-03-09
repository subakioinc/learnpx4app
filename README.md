# Learn PX4 App
 * Hello World
   * Simple C
   * Simple C++
 * Subscribe
   * simple_subscribe
 * Pub/Sub
   * mypublish
   * mysubscribe
 * MyModule
 * QGC -> Mavlink -> Subscribe [이동](QGCReceiver/README.md)
## 설치
 * Visual Studio Code 
   * https://code.visualstudio.com/download
 * Windows
   * PuTTY
   * https://www.putty.org/
 * Linux
   * picocom
```console
> sudo apt-get install picocom
```
## 환경
 * PX4 1.12.3
```console
 > git clone https://github.com/PX4/PX4-Autopilot.git Firmware
 > cd Firmware
 > git checkout v1.12.3
 > make px4_fmu-v5
 > make px4_sitl
```

## Console 연결
 * https://subak.io/?p=2157


## git 버전 표시
 * ~/.bashrc 파일 맨 마지막에 추가
```sh
> code ~/.bashrc
```

```sh
parse_git_branch() {
     git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}
export PS1="\u@\h \[\033[32m\]\w\[\033[33m\]\$(parse_git_branch)\[\033[00m\] $ "
```
