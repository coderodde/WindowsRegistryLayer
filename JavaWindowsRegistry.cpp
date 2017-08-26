#include "stdafx.h"
#include <jni.h>
#include <string.h>
#include <windows.h>
#include <winnt.h>

#define PKG "net/coderodde/windows/registry/"
#define SIG_STRING "Ljava/lang/String;" // String signature.
#define SIG_BYTE_ARRAY "[B"
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
		LPWSTR subKeyStringBuffer = NULL;
		DWORD dwReserved = (DWORD)reserved;
		LPWSTR classStringBuffer = NULL;
		DWORD options = (DWORD)dwOptions;
		REGSAM sam = (REGSAM)samDesired;
		HKEY hkResult = 0;
		DWORD dwDisposition;

		if (lpSubKey != NULL) {
			const jchar* lpSubKeyJavaChars = env->GetStringChars(lpSubKey, NULL);
			jsize lpSubKeyJavaCharsLength = env->GetStringLength(lpSubKey);
			subKeyStringBuffer = (LPWSTR)calloc(lpSubKeyJavaCharsLength + 1, sizeof(wchar_t));
			wcscpy_s(
				(wchar_t*)subKeyStringBuffer, 
				lpSubKeyJavaCharsLength + 1,
				(const wchar_t*)lpSubKeyJavaChars);
			env->ReleaseStringChars(lpSubKey, lpSubKeyJavaChars);
		}

		if (lpClass != NULL) {
			const jchar* lpClassJavaChars = env->GetStringChars(lpClass, NULL);
			jsize lpClassJavaCharsLength = env->GetStringLength(lpClass);
			classStringBuffer = (LPWSTR)calloc(lpClassJavaCharsLength + 1, sizeof(wchar_t));
			wcscpy_s(
				(wchar_t*)classStringBuffer,
				lpClassJavaCharsLength + 1,
				(const wchar_t*)lpClassJavaChars);
			env->ReleaseStringChars(lpClass, lpClassJavaChars);
		}

		LONG ret = RegCreateKeyExW(
			(HKEY)hKey,
			subKeyStringBuffer,
			(DWORD)reserved,
			classStringBuffer,
			options,
			sam,
			NULL, // LPSECURITY_ATTRIBUTES not yet supported.
			(phkResult ? &hkResult : NULL),
			(lpdwDisposition ? &dwDisposition : NULL));

		if (subKeyStringBuffer != NULL) {
			free((void*)subKeyStringBuffer);
		}

		if (classStringBuffer != NULL) {
			free((void*)classStringBuffer);
		}

		// Fill the objects with data.
		if (phkResult != NULL) {
			setInt(
				env,
				phkResult,
				PKG "PHKEY",
				"value",
				(jint)hkResult);
		}

		if (lpdwDisposition != NULL) {
			setInt(
				env,
				lpdwDisposition,
				PKG "LPDWORD",
				"value",
				(jint)dwDisposition);
		}

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
			_In_		jint	hKey,
			_In_		jint	dwIndex,
			_Out_		jobject lpName,
			_Inout_		jobject lpcName,
			_Reserved_	jobject lpReserved,
			_Inout_		jobject lpClass,
			_Inout_opt_ jobject lpcClass,
			_Out_opt_	jobject lpftLastWriteTime) {
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
			dwlpcClass = (DWORD)getInt(env, lpcClass, PKG "LPDWORD", "value");
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
		setInt(env, lpcName, PKG "LPDWORD", "value", (jint)dwlpcName);

		// Try save lpClassBuffer:
		if (lpClassStringBuffer != NULL) {
			jstring lpClassResultValueString = env->NewString((jchar*)lpClassStringBuffer, dwlpcClass);
			setObject(env, lpClass, PKG "LPWSTR", "value", SIG_STRING, lpClassResultValueString);
		}

		// Try save lpcClass:
		if (lpcClass != NULL) {
			setInt(env, lpcClass, PKG "LPDWORD", "value", dwlpcName);
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
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegFlushKey(
			JNIEnv* env,
			jobject obj,
			jint hKey) {
		return (jint)RegFlushKey((HKEY)hKey);
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegEnumValue(
						JNIEnv* env,
						jobject obj,
			_In_		jint	hKey,
			_In_		jint	dwIndex,
			_Out_		jobject lpValueName,
			_Inout_		jobject lpcchValueName,
			_Reserved_	jobject lpReserved,
			_Out_opt_	jobject lpType,
			_Out_opt_	jobject lpData,
			_Inout_opt_ jobject lpcbData) {
		if (lpValueName == NULL) {
			return throwNullPointerException(env, "lpValueName is null.");
		}

		if (lpcchValueName == NULL) {
			return throwNullPointerException(env, "lpcchValueName is null.");
		}

		jstring lpValueNameString = NULL;
		LPWSTR lpValueNameStringBuffer = NULL;

		// Create and populate lpValueName:
		lpValueNameString = (jstring)getObject(env, lpValueName, PKG "LPWSTR", "value", SIG_STRING);
		jsize lpValueNameStringLength = env->GetStringLength(lpValueNameString);
		lpValueNameStringBuffer = (LPWSTR)calloc(lpValueNameStringLength + 1, sizeof(wchar_t));
		const jchar* lpValueNameStringChars = env->GetStringChars(lpValueNameString, NULL);

		wcscpy_s(
			lpValueNameStringBuffer,
			lpValueNameStringLength + 1,
			(wchar_t*)lpValueNameStringChars);

		env->ReleaseStringChars(lpValueNameString, lpValueNameStringChars);

		// Read lpcchValueName:
		DWORD dwlpcchValueName = (DWORD)getInt(env, lpcchValueName, PKG "LPDWORD", "value");
		DWORD dwlpcbData;
		DWORD dwReserved;
		DWORD dwType;

		LPBYTE bytes = NULL;
		jbyteArray arr = NULL;

		if (lpData != NULL) {
			arr = (jbyteArray)getObject(env, lpData, PKG "LPBYTE", "value", SIG_BYTE_ARRAY);
			jsize arrLength = env->GetArrayLength(arr);
			bytes = (LPBYTE)calloc(arrLength, sizeof(BYTE));
		}

		if (lpcbData != NULL) {
			// Read lpcbData:
			dwlpcbData = (DWORD)getInt(env, lpcbData, PKG "LPDWORD", "value");
		}

		jint ret = (jint)RegEnumValueW(
			(HKEY)hKey,
			(DWORD)dwIndex,
			lpValueNameStringBuffer,
			&dwlpcchValueName,
			(lpReserved ? &dwReserved : NULL),
			(lpType ? &dwType : NULL),
			(lpData ? bytes : NULL),
			(lpcbData ? &dwlpcbData : NULL));

		// Save lpValueName:
		jstring lpValueNameResultString = 
			env->NewString(
				(const jchar*)lpValueNameStringBuffer,
				dwlpcchValueName);
		setObject(env, lpValueName, PKG "LPWSTR", "value", SIG_STRING, lpValueNameResultString);

		// Save lpcchValueName:
		setInt(env, lpcchValueName, PKG "LPDWORD", "value", (jint)dwlpcchValueName);

		// Try save lpType:
		if (lpType != NULL) {
			setInt(env, lpType, PKG "LPDWORD", "value", (jint)dwType);
		}

		// Try save lpData:
		if (lpData != NULL) {
			jbyteArray arr = (jbyteArray)getObject(env, lpData, PKG "LPBYTE", "value", SIG_BYTE_ARRAY);
			jbyte* javaBytes = env->GetByteArrayElements(arr, NULL);
			jsize sz = env->GetArrayLength(arr);
			strcpy_s((char*) javaBytes, sz, (const char*) bytes);
			env->ReleaseByteArrayElements(arr, javaBytes, NULL);
		}

		// Try save lpcbData:
		if (lpcbData != NULL) {
			setInt(env, lpcbData, PKG "LPDWORD", "value", (jint)dwlpcbData);
		}

		if (lpValueNameStringBuffer != NULL) {
			free(lpValueNameStringBuffer);
		}
		
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegGetValue(
						JNIEnv* env,
						jobject obj,
			_In_		jint hKey,
			_In_opt_	jobject lpSubKey,
			_In_opt_	jobject lpValue,
			_In_opt_	jint dwFlags,
			_Out_opt_	jobject pdwType,
			_Out_opt_	jobject pvData,
			_Inout_opt_ jobject pcbData) {
		LPCWSTR lpSubKeyStringBuffer = NULL;
		LPCWSTR lpValueStringBuffer = NULL;
		DWORD dwType;
		PVOID data = NULL;
		jbyteArray javaDataArray = NULL;
		DWORD cbData;

		if (lpSubKey != NULL) {
			jstring lpSubKeyString = (jstring)getObject(env, lpSubKey, PKG "LPWSTR", "value", SIG_STRING);
			lpSubKeyStringBuffer = (LPCWSTR)env->GetStringChars(lpSubKeyString, NULL);
		}

		if (lpValue != NULL) {
			jstring lpValueString = (jstring)getObject(env, lpValue, PKG "LPWSTR", "value", SIG_STRING);
			lpValueStringBuffer = (LPCWSTR)env->GetStringChars(lpValueString, NULL);
		}

		if (pvData != NULL) {
			javaDataArray = (jbyteArray)getObject(env, pvData, PKG "PVOID", "value", SIG_BYTE_ARRAY);
			jsize arrLength = env->GetArrayLength(javaDataArray);
			data = (PVOID)calloc(arrLength, sizeof(BYTE));
		}

		LONG ret = RegGetValueW(
			(HKEY)hKey,
			(lpSubKey ? lpSubKeyStringBuffer : NULL),
			(lpValue ? lpValueStringBuffer : NULL),
			(DWORD)dwFlags,
			(pdwType ? &dwType : NULL),
			(pvData ? data : NULL),
			(pcbData ? &cbData : NULL));

		// Release memory:
		if (lpSubKeyStringBuffer != NULL) {
			free((void*)lpSubKeyStringBuffer);
		}

		if (lpValueStringBuffer != NULL) {
			free((void*)lpValueStringBuffer);
		}

		// Recover output data:
		if (pdwType != NULL) {
			setInt(env, pdwType, PKG "LPDWORD", "value", (jint)dwType);
		}

		if (pvData != NULL) {
			env->ReleaseByteArrayElements(javaDataArray, (jbyte*)data, 0);
		}

		if (pcbData != NULL) {
			setInt(env, pcbData, PKG "LPDWORD", "value", (jint)cbData);
		}

		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegOpenCurrentUser(
			JNIEnv* env,
			jobject obj,
			jint samDesired,
			jobject phkResult) {
		HKEY hkResult;
		LONG ret = RegOpenCurrentUser(
			(REGSAM)samDesired,
			&hkResult);

		setInt(env, phkResult, PKG "PHKEY", "value", (jint)hkResult);
		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegOpenKeyEx(
			JNIEnv* env,
			jobject obj,
			jint hKey,
			jobject lpSubKey,
			jint ulOptions,
			jint samDesired,
			jobject phkResult) {
		LPCWSTR lpSubKeyStringBuffer = NULL;
		HKEY hkeyResult;

		if (lpSubKey != NULL) {
			jstring lpSubKeyJavaString = (jstring)getObject(env, lpSubKey, PKG "LPWSTR", "value", SIG_STRING);
			lpSubKeyStringBuffer = (LPCWSTR)env->GetStringChars(lpSubKeyJavaString, NULL);
		}

		LONG ret = RegOpenKeyExW(
			(HKEY)hKey,
			lpSubKeyStringBuffer,
			(DWORD)ulOptions,
			(REGSAM)samDesired,
			&hkeyResult);

		if (phkResult != NULL) {
			setInt(env, phkResult, PKG "PHKEY", "value", (jint)hkeyResult);
		}

		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegOpenUserClassesRoot(
			JNIEnv* env,
			jobject obj,
			jint hToken,
			jint dwOptions,
			jint samDesired,
			jobject phkResult) {
		if (phkResult == NULL) {
			return throwNullPointerException(env, "phkResult is null.");
		}

		HKEY hkeyResult;

		LONG ret = RegOpenUserClassesRoot(
			(HANDLE)hToken,
			(DWORD)dwOptions,
			(REGSAM)samDesired,
			&hkeyResult);

		setInt(env, phkResult, PKG "PHKEY", "value", (jint)hkeyResult);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegQueryInfoKey(
			JNIEnv* env,
			jobject obj,
			_In_		jint hKey,
			_Out_opt_	jobject lpClass,
			_Inout_opt_ jobject lpcClass,
			_Reserved_	jobject lpReserved,
			_Out_opt_	jobject lpcSubKeys,
			_Out_opt_	jobject lpcMaxSubKeyLen,
			_Out_opt_	jobject lpcMaxClassLen,
			_Out_opt_	jobject lpcValues,
			_Out_opt_	jobject lpcMaxValueNameLen,
			_Out_opt_	jobject lpcMaxValueLen,
			_Out_opt_	jobject lpcbSecurityDescriptor,
			_Out_opt_	jobject lpftLastWriteTime) {
		DWORD dwlpcClass;
		DWORD dwlpReserved;
		DWORD dwlpcSubKeys;
		DWORD dwlpcMaxSubKeyLen;
		DWORD dwlpcMaxClassLen;
		DWORD dwlpcValues;
		DWORD dwlpcMaxValueNameLen;
		DWORD dwlpcMaxValueLen;
		DWORD dwlpcbSecurityDescriptor;
		FILETIME ftLastWriteTime;

		LPWSTR lpClassStringBuffer = NULL;

		if (lpClass != NULL) {
			jstring javaString = (jstring)getObject(env, lpClass, PKG "LPWSTR", "value", SIG_STRING);
			jsize javaStringLength = env->GetStringLength(javaString);
			const jchar* javaStringChars = env->GetStringChars(javaString, NULL);
			lpClassStringBuffer = (LPWSTR)calloc(javaStringLength + 1, sizeof(jchar));
			wcscpy_s(
				(wchar_t*)lpClassStringBuffer, 
				javaStringLength + 1, 
				(const wchar_t*)javaStringChars);
		}

		LONG ret = RegQueryInfoKeyW(
			(HKEY)hKey,
			lpClassStringBuffer,
			(lpcClass ? &dwlpcClass : NULL),
			(lpReserved ? &dwlpReserved : NULL),
			(lpcSubKeys ? &dwlpcSubKeys : NULL),
			(lpcMaxSubKeyLen ? &dwlpcMaxSubKeyLen : NULL),
			(lpcMaxClassLen ? &dwlpcMaxClassLen : NULL),
			(lpcValues ? &dwlpcValues : NULL),
			(lpcMaxValueNameLen ? &dwlpcMaxValueNameLen : NULL),
			(lpcMaxValueLen ? &dwlpcMaxValueLen : NULL),
			(lpcbSecurityDescriptor ? &dwlpcbSecurityDescriptor : NULL),
			(lpftLastWriteTime ? &ftLastWriteTime : NULL));

		// Start recovering the data:
		if (lpClassStringBuffer != NULL) {
			jstring newClassJavaString = env->NewString(
				(const jchar*)lpClassStringBuffer,
				wcslen(lpClassStringBuffer));
			setObject(env, lpClass, PKG "LPWSTR", "value", SIG_STRING, newClassJavaString);
			free((void*)lpClassStringBuffer);
		}

		if (lpcClass != NULL) {
			setInt(env, lpcClass, PKG "LPDWORD", "value", (jint)dwlpcClass);
		}

		if (lpcSubKeys != NULL) {
			setInt(env, lpcSubKeys, PKG "LPDWORD", "value", (jint)dwlpcSubKeys);
		}

		if (lpcMaxSubKeyLen != NULL) {
			setInt(env, lpcMaxSubKeyLen, PKG "LPDWORD", "value", (jint)dwlpcMaxSubKeyLen);
		}

		if (lpcMaxClassLen != NULL) {
			setInt(env, lpcMaxClassLen, PKG "LPDWORD", "value", (jint)dwlpcMaxClassLen);
		}

		if (lpcValues != NULL) {
			setInt(env, lpcValues, PKG "LPDWORD", "value", (jint)dwlpcValues);
		}

		if (lpcMaxValueNameLen != NULL) {
			setInt(env, lpcMaxValueNameLen, PKG "LPDWORD", "value", (jint)dwlpcMaxValueNameLen);
		}

		if (lpcMaxValueLen != NULL) {
			setInt(env, lpcMaxValueLen, PKG "LPDWORD", "value", (jint)dwlpcMaxValueLen);
		}

		if (lpcbSecurityDescriptor != NULL) {
			setInt(env, lpcbSecurityDescriptor, PKG "LPDWORD", "value", (jint)dwlpcbSecurityDescriptor);
		}

		if (lpftLastWriteTime != NULL) {
			setInt(env, lpftLastWriteTime, PKG "PFILETIME", "dwLowDateTime", (jint)ftLastWriteTime.dwLowDateTime);
			setInt(env, lpftLastWriteTime, PKG "PFILETIME", "dwHighDateTime", (jint)ftLastWriteTime.dwHighDateTime);
		}

		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegQueryValueEx(
						JNIEnv* env,
						jobject obj,
			_In_		jint hKey,
			_In_opt_	jobject lpValueName,
			_Reserved_	jobject lpReserved,
			_Out_opt_	jobject lpType,
			_Out_opt_	jobject lpData,
			_Inout_opt_ jobject lpcbData) {
		DWORD dwReserved;
		DWORD dwType;
		DWORD dwcbData;
		LPBYTE dataBuffer = NULL;
		LPCWSTR valueNameStringBuffer = NULL;
		jstring javaValueNameString = NULL;
		jbyteArray javaDataBytes = NULL;

		if (lpcbData != NULL) {
			dwcbData = getInt(env, lpcbData, PKG "LPDWORD", "value");
		}

		if (lpValueName != NULL) {
			javaValueNameString = 
				(jstring)getObject(
					env, 
					lpValueName, 
					PKG "LPWSTR", 
					"value", 
					SIG_STRING);
			valueNameStringBuffer = (LPCWSTR)env->GetStringChars(javaValueNameString, NULL);
		}

		if (lpData != NULL) {
			javaDataBytes = 
				(jbyteArray)getObject(
					env, 
					lpData, 
					PKG "LPBYTE", 
					"value", 
					SIG_BYTE_ARRAY);
			dataBuffer = (LPBYTE)env->GetByteArrayElements(javaDataBytes, NULL);
		}

		LONG ret = RegQueryValueEx(
			(HKEY)hKey,
			valueNameStringBuffer,
			(lpReserved ? &dwReserved : NULL),
			(lpType ? &dwType : NULL),
			(lpData ? dataBuffer : NULL),
			(lpcbData ? &dwcbData : NULL));

		if (valueNameStringBuffer != NULL) {
			free((void*)valueNameStringBuffer);
		}

		if (javaValueNameString != NULL) {
			env->ReleaseStringChars(
				javaValueNameString,
				(const jchar*)valueNameStringBuffer);
		}

		if (lpData != NULL) {
			jbyteArray newByteArray = env->NewByteArray(dwcbData);
			jbyte* newByteArrayElements = env->GetByteArrayElements(newByteArray, NULL);
			strcpy_s((char*)newByteArrayElements, (size_t)dwcbData, (const char*)dataBuffer);
			setObject(env, lpData, PKG "LPBYTE", "value", SIG_BYTE_ARRAY, newByteArray);
		}

		if (lpType != NULL) {
			setInt(env, lpType, PKG "LPDWORD", "value", (jint)dwType);
		}

		if (lpcbData != NULL) {
			setInt(env, lpcbData, PKG "LPDWORD", "value", (jint)dwcbData);
		}
		
		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegSetKeyValue(
						JNIEnv* env,
						jobject obj,
			_In_		jint hKey,
			_In_opt_	jobject lpSubKey,
			_In_opt_	jobject lpValueName,
			_In_		jint dwType,
			_In_opt_	jobject lpData,
			_In_		jint cbData) {
		LPCWSTR lpSubKeyStringBuffer = NULL;
		LPCWSTR lpValueNameStringBuffer = NULL;
		LPCVOID data = NULL;

		if (lpSubKey != NULL) {
			jstring javaSubKeyString = (jstring)getObject(
				env, 
				lpSubKey, 
				PKG "LPWSTR", 
				"value", 
				SIG_STRING);

			jsize javaStringLength = env->GetStringLength(javaSubKeyString);
			const jchar* javaStringChars = env->GetStringChars(javaSubKeyString, NULL);
			lpSubKeyStringBuffer = (LPCWSTR)calloc(javaStringLength + 1, sizeof(wchar_t));

			wcscpy_s(
				(wchar_t*)lpSubKeyStringBuffer,
				javaStringLength + 1,
				(const wchar_t*)javaStringChars);

			env->ReleaseStringChars(javaSubKeyString, javaStringChars);
		}

		if (lpValueName != NULL) {
			jstring javaValueNameString = (jstring)getObject(
				env,
				lpValueName,
				PKG "LPWSTR",
				"value",
				SIG_STRING);

			jsize javaStringLength = env->GetStringLength(javaValueNameString);
			const jchar* javaStringChars = env->GetStringChars(javaValueNameString, NULL);
			lpValueNameStringBuffer = (LPCWSTR)calloc(javaStringLength + 1, sizeof(wchar_t));

			wcscpy_s(
				(wchar_t*)lpValueNameStringBuffer,
				javaStringLength,
				(const wchar_t*)javaStringChars);
			
			env->ReleaseStringChars(javaValueNameString, javaStringChars);
		}

		if (lpData != NULL) {
			jbyteArray javaBytes = (jbyteArray)getObject(env, lpData, PKG "PVOID", "value", SIG_BYTE_ARRAY);
			jsize javaBytesLength = env->GetArrayLength(javaBytes);
			data = (LPCVOID)calloc(javaBytesLength, sizeof(BYTE));
			jbyte* rawBytes = env->GetByteArrayElements(javaBytes, NULL);
			memcpy_s((void*)data, javaBytesLength, rawBytes, javaBytesLength);
			env->ReleaseByteArrayElements(javaBytes, rawBytes, 0);
		}

		LONG ret = RegSetKeyValueW(
			(HKEY)hKey,
			(LPCWSTR)lpSubKeyStringBuffer,
			(LPCWSTR)lpValueNameStringBuffer,
			(DWORD)dwType,
			data,
			cbData);

		if (lpSubKeyStringBuffer != NULL) {
			free((void*)lpSubKeyStringBuffer);
		}

		if (lpValueNameStringBuffer != NULL) {	
			free((void*)lpValueNameStringBuffer);
		}

		if (data != NULL) {
			free((void*)data);
		}

		return (jint)ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegSetValueEx(
			JNIEnv* env,
			jobject obj,
			_In_		jint	hKey,
			_In_opt_	jobject lpValueName,
			_Reserved_  jint	Reserved,
			_In_		jint	dwType,
			_In_		jobject lpData,
			_In_		jint	cbData) {
		LPCWSTR lpValueNameStringBuffer = NULL;
		LPBYTE data = NULL;

		if (lpValueName != NULL) {
			jstring valueNameJavaString =
				(jstring)getObject(
					env,
					lpValueName,
					PKG "LPWSTR",
					"value",
					SIG_STRING);

			jsize valueNameJavaStringLength = env->GetStringLength(valueNameJavaString);
			lpValueNameStringBuffer = (LPCWSTR)calloc(valueNameJavaStringLength + 1, sizeof(wchar_t));
			const jchar* valueNameJavaStringChars = env->GetStringChars(valueNameJavaString, NULL);

			wcscpy_s(
				(wchar_t*)lpValueNameStringBuffer,
				valueNameJavaStringLength + 1,
				(const wchar_t*)valueNameJavaStringChars);

			env->ReleaseStringChars(valueNameJavaString, valueNameJavaStringChars);
		}

		if (lpData != NULL) {
			jbyteArray javaDataByteArray =
				(jbyteArray)getObject(
					env,
					lpData,
					PKG "LPBYTE",
					"value",
					SIG_BYTE_ARRAY);

			jsize javaDataByteArrayLength = env->GetArrayLength(javaDataByteArray);
			data = (LPBYTE)calloc(javaDataByteArrayLength, sizeof(BYTE));
			jbyte* javaDataByteArrayElements = env->GetByteArrayElements(javaDataByteArray, NULL);

			memcpy_s(
				data,
				javaDataByteArrayLength,
				javaDataByteArrayElements,
				javaDataByteArrayLength);

			env->ReleaseByteArrayElements(
				javaDataByteArray,
				javaDataByteArrayElements,
				0);
		}

		LONG ret = RegSetValueEx(
			(HKEY)hKey,
			(LPCWSTR)lpValueNameStringBuffer,
			(DWORD)Reserved,
			(DWORD)dwType,
			(LPBYTE)data,
			(DWORD)cbData);

		if (data != NULL) {
			free(data);
		}

		if (lpValueNameStringBuffer != NULL) {
			free((void*)lpValueNameStringBuffer);
		}

		return (jint)ret;
	}

#ifdef __cplusplus
}
#endif