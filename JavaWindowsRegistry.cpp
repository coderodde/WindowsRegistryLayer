#include "stdafx.h"
#include <jni.h>
#include <string.h>
#include <windows.h>
#include <winnt.h>

#ifdef __cplusplus
extern "C" {
#endif
	/*
	* Class:     net_coderodde_windows_registry_WindowsRegistryLayer
	* Method:    GetSystemRegistryQuota
	* Signature: (Lnet/coderodde/windows/registry/LPDWORD;Lnet/coderodde/windows/registry/LPDWORD;)Z
	*/
	JNIEXPORT jboolean JNICALL 
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_GetSystemRegistryQuota
		(JNIEnv* env, 
		 jobject obj, // Instance method, so we have to ask for the this object.
					  // Same for all other functions in this file.
	     jobject jpdwQuotaAllowed, 
		 jobject jpwdQuotaUsed)
	{
		DWORD pdwQuotaAllowed;
		DWORD pdwQuotaUsed;
		BOOL success = GetSystemRegistryQuota(&pdwQuotaAllowed, &pdwQuotaUsed);

		if (!success) {
			return JNI_FALSE;
		}

		jclass clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "I");
		env->SetIntField(jpdwQuotaAllowed, fieldId, pdwQuotaAllowed);
		env->SetIntField(jpwdQuotaUsed, fieldId, pdwQuotaUsed);
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}

		const char* nativeLpSubKey = env->GetStringUTFChars(lpSubKey, 0);
		int ret = RegCopyTreeA((HKEY) hKeySrc, nativeLpSubKey, (HKEY) hKeyDest);
		env->ReleaseStringUTFChars(lpSubKey, nativeLpSubKey);
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		const jchar* nativeLpClass = env->GetStringChars(lpClass, 0);
		jsize nativeLpClassLength = env->GetStringLength(lpClass);
		jchar* modifiableNativeLpClass = 
			(jchar*) calloc(nativeLpClassLength + 1, sizeof(jchar));

		wcscpy_s((wchar_t*) modifiableNativeLpClass, 
			      nativeLpClassLength + 1,
			     (const wchar_t*) nativeLpClass);

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
		jclass clazzLPDWORD = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		jclass claszzPHKEY = env->FindClass("net/coderodde/windows/registry/PHKEY");
		jfieldID fieldIdLPDWORD = env->GetFieldID(clazzLPDWORD, "value", "I");
		jfieldID fieldIdPHKEY = env->GetFieldID(claszzPHKEY, "value", "I");
		env->SetIntField(lpdwDisposition, fieldIdLPDWORD, (jint) dwDisposition);
		env->SetIntField(phkResult, fieldIdPHKEY, (jint) hkResult);
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint) RegDeleteKey((HKEY)hKey, (LPCWSTR) nativeLpSubKey);  
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint)RegDeleteKeyEx(
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}
		
		if (lpValueName == NULL) {
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpValueName is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		const jchar* nativeLpValueName = env->GetStringChars(lpValueName, 0);
		jint ret = (jint)RegDeleteKeyValue(
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
		}

		const jchar* nativeLpSubKey = env->GetStringChars(lpSubKey, 0);
		jint ret = (jint)RegDeleteTree((HKEY)hKey, (LPCWSTR)nativeLpSubKey);
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpValueName is null.");
		}

		const jchar* nativeLpValueName = env->GetStringChars(lpValueName, 0);
		jint ret = (jint)RegDeleteValue((HKEY)hKey, (LPCWSTR)nativeLpValueName);
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
		if (lpName == NULL) {
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpName is null.");
		}

		if (lpcName == NULL) {
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpcName is null.");
		}

		wchar_t lpNameBuffer[255];
		wchar_t lpClassBuffer[255];

		jclass clazz = env->FindClass("net/coderodde/windows/registry/LPSTR");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "Ljava/lang/String;");
		jstring jstr = (jstring) env->GetObjectField(lpName, fieldId);

		const jchar* nativeLpName = env->GetStringChars(jstr, 0);
		jsize nativeLpNameLength = env->GetStringLength(jstr);
		jchar* modifiableNativeLpClass =
			(jchar*)calloc(nativeLpNameLength + 1, sizeof(jchar));

		wcscpy_s((wchar_t*)modifiableNativeLpClass,
			nativeLpNameLength + 1,
			(const wchar_t*)nativeLpName);

		const jchar* nativeLpClass = env->GetStringChars(lpClass, 0);

		DWORD dwlpcName = 255;
		DWORD dwlpcClass;

		clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "I");
		dwlpcName = (DWORD) env->GetIntField(lpcName, fieldId);

		bool lpClassPresent = lpClass != NULL;
		bool lpcClassPresent = lpcClass != NULL;

		if (lpClassPresent != lpcClassPresent) {
			const char* exClassName = "java/lang/IllegalArgumentException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpClass and lpcClass must be both null or both not null.");
		}

		FILETIME lpftLastWriteTimeStruct;

		jint ret = (jint)RegEnumKeyExW(
			(HKEY)hKey,
			(DWORD)dwIndex,
			(LPWSTR) modifiableNativeLpClass,
			&dwlpcName,
			NULL,
			(lpClassPresent ? lpClassBuffer : NULL),
			(lpcClassPresent ? &dwlpcClass : NULL),
			(lpftLastWriteTime != NULL ? &lpftLastWriteTimeStruct : NULL)
		);

		// Pull off: lpName, lpcName, lpClass, lpcClass, lpftLastWriteTime

		env->ReleaseStringChars(jstr, nativeLpName);
		free(modifiableNativeLpClass);
		return ret;
	}

#ifdef __cplusplus
}
#endif