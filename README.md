##### 前言

本demo是将使用了开源项目[EasyAACEncoder](https://github.com/EasyDarwin/EasyAACEncoder)里的[`src/g726.cpp`(demo中的已重命名为`g726.c`)](https://github.com/EasyDarwin/EasyAACEncoder/blob/master/src/g726.cpp)和[`src/g726.h`](https://github.com/EasyDarwin/EasyAACEncoder/blob/master/src/g726.h)将16位小字节序的pcm数据和g726进行相互转换。

> 注：相关测试文件已存放在demo的`audio`目录下，目前发现pcm转换得到的g726文件用软件Audacity播放不正常（没找到g726，所以选的VOX ADPCM），而将所得到的g726再转换回pcm却播放正常，网上说需要支持g726解码的播放器才能播放，用ffmpeg播放也出错，所以看到本注释时的g726都是播放不了的。

### 1、编译

```bash
$ make clean && make # 或者`make DEBUG=1`打开调试打印信息，又或者指定`CC=your-crosscompile-gcc`进行编译交叉编译
```

### 2、使用

```bash
$ ./pcm_g726_convert
Usage:
   ./pcm_g726_convert -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 16000 -o out_8khz_16kbps.g726
   ./pcm_g726_convert -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 24000 -o out_8khz_24kbps.g726
   ./pcm_g726_convert -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 32000 -o out_8khz_32kbps.g726
   ./pcm_g726_convert -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 40000 -o out_8khz_40kbps.g726
   ./pcm_g726_convert -t g726_2_pcm -i ./audio/test_8khz_16kbps.g726 -r 16000 -o out_8khz_16bit_mono_128kbps-1.pcm
   ./pcm_g726_convert -t g726_2_pcm -i ./audio/test_8khz_24kbps.g726 -r 24000 -o out_8khz_16bit_mono_128kbps-2.pcm
   ./pcm_g726_convert -t g726_2_pcm -i ./audio/test_8khz_32kbps.g726 -r 32000 -o out_8khz_16bit_mono_128kbps-3.pcm
   ./pcm_g726_convert -t g726_2_pcm -i ./audio/test_8khz_40kbps.g726 -r 40000 -o out_8khz_16bit_mono_128kbps-4.pcm
```

### 3、参考文章

 - [音频采样及编解码——LPCM 、ADPCM、G711、G726、AAC\_夜风的博客-CSDN博客\_adpcm](https://blog.csdn.net/u014470361/article/details/88837776)

 - [g726转pcm\_ybn187的专栏-CSDN博客\_g726转pcm](https://blog.csdn.net/ybn187/article/details/82995829)

 - [g726算法的一些总结\_那年晴天的博客-CSDN博客](https://blog.csdn.net/lichen18848950451/article/details/71595990)

### 附录（demo目录架构）

```bash
$ tree
.
├── audio
│   ├── test_8khz_16bit_mono_128kbps.pcm
│   ├── test_8khz_16kbps.g726
│   ├── test_8khz_24kbps.g726
│   ├── test_8khz_32kbps.g726
│   └── test_8khz_40kbps.g726
├── docs
│   ├── g726算法的一些总结_那年晴天的博客-CSDN博客.mhtml
│   ├── g726转pcm_ybn187的专栏-CSDN博客_g726转pcm.mhtml
│   └── 音频采样及编解码——LPCM 、ADPCM、G711、G726、AAC_夜风的博客-CSDN博客_adpcm.mhtml
├── g726.c
├── g726.h
├── main.c
├── Makefile
└── README.md
```
