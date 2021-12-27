#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "g726.h"

// 编译时Makefile里控制
#ifdef ENABLE_DEBUG
	#define DEBUG(fmt, args...)     printf(fmt, ##args)
#else
	#define DEBUG(fmt, args...)
#endif

#define BUF_SIZE 	2048	


void print_Usage(char *processName)
{
	printf("Usage: \n"
		   "   %s -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 16000 -o out_8khz_16kbps.g726\n"
		   "   %s -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 24000 -o out_8khz_24kbps.g726\n"
		   "   %s -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 32000 -o out_8khz_32kbps.g726\n"
		   "   %s -t pcm_2_g726 -i ./audio/test_8khz_16bit_mono_128kbps.pcm -r 40000 -o out_8khz_40kbps.g726\n"
		   "   %s -t g726_2_pcm -i ./audio/test_8khz_16kbps.g726 -r 16000 -o out_8khz_16bit_mono_128kbps-1.pcm\n"
		   "   %s -t g726_2_pcm -i ./audio/test_8khz_24kbps.g726 -r 24000 -o out_8khz_16bit_mono_128kbps-2.pcm\n"
		   "   %s -t g726_2_pcm -i ./audio/test_8khz_32kbps.g726 -r 32000 -o out_8khz_16bit_mono_128kbps-3.pcm\n"
		   "   %s -t g726_2_pcm -i ./audio/test_8khz_40kbps.g726 -r 40000 -o out_8khz_16bit_mono_128kbps-4.pcm\n",
		   processName, processName, processName, processName, processName, processName, processName, processName);
}


int main(int argc, char *argv[])
{
	unsigned int bitRates = 0;
	unsigned char *inBuf = (unsigned char *)malloc(BUF_SIZE);
	unsigned char *outBuf = (unsigned char *)malloc(BUF_SIZE);
	char convertType[128];
	char inputFileName[128];
	char outputFileName[128];
	FILE *fpInput = NULL;
	FILE *fpOutput = NULL;
	g726_state_t *g726Handler = NULL; // g726操作句柄

	if(argc == 1)
	{
		print_Usage(argv[0]);
		return -1;
	}

	// 解析命令行参数 -- start --
	char option = 0;
	int option_index = 0;
	const char *short_options = "ht:i:o:r:";
	struct option long_options[] =
	{
		{"help",         no_argument,       NULL, 'h'},
		{"convert_type", required_argument, NULL, 't'},
		{"input_file", 	 required_argument, NULL, 'i'},
		{"output_file",  required_argument, NULL, 'o'},
		{"bit_rates",    required_argument, NULL, 'r'},
		{NULL,           0,                 NULL,  0 },
	};  
	while((option = getopt_long_only(argc, argv, short_options, long_options, &option_index)) != -1) 
	{
		switch(option)
		{
			case 'h':
				print_Usage(argv[0]);
				return 0;
			case 't':
				strncpy(convertType, optarg, 128);
				break;
			case 'i':
				strncpy(inputFileName, optarg, 128);
				break;
			case 'o':
				strncpy(outputFileName, optarg, 128);
				break;
			case 'r':
				bitRates = atoi(optarg);
				break;
			defalut:
				printf("Unknown argument!\n");
				break;
		}
	}
	// 解析命令行参数 -- end --
	printf("\n**************************************\n"
		   "convert type: %s\n"
		   "input file name: %s\n"
		   "output file name: %s\n"
		   "g726 bit rates: %d bps\n"
		   "**************************************\n\n",
		   !strcmp(convertType, "pcm_2_g726") ? "pcm -> g726" : "g726 -> pcm",
		   inputFileName, outputFileName, bitRates);


	fpInput  = fopen(inputFileName, "rb");
	fpOutput = fopen(outputFileName, "wb");
	if(!fpInput || !fpOutput)
	{
		printf("Open Input/Output file failed!\n");
		return -1;
	}

	// step 1: 先分配内存空间给操作句柄
	g726Handler = (g726_state_t *)malloc(sizeof(g726_state_t));
	if(g726Handler == NULL)
	{
		printf("Alloc memory for g726 handler failed!\n");
		return -1;
	}

	// step 2: 根据比特率（码率）进行初始化得到句柄
	g726Handler = g726_init(g726Handler, bitRates);

	// 按一"帧"160个采样点进行操作
	#define SAMPLES_PER_FRAME 	(160)

	if(strcmp(convertType, "pcm_2_g726") == 0) // encode
	{
		int readBytes = -1;
		int ret = -1;

		while(1)
		{
			readBytes = fread(inBuf, 1, SAMPLES_PER_FRAME*(16/8), fpInput);
			if(readBytes <= 0)
				break;

			/* 参数：句柄、g726缓存（传出）、pcm缓存（传入）、pcm的采样点个数；
			 * 返回值：编码得到的g726数据长度
			 */
			ret = g726_encode(g726Handler, (unsigned char*)outBuf, (const short*)inBuf, readBytes/2); // 记得读到的字节数要除以2
			DEBUG("[g726_encode]  read pcm bytes: %d  ->  encode g726 bytes: %d\n", readBytes, ret);
			if(ret != readBytes * bitRates / 128000)
			{
				printf("PCM encode to G726 failed!\n");
				printf("\033[31mFailed!\033[0m\n");
				break;
			}

			fwrite(outBuf, 1, ret, fpOutput);
		}
	}
	else if(strcmp(convertType, "g726_2_pcm") == 0) // decode
	{
		int readBytes = -1;
		int ret = -1;

		while(1)
		{
			readBytes = fread(inBuf, 1, SAMPLES_PER_FRAME * (16/8) * bitRates / 128000, fpInput);
			if(readBytes <= 0)
				break;

			/* 参数：句柄、pcm缓存（传出）、g726缓存（传入）、g726数据长度；
			 * 返回值：pcm的采样点个数，注意不是字节数！！！所以字节数是要x2
			 */
			ret = g726_decode(g726Handler, (short*)outBuf, inBuf, readBytes);
			DEBUG("[g726_decode] read g726 bytes: %d  ->  decode pcm bytes: %d\n", readBytes, ret*2);
			if(ret*2 * bitRates / 128000 != readBytes)
			{
				printf("G726 decode to PCM failed!\n");
				printf("\033[31mFailed!\033[0m\n");
				break;
			}

			fwrite(outBuf, 2, ret, fpOutput);
		}
	}
	else
	{
		printf("Unknown convert type!\n");
		printf("\033[31mFailed!\033[0m\n");
		return -1;
	}

	printf("\033[32msuccess!\033[0m\n");

	// step : 释放句柄的内存
	free(g726Handler);

	free(inBuf);
	free(outBuf);
	fclose(fpInput);
	fclose(fpOutput);

	return 0;
}

