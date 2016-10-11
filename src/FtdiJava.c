/*
 * FtdiJava.c
 *
 *  Created on: 2016年9月9日
 *      Author: user
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jni.h>
#include "tw_everest_fdti_FtdiJava.h"
#include "ftd2xx.h"

FT_HANDLE ftHandle;

char* read_response(){
	FT_STATUS ftStatus=20;
	HANDLE hEvent;
	DWORD EventMask;
	hEvent = CreateEvent(
	NULL,
	FALSE, // auto-reset event
	FALSE, // non-signalled state
	"");
	EventMask = FT_EVENT_RXCHAR;
	ftStatus = FT_SetEventNotification(ftHandle,EventMask,hEvent);
	WaitForSingleObject(hEvent,INFINITE);
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;
	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	char *RxBuffer=malloc((int)RxBytes);
	if (RxBytes > 0){
		DWORD BytesReceived;
		ftStatus = FT_Read(ftHandle, RxBuffer, RxBytes, &BytesReceived);
		if (ftStatus == FT_OK && ftStatus == FT_OK && FT_EVENT_RXCHAR == EventDWord) {

		}
	}
	return RxBuffer;
}

JNIEXPORT jlong JNICALL Java_tw_everest_fdti_FtdiJava_connect
  (JNIEnv *env, jobject obj){
	FT_STATUS ftStatus=20;
	ftStatus = FT_Open(0, &ftHandle);
	return ftStatus;
}

JNIEXPORT void JNICALL Java_tw_everest_fdti_FtdiJava_init(JNIEnv *env,jobject obj) {
	FT_STATUS ftStatus=20;
	if (ftStatus == FT_OK)
		ftStatus = FT_SetBaudRate(ftHandle, 57600);
	if (ftStatus == FT_OK)
		ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8,
				FT_STOP_BITS_1, FT_PARITY_NONE);
	if (ftStatus == FT_OK)
		ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0);
	if (ftStatus == FT_OK)
		ftStatus = FT_SetTimeouts(ftHandle, 10000, 1000);
	// RN2483 command
	DWORD BytesWritten;
	char TxBuffer[11] = "mac pause\r\n";
	ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer),&BytesWritten);
	if(ftStatus == FT_OK){
		printf("MAC PAUSE\n");
		char* r= read_response();
		printf("%s \n",r);
		free(r);
	}
}

JNIEXPORT jlong JNICALL Java_tw_everest_fdti_FtdiJava_disConnect(JNIEnv *env, jobject obj){
	FT_STATUS ftStatus=20;
	DWORD BytesWritten;
	char TxBuffer[12] = "mac resume\r\n";
	ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer),&BytesWritten);
	if(ftStatus == FT_OK){
		char* r= read_response();
		printf("%s \n",r);
		printf("MAC RESUME\n");
		free(r);
	}
	ftStatus=FT_Close(ftHandle);
	return ftStatus;
}

JNIEXPORT jint JNICALL Java_tw_everest_fdti_FtdiJava_nuOfdevice(JNIEnv *env,jobject obj) {
	FT_STATUS ftStatus=20;
	DWORD numDevs;
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK) {

	}else{
		numDevs=0;
	}

	return numDevs;
}

JNIEXPORT jstring JNICALL Java_tw_everest_fdti_FtdiJava_listen
  (JNIEnv *env, jobject obj){
	FT_STATUS ftStatus=20;
	DWORD BytesWritten;
	char TxBuffer[12] = "radio rx 0\r\n";
	ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer),&BytesWritten);
	if(ftStatus == FT_OK){
		char* r= read_response();
		printf("%s \n",r);
		if(strstr(r,"ok")>0){
			char* r1= read_response();
			printf("%s \n",r1);
			jstring jstrBuf = (*env)->NewStringUTF(env, r1);
			free(r);
			free(r1);
			return jstrBuf;
		}
		free(r);
	}
	return NULL;
}


JNIEXPORT void JNICALL Java_tw_everest_fdti_FtdiJava_sendString(JNIEnv *env,
		jobject obj, jstring str) {
	jsize ln = (*env)->GetStringLength(env, str);
	printf("length:%d\n", (int)ln);
	if (ln > 0) {
		const char* data = (*env)->GetStringUTFChars(env, str, NULL);
		char TxBuffer[ln+11];
		sprintf(TxBuffer, "radio tx %s\r\n", data);
		//printf("%s\n",TxBuffer);
		printf("%s  %I64d\n",TxBuffer,sizeof(TxBuffer));
		FT_STATUS ftStatus=20;
		DWORD BytesWritten;
		ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer),&BytesWritten);
		if (ftStatus == FT_OK){
			printf("SEND DATA\n");
			char* r= read_response();
			printf("%s \n",r);
			if(strstr(r,"ok")>0){
				char* r1= read_response();
				printf("%s \n",r1);
				free(r1);
			}
			free(r);
		}
		(*env)->ReleaseStringUTFChars(env, str, data);
	}
}

JNIEXPORT jstring JNICALL Java_tw_everest_fdti_FtdiJava_getSysVer
  (JNIEnv *env, jobject obj){
	FT_STATUS ftStatus=20;
	DWORD BytesWritten;
	char TxBuffer[13] = "sys get ver\r\n";
	ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer),&BytesWritten);

	if(ftStatus == FT_OK){
		char* r= read_response();
		printf("%s \n",r);
		jstring jstrBuf = (*env)->NewStringUTF(env, r);
		free(r);
		return jstrBuf;
	}

	return NULL;
}

