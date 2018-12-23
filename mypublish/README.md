# mypublish
 * subak_info topic 생성
   * /msg/subak_info.msg 위치시키기
 * examples/mypublish 폴더 만들기
   * mypub_main.cpp 만들기
   * CMakeList.txt 생성
 * mypublish 모듈 빌드 되도록 추가
   * cmake/configs/nuttx_px4fmu-v3_default.cmake
     * 맨 마지막 줄에 examples/mypublish 추가

# 확인
 * firmward upload
```console
jeyong@bar:~$ make px4fmu-v3_default upload
```
 * system console
```console
nsh> help
```
   * simple_c_app이 보이는지

 * simple_c_app 실행
```console
nsh> simple_c_app
Hello World!
```