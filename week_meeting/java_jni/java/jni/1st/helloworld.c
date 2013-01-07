#include <jni.h>
JNIEXPORT void JNICALL Java_HelloWorld_print_1who
  (JNIEnv *env, jclass obj)
{
	printf("hello world , come from c file\n");

}
