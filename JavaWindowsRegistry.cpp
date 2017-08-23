#include "stdafx.h"
#include <jni.h>
#include <string.h>
#include <windows.h>
#include <winnt.h>

#define PKG "net/coderodde/windows/registry/"
#define SIG_STRING "Ljava/lang/String;" // String signature.
#define KEY_NAME_MAX_LENGTH 255
#define VALUE_NAME_MAX_LENGTH 16383

/*************************************************************************************
* Throws an exception of class 'exceptionClassName' with message 'exceptionMessage'. *
*************************************************************************************/
static jint throwException(
	JNIEnv* env,
	const char* exceptionClassName,
	const char* exceptionMessage) {
	jclass clazz = env->FindClass(exceptionClassName);
	return env->ThrowNew(clazz, exceptionMessage);
}

/*****************************************************************
* Throws a NullPointerException with message 'exceptionMessage'. *
*****************************************************************/
static jint throwNullPointerException(
	JNIEnv* env,
	const char* exceptionMessage
) {
	return throwException(
		env,
		"java/lang/NullPointerException",
		exceptionMessage);
}

/**********************************************************************
* Throws an IllegalArgumentException with message 'exceptionMessage'. *
**********************************************************************/
static jint throwIllegalArgumentException(
	JNIEnv* env,
	const char* exceptionMessage
) {
	return throwException(
		env,
		"java/lang/IllegalArgumentException",
		exceptionMessage
	);
}

/*********************************************************************************
* Sets an int value to object 'obj' of class 'className' into field 'fieldName'. *
*********************************************************************************/
static void setInt(
	JNIEnv* env,
	jobject obj,
	const char* className,
	const char* fieldName,
	jint value) {
	jclass clazz = env->FindClass(className);
	jfieldID fid = env->GetFieldID(clazz, fieldName, "I");
	env->SetIntField(obj, fid, value);
}

/***********************************************************************************
* Gets an int value from object 'obj' of class 'className' from field 'fieldName'. *
***********************************************************************************/
static jint getInt(
	JNIEnv* env,
	jobject obj,
	const char* className,
	const char* fieldName
) {
	jclass clazz = env->FindClass(className);
	jfieldID fid = env->GetFieldID(clazz, fieldName, "I");
	return env->GetIntField(obj, fid);
}

/**********************************************************************
* Sets an object field to 'obj' of class 'className' field 'fielName' *
* with signature 'signature'.                                         *
**********************************************************************/
static void setObject(
	JNIEnv* env,
	jobject obj,
	const char* className,
	const char* fieldName,
	const char* signature,
	jobject value) {
	jclass clazz = env->FindClass(className);
	jfieldID fid = env->GetFieldID(clazz, fieldName, signature);
	env->SetObjectField(obj, fid, value);
}

/********************************************************************
* Gets an object field named 'fieldName' from object 'obj' of class *
* 'className' having signature 'signature'.                         *
********************************************************************/
static jobject getObject(
	JNIEnv* env,
	jobject obj,
	const char* className,
	const char* fieldName,
	const char* signature
) {
	jclass clazz = env->FindClass(className);
	jfieldID fid = env->GetFieldID(clazz, fieldName, signature);
	return env->GetObjectField(obj, fid);
}

#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT jboolean JNICALL 
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_GetSystemRegistryQuota
		(JNIEnv* env, 
		 jobject obj, // Instance method, so we have to ask for the this object.
					  // Same for all other functions in this file.
	     jobject jpdwQuotaAllowed, 
		 jobject jpdwQuotaUsed)
	{
		DWORD pdwQuotaAllowed;
		DWORD pdwQuotaUsed;
		BOOL success = GetSystemRegistryQuota(&pdwQuotaAllowed, &pdwQuotaUsed);

		if (!success) {
			return JNI_FALSE;
		}

		setInt(
			env, 
			jpdwQuotaAllowed, 
			PKG "LPDWORD", 
			"value", 
			pdwQuotaAllowed);

		setInt(
			env,
			jpdwQuotaUsed,
			PKG "LPDWORD",
			"value",
			pdwQuotaUsed);

		return JNI_TRUE;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegCloseKey
		(JNIEnv* env, jobject obj, jint hKey)
	{
		return RegCloseKey((HKEY) hKey);
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegCopyTree
		(JNIEnv* env, jobject obj, jint hKeySrc, jstring lpSubKey, jint hKeyDest)
	{
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		int ret = RegCopyTreeW((HKEY) hKeySrc, (LPCWSTR)nativeLpSubKey, (HKEY) hKeyDest);
		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegCreateKeyEx
		(JNIEnv* env, 
		 jobject obj,
		 jint hKey,
		 jstring lpSubKey,
		 jint reserved,
		 jstring lpClass,
		 jint dwOptions,
		 jint samDesired,
		 jobject lpSecurityAttributes,
		 jobject phkResult,
		 jobject lpdwDisposition)
	{
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		const jchar* nativeLpClass = env->GetStringChars(lpClass, 0);
		jsize nativeLpClassLength = env->GetStringLength(lpClass);
		jchar* modifiableNativeLpClass = 
			(jchar*) calloc(nativeLpClassLength + 1, sizeof(jchar));

		wcscpy_s((LPWSTR) modifiableNativeLpClass, 
			      nativeLpClassLength + 1,
			     (LPCWSTR) nativeLpClass);

		DWORD dwDisposition;
		HKEY hkResult;

		int ret = RegCreateKeyExW(
			(HKEY) hKey,
			(LPCWSTR) nativeLpSubKey,
			0, // Reserved.
			(LPWSTR) modifiableNativeLpClass,
			dwOptions,
			samDesired,
			NULL,
			&hkResult,
			&dwDisposition);

		// Release the strings.
		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		env->ReleaseStringChars(lpClass, nativeLpClass);
		free(modifiableNativeLpClass);

		// Fill the objects with data.
		setInt(
			env,
			lpdwDisposition,
			PKG "LPDWORD",
			"value",
			(jint)dwDisposition);

		setInt(
			env,
			phkResult,
			PKG "PHKEY",
			"value",
			(jint)hkResult);

		return (jint) ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegDeleteKey
		   (JNIEnv* env,
			jobject obj,
			jint hKey,
			jstring lpSubKey)
	{
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint) RegDeleteKeyW((HKEY)hKey, (LPCWSTR) nativeLpSubKey);  
		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegDeleteKeyEx(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jstring lpSubKey,
			jint samDesired,
			jint Reserved) {
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint)RegDeleteKeyExW(
			(HKEY) hKey, 
			(LPCWSTR) nativeLpSubKey, 
			(REGSAM) samDesired, 
			(DWORD) Reserved);
		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegDeleteKeyValue(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jstring lpSubKey,
			jstring lpValueName
		) {
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}
		
		if (lpValueName == NULL) {
			return throwNullPointerException(env, "lpValueName is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		const jchar* nativeLpValueName = env->GetStringChars(lpValueName, 0);

		jint ret = (jint)RegDeleteKeyValueW(
			(HKEY)hKey,
			(LPCWSTR)nativeLpSubKey,
			(LPCWSTR)nativeLpValueName);

		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		env->ReleaseStringChars(lpValueName, nativeLpValueName);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegDeleteTree(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jstring lpSubKey) {
		if (lpSubKey == NULL) {
			return throwNullPointerException(env, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint)RegDeleteTreeW((HKEY)hKey, (LPCWSTR)nativeLpSubKey);
		env->ReleaseStringChars(lpSubKey, nativeLpSubKey);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegDeleteValue(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jstring lpValueName) {
		if (lpValueName == NULL) {
			return throwNullPointerException(env, "lpValueName is null.");
		}

		const jchar* nativeLpValueName = env->GetStringChars(lpValueName, 0);
		jint ret = (jint)RegDeleteValueW((HKEY)hKey, (LPCWSTR)nativeLpValueName);
		env->ReleaseStringChars(lpValueName, nativeLpValueName);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegEnumKeyEx(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jint dwIndex,
			jobject lpName,
			jobject lpcName,
			jobject lpReserved,
			jobject lpClass,
			jobject lpcClass,
			jobject lpftLastWriteTime) {
		if (lpcName == NULL) {
			return throwNullPointerException(env, "DWORD lpcName is null.");
		}

		jstring lpNameString = NULL;
		jstring lpClassString = NULL;

		LPWSTR lpNameStringBuffer = NULL;
		LPWSTR lpClassStringBuffer = NULL;

		bool lpClassPresent = lpClass != NULL;
		bool lpcClassPresent = lpcClass != NULL;

		DWORD dwlpcName;
		DWORD dwlpcClass;

		// Read the value of lpcName:
		dwlpcName = (DWORD)getInt(env, lpcName, PKG "LPDWORD", "value");
		
		// Try create the buffer for lpName:
		if (lpName != NULL) {
			lpNameString = (jstring)getObject(env, lpName, PKG "LPWSTR", "value", SIG_STRING);
			jsize lpNameStringLength = env->GetStringLength(lpNameString);
			lpNameStringBuffer = (LPWSTR)calloc(lpNameStringLength + 1, sizeof(wchar_t));
			const jchar* lpNameStringChars = env->GetStringChars(lpNameString, NULL);

			wcscpy_s(
				lpNameStringBuffer, 
				lpNameStringLength + 1, 
				(wchar_t*) lpNameStringChars);

			env->ReleaseStringChars(lpNameString, lpNameStringChars);
		}

		// Try create the buffer for lpClass:
		if (lpClass != NULL) {
			lpClassString = (jstring)getObject(env, lpClass, PKG "LPWSTR", "value", SIG_STRING);
			jsize lpClassStringLength = env->GetStringLength(lpClassString);
			lpClassStringBuffer = (LPWSTR)calloc(lpClassStringLength + 1, sizeof(wchar_t));
			const jchar* lpClassStringChars = env->GetStringChars(lpClassString, NULL);

			wcscpy_s(
				lpClassStringBuffer,
				lpClassStringLength + 1,
				(wchar_t*)lpClassStringChars);

			env->ReleaseStringChars(lpClassString, lpClassStringChars);
		}

		// Try read the lpcClass:
		if (lpcClass != NULL) {
			dwlpcClass = (DWORD)getInt(env, lpcClass, "LPDWORD", "value");
		}

		DWORD dwReserved;
		FILETIME fileTime;

		jint ret = (jint)RegEnumKeyExW(
			(HKEY)hKey,
			(DWORD)dwIndex,
			(lpName ? lpNameStringBuffer : NULL),
			&dwlpcName,
			(lpReserved ? &dwReserved : NULL),
			(lpClass ? lpClassStringBuffer : NULL),
			(lpcClass ? &dwlpcClass : NULL),
			(lpftLastWriteTime ? &fileTime : NULL));

		// Save lpNameBuffer:
		jstring lpNameResultValueString = env->NewString((jchar*) lpNameStringBuffer, dwlpcName);
		setObject(env, lpName, PKG "LPWSTR", "value", SIG_STRING, lpNameResultValueString);

		// Save lpcName:
		setInt(env, lpcName, "LPDWORD", "value", (jint)dwlpcName);

		// Try save lpClassBuffer:
		if (lpClassStringBuffer != NULL) {
			jstring lpClassResultValueString = env->NewString((jchar*)lpClassStringBuffer, dwlpcClass);
			setObject(env, lpClass, PKG "LPWSTR", "value", SIG_STRING, lpClassResultValueString);
		}

		// Try save lpcClass:
		if (lpcClass != NULL) {
			setInt(env, lpcClass, "LPDWORD", "value", dwlpcName);
		}

		// Try save lpftLastWriteTime:
		if (lpftLastWriteTime != NULL) {
			setInt(env, lpftLastWriteTime, PKG "PFILETIME", "dwLowDateTime", fileTime.dwLowDateTime);
			setInt(env, lpftLastWriteTime, PKG "PFILETIME", "dwHighDateTime", fileTime.dwHighDateTime);
		}

		// Release resources:
		if (lpNameStringBuffer != NULL) {
			free(lpNameStringBuffer);
		}

		if (lpClassStringBuffer != NULL) {
			free(lpClassStringBuffer);
		}

		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegEnumValue(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jint dwIndex,
			jobject lpValueName,
			jobject lpcchValueName,
			jobject lpReserved,
			jobject lpType,
			jobject lpData,
			jobject lpcbData) {
		if (lpValueName == NULL) {
			return throwNullPointerException(env, "lpValueName is null.");
		}

		if (lpcchValueName == NULL) {
			return throwNullPointerException(env, "lpcchValueName is null.");
		}

		if (lpReserved != NULL) {
			return throwIllegalArgumentException(env, "lpReserved must be null.");
		}

		if (lpData != NULL && lpcbData == NULL) {
			return throwIllegalArgumentException(
				env, 
				"lpcbData cannot be null when lpData is not null.");		
		}

		DWORD dwlpcbData;

		if (lpcbData != NULL) {
			// Read lpcbData:
			dwlpcbData = (DWORD)getInt(env, lpcbData, PKG "LPDWORD", "value");
		}

		wchar_t* valueName[32768];
		DWORD cchValueName = 32768;
		DWORD type;
		LPBYTE data = NULL;

		if (lpData != NULL) {
			data = (LPBYTE)calloc(dwlpcbData + 1, sizeof(BYTE));
		}

		jint ret = (jint)RegEnumValueW(
			(HKEY)hKey,
			(DWORD)dwIndex,
			(LPWSTR)valueName,
			(LPDWORD)&cchValueName,
			NULL,
			&type,
			data,
			(lpcbData != NULL ? &dwlpcbData : NULL)
		);

		// Now save the state:
		// Save valueName:
		jstring jValueName = env->NewString((jchar*)valueName, cchValueName);
		setObject(env, lpValueName, PKG "LPWSTR", "value", SIG_STRING, jValueName);

		// Save lpcchValueName:
		setInt(env, lpcchValueName, PKG "LPDWORD", "value", cchValueName);

		if (lpType != NULL) {
			// Save lpType:
			setInt(env, lpType, PKG "LPDWORD", "value", (jint)lpType);
		}
		
		if (lpData != NULL) {
			// Save lpData:
			jbyteArray byteArray = env->NewByteArray((jsize)dwlpcbData);
			jbyte* arrayData = env->GetByteArrayElements(byteArray, 0);
			strcpy_s((char*)arrayData, (size_t) dwlpcbData, (char*) data);
			env->ReleaseByteArrayElements(byteArray, arrayData, 0);
			free(data);
		}

		// Save lpcbData:
		if (lpcbData != NULL) {
			setInt(env, lpcbData, PKG "LPDWORD", "value", dwlpcbData);
		}

		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegFlushKey(
			JNIEnv* env,
			jobject obj,
			jint hKey) {
		return (jint)RegFlushKey((HKEY)hKey);
	}

#ifdef __cplusplus
}
#endif