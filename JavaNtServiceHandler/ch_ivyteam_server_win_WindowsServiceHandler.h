/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class ch_ivyteam_server_win_WindowsServiceHandler */

#ifndef _Included_ch_ivyteam_server_win_WindowsServiceHandler
#define _Included_ch_ivyteam_server_win_WindowsServiceHandler
#ifdef __cplusplus
extern "C" {
#endif
#undef ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_AUTO
#define ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_AUTO 2L
#undef ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_MANUAL
#define ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_MANUAL 3L
#undef ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_DISABLED
#define ch_ivyteam_server_win_WindowsServiceHandler_START_TYPE_DISABLED 4L
#undef ch_ivyteam_server_win_WindowsServiceHandler_STOPPED
#define ch_ivyteam_server_win_WindowsServiceHandler_STOPPED 1L
#undef ch_ivyteam_server_win_WindowsServiceHandler_START_PENDING
#define ch_ivyteam_server_win_WindowsServiceHandler_START_PENDING 2L
#undef ch_ivyteam_server_win_WindowsServiceHandler_STOP_PENDING
#define ch_ivyteam_server_win_WindowsServiceHandler_STOP_PENDING 3L
#undef ch_ivyteam_server_win_WindowsServiceHandler_RUNNING
#define ch_ivyteam_server_win_WindowsServiceHandler_RUNNING 4L
#undef ch_ivyteam_server_win_WindowsServiceHandler_CONTINUE_PENDING
#define ch_ivyteam_server_win_WindowsServiceHandler_CONTINUE_PENDING 5L
#undef ch_ivyteam_server_win_WindowsServiceHandler_PAUSE_PENDING
#define ch_ivyteam_server_win_WindowsServiceHandler_PAUSE_PENDING 6L
#undef ch_ivyteam_server_win_WindowsServiceHandler_SERVICE_PAUSED
#define ch_ivyteam_server_win_WindowsServiceHandler_SERVICE_PAUSED 7L
/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    openService
 * Signature: (Ljava/lang/String;)Lch/ivyteam/server/win/WindowsServiceHandler;
 */
JNIEXPORT jobject JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_openService
  (JNIEnv *, jclass, jstring);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    getServiceNames
 * Signature: ()Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_getServiceNames
  (JNIEnv *, jclass);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    createService
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lch/ivyteam/server/win/WindowsServiceHandler;
 */
JNIEXPORT jobject JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_createService
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    getServiceState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_getServiceState
  (JNIEnv *, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    startService
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_startService
  (JNIEnv *, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    stopService
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_stopService
  (JNIEnv *, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_close
  (JNIEnv *, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    setStartTypeNative
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_setStartTypeNative
  (JNIEnv *, jobject, jint);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    setServiceUserNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_setServiceUserNative
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    deleteService
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_deleteService
  (JNIEnv *, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    setDependOnServices
 * Signature: (Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_setDependOnServices
  (JNIEnv *, jobject, jobject);

/*
 * Class:     ch_ivyteam_server_win_WindowsServiceHandler
 * Method:    getExitCode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_ch_ivyteam_server_win_WindowsServiceHandler_getExitCode
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
