#include <jni.h>

/*
 * Class:     HelloWorld
 * Method:    add
 * Signature: (II)I
 */

JNIEXPORT jint JNICALL Java_HelloWorld_add
  (JNIEnv *env, jclass obj, jint a, jint b)
{
	return (a + b);
}

