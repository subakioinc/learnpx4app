# Learn PX4 App
* Hello World
  * [Simple C](./simple_c_app/README.md)
  * [Simple C++](./simple_cpp_app/README.md)
* Subscribe
  * [simple_subscribe](./simple_subscribe/README.md)
* Pub/Sub
  * [mypublish](./mypublish/README.md)
  * [mysubscribe](./mysubscribe/README.md)
* module
  * [mymodule](./mymodule/README.md)
* module subscribe
  * [mymodule_subscribe](./mymodule_subscribe/README.md)

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
 * PX4 1.16.0
```console
> git clone https://github.com/PX4/PX4-Autopilot
> cd PX4-Autopilot
> git checkout v1.16.0
> make px4_sitl jmavsim
> make px4_fmu-v3
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
