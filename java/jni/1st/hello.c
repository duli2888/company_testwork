#include "HelloWorld.h"

JNIEXPORT void JNICALL Java_HelloWorld_DisplayHello(JNIEnv *env, jobject obj)
{
	printf("I am from .c file\n");
	printf("Hello World\n");
	return;

}

