# Learn PX4 App
 * Hello World
   * Simple C
   * Simple C++
 * Subscribe
 * Publish
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
 * PX4 1.9.0 beta2 
```console
 > git clone https://github.com/PX4/Firmware
 > cd Firmware
 > make px4_fmu-v3
```
## Console 연결
 * https://subak.io/?p=2157


## git 버전 표시
 * ~/.bashrc 파일 맨 마지막에 추가
```sh
parse_git_branch() {
     git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}
export PS1="\u@\h \[\033[32m\]\w\[\033[33m\]\$(parse_git_branch)\[\033[00m\] $ "
```
