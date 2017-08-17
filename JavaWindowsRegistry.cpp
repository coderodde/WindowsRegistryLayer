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
		 jobject obj, 
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
		Java_net_coderodde_windows_registry_WindowRegistryLayer_RegCloseKey
		(JNIEnv* env, jobject obj, jint hKey)
	{
		return RegCloseKey((HKEY) hKey);
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegCopyTree
		(JNIEnv* env, jobject obj, jint hKeySrc, jstring lpSubKey, jint hKeyDest)
	{
		const char* nativeLpSubKey = env->GetStringUTFChars(lpSubKey, 0);
		int ret = RegCopyTreeA((HKEY) hKeySrc, nativeLpSubKey, (HKEY) hKeyDest);
		env->ReleaseStringUTFChars(lpSubKey, nativeLpSubKey);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegCreateKeyEx
		(JNIEnv* env, 
		 jobject obj, // Instance method; needs the reference.
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

#ifdef __cplusplus
}
#endif