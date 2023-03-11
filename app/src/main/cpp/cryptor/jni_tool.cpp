#include <jni.h>
#include <string>
#include <zlib.h>
#include "base64.h"


const jint PADDING_VALUE = 10;

char *ConvertJByteaArrayToChars(JNIEnv *env, jbyteArray bytearray) {
    char *chars = NULL;
    jbyte *bytes;
    bytes = env->GetByteArrayElements(bytearray, 0);
    int chars_len = env->GetArrayLength(bytearray);
    chars = new char[chars_len + 1];
    memset(chars, 0, chars_len + 1);
    memcpy(chars, bytes, chars_len);
    chars[chars_len] = 0;
    env->ReleaseByteArrayElements(bytearray, bytes, 0);
    //
    return chars;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_nrcan_http_CryptoJNI_encrypt(JNIEnv *env, jobject clazz, jbyteArray bytes) {

    //padding
    jbyte *arr = env->GetByteArrayElements(bytes, 0);
    jsize len = env->GetArrayLength(bytes);
    for (int i = 0; i < len; ++i) {
        arr[i] += PADDING_VALUE;
    }
    jbyteArray data = env->NewByteArray(len);
    env->SetByteArrayRegion(data, 0, len, arr);
    //create ByteArrayOutputStream class
    jclass byte_class = env->FindClass("java/io/ByteArrayOutputStream");
    //method ByteArrayOutputStream init
    jmethodID byte_out_stream_init = env->GetMethodID(byte_class,
                                                      "<init>", "()V");
    //init ByteArrayOutputStream
    jobject byte_out_stream_object = env->NewObject(byte_class, byte_out_stream_init);
    //create Gzip class
    jclass gzip_class = env->FindClass("java/util/zip/GZIPOutputStream");
    //method Gzip init
    jmethodID gzip_init = env->GetMethodID(gzip_class, "<init>",
                                           "(Ljava/io/OutputStream;)V");
    //init Gzip
    jobject gzip_object = env->NewObject(gzip_class, gzip_init,
                                         byte_out_stream_object);
    //method Gzip write
    jmethodID gzip_wirte = env->GetMethodID(gzip_class, "write", "([B)V");
    //method Gzip finish
    jmethodID gzip_finish = env->GetMethodID(gzip_class, "finish", "()V");
    //method Gzip close
    jmethodID gzip_close = env->GetMethodID(gzip_class, "close", "()V");
    //Execute method(Gzip write)
    env->CallVoidMethod(gzip_object, gzip_wirte, data);
    //Execute method(Gzip finish)
    env->CallVoidMethod(gzip_object, gzip_finish);
    //Execute method(Gzip close)
    env->CallVoidMethod(gzip_object, gzip_close);
    //method ByteOutputStream toByteArray
    jmethodID byte_to_byte_array = env->GetMethodID(byte_class, "toByteArray", "()[B");
    jbyteArray byte_result = (jbyteArray) env->CallObjectMethod(byte_out_stream_object,
                                                                byte_to_byte_array);

    env->ReleaseByteArrayElements(bytes, arr, JNI_ABORT);
    env->ReleaseByteArrayElements(data, arr, JNI_ABORT);
    env->DeleteLocalRef(gzip_object);
    env->DeleteLocalRef(gzip_class);
    env->DeleteLocalRef(byte_out_stream_object);
    env->DeleteLocalRef(byte_class);
    return byte_result;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_nrcan_http_CryptoJNI_decrypt(JNIEnv *env, jobject clazz, jbyteArray array) {
    //create Gzip class
    jclass gzip_class = env->FindClass("java/util/zip/GZIPInputStream");

    //method Gzip init
    jmethodID gzip_init = env->GetMethodID(gzip_class, "<init>", "(Ljava/io/InputStream;)V");
    //method Gzip read
    jmethodID gzip_read = env->GetMethodID(gzip_class, "read", "([BII)I");

    //create ByteArrayOutputStream class
    jclass byte_out_class = env->FindClass("java/io/ByteArrayOutputStream");
    //method ByteArrayOutputStream init
    jmethodID byte_out_init = env->GetMethodID(byte_out_class, "<init>", "()V");
    //method ByteArrayOutputStream write
    jmethodID byte_out_write = env->GetMethodID(byte_out_class, "write", "([B)V");
    //method ByteArrayOutputStream toByteArray
    jmethodID byte_out_to_byte_array = env->GetMethodID(byte_out_class, "toByteArray", "()[B");

    jclass byte_in_class = env->FindClass("java/io/ByteArrayInputStream");
    jmethodID byte_in_init = env->GetMethodID(byte_in_class, "<init>", "([B)V");
    //init InputStream
    jobject bis = env->NewObject(byte_in_class, byte_in_init, array);
    //init Gzip
    jobject gzip_object = env->NewObject(gzip_class, gzip_init,
                                         bis);
    jbyteArray buf = env->NewByteArray(env->GetArrayLength(array) + 1024);
    //init ByteArrayOutputStream
    jobject byte_out_stream_object = env->NewObject(byte_out_class, byte_out_init);
    jint index = -1;
    jint total = 0;
    while ((index = env->CallIntMethod(gzip_object, gzip_read, buf, 0, env->GetArrayLength(array) + 1024)) > 0) {
        total += index;
        env->CallVoidMethod(byte_out_stream_object, byte_out_write, buf);
    }
    jbyteArray byte_result = (jbyteArray) env->CallObjectMethod(byte_out_stream_object,
                                                                byte_out_to_byte_array);
    jbyteArray new_byte_result = env->NewByteArray(total);
    jbyte *a = env->GetByteArrayElements(byte_result, 0);
    env->SetByteArrayRegion(new_byte_result, 0,
                            total, a);
    //padding
    jbyte *arr = env->GetByteArrayElements(new_byte_result, 0);
    jsize len = env->GetArrayLength(new_byte_result);
    for (int i = 0; i < len; ++i) {
        arr[i] -= PADDING_VALUE;
    }
    jbyteArray new_array = env->NewByteArray(len);
    env->SetByteArrayRegion(new_array, 0, env->GetArrayLength(new_byte_result), arr);
    char *d = ConvertJByteaArrayToChars(env, new_array);
    env->ReleaseByteArrayElements(new_array, arr, JNI_ABORT);
    env->DeleteLocalRef(byte_out_stream_object);
    env->DeleteLocalRef(byte_out_class);
    env->DeleteLocalRef(gzip_object);
    env->DeleteLocalRef(gzip_class);
    env->DeleteLocalRef(bis);
    return env->NewStringUTF(d);
}

