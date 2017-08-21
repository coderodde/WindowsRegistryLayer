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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpSubKey is null.");
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpValueName is null.");
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

		if (lpReserved != NULL) {
			const char* exClassName = "java/lang/IllegalArgumentException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpReserved must be null.");
		}

		bool lpClassPresent = lpClass != NULL;
		bool lpcClassPresent = lpcClass != NULL;

		if (lpClassPresent != lpcClassPresent) {
			const char* exClassName = "java/lang/IllegalArgumentException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpClass and lpcClass must be both null or both not null.");
		}

		wchar_t lpNameBuffer[256];
		wchar_t lpClassBuffer[256];
		DWORD dwlpcName = 256;
		DWORD dwlpcClass = 256;

		// Get the name of the subkey:
		jclass clazz = env->FindClass("net/coderodde/windows/registry/LPWSTR");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "Ljava/lang/String;");
		jstring jstr = (jstring) env->GetObjectField(lpName, fieldId);

		// Copy the lpName to lpNameBuffer:
		const jchar* nativeLpName = env->GetStringChars(jstr, 0);
		jsize nativeLpNameLength = env->GetStringLength(jstr);
		jchar* modifiableNativeLpName =
			(jchar*)calloc(nativeLpNameLength + 1, sizeof(jchar));

		wcscpy_s((wchar_t*)modifiableNativeLpName,
			nativeLpNameLength + 1,
			(const wchar_t*)nativeLpName);

		dwlpcName = (DWORD)(nativeLpNameLength + 1);
		
		jchar* modifiableNativeLpClass = NULL;
		jstring jstr2 = NULL;
		const jchar* nativeLpClass = NULL;

		// Now, modifiableNativeLpClass contains the name of the subkey:
		if (lpClassPresent) {
			jstr2 = (jstring)env->GetObjectField(lpClass, fieldId);

			// Copy the lpClass to lpClassBuffer:
			nativeLpClass = env->GetStringChars(jstr2, 0);
			jsize nativeLpClassLength = env->GetStringLength(jstr2);
			modifiableNativeLpClass =
				(jchar*)calloc(nativeLpClassLength + 1, sizeof(jchar));

			wcscpy_s((wchar_t*)modifiableNativeLpClass,
				nativeLpClassLength + 1,
				(const wchar_t*)nativeLpClass);

			dwlpcClass = (DWORD)(nativeLpClassLength + 1);
		}

		clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "I");
		dwlpcName = (DWORD) env->GetIntField(lpcName, fieldId);

		if (lpClassPresent) {
			dwlpcClass = (DWORD)env->GetIntField(lpcClass, fieldId);
		}

		FILETIME lpftLastWriteTimeStruct;

		jint ret = (jint)RegEnumKeyExW(
			(HKEY)hKey,
			(DWORD)dwIndex,
			(LPWSTR) modifiableNativeLpName,
			&dwlpcName,
			NULL,
			(lpClassPresent ? (LPWSTR) modifiableNativeLpClass : NULL),
			(lpcClassPresent ? &dwlpcClass : NULL),
			(lpftLastWriteTime != NULL ? &lpftLastWriteTimeStruct : NULL)
		);

		// Pull off: lpName, lpcName, lpClass, lpcClass, lpftLastWriteTime
		// Save lpName:
		clazz = env->FindClass("net/coderodde/windows/registry/LPWSTR");
		fieldId = env->GetFieldID(clazz, "value", "Ljava/lang/String;");
		jstring newLpName = env->NewString(modifiableNativeLpName, dwlpcName);
		env->SetObjectField(lpName, fieldId, newLpName);
		env->ReleaseStringChars(jstr, nativeLpName);
		free(modifiableNativeLpName);
		
		// Save lpcName:
		clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		fieldId = env->GetFieldID(clazz, "value", "I");
		env->SetIntField(lpcName, fieldId, dwlpcName);

		if (lpClassPresent) {
			// Save lpClass:
			clazz = env->FindClass("net/coderodde/windows/registry/LPWSTR");
			fieldId = env->GetFieldID(clazz, "value", "Ljava/lang/String;");
			jstring newLpClass = env->NewString(modifiableNativeLpClass, dwlpcClass);
			env->SetObjectField(lpClass, fieldId, newLpClass);

			// Save lpcClass:
			clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
			fieldId = env->GetFieldID(clazz, "value", "I");
			env->SetIntField(lpcClass, fieldId, dwlpcClass);

			env->ReleaseStringChars(jstr2, nativeLpClass);
			free(modifiableNativeLpClass);
		}

		// Finally, deal with FILETIME:
		if (lpftLastWriteTime != NULL) {
			clazz = env->FindClass("net/coderodde/windows/registry/PFILETIME");
			fieldId = env->GetFieldID(clazz, "dwLowDateTime", "I");
			env->SetIntField(lpftLastWriteTime, fieldId, lpftLastWriteTimeStruct.dwLowDateTime);
			fieldId = env->GetFieldID(clazz, "dwHighDateTime", "I");
			env->SetIntField(lpftLastWriteTime, fieldId, lpftLastWriteTimeStruct.dwHighDateTime);
		}

		free(modifiableNativeLpName);
		return ret;
	}

	JNIEXPORT jint JNICALL
		Java_net_coderodde_windows_registry_WindowsRegistryLayer_RegEnumKeyEx(
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
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpValueName is null.");
		}

		if (lpcchValueName == NULL) {
			const char* exClassName = "java/lang/NullPointerException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpcchValueName is null.");
		}

		if (lpReserved != NULL) {
			const char* exClassName = "java/lang/IllegalArgumentException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpReserved must be null.");
		}

		if (lpData != NULL && lpcbData == NULL) {
			const char* exClassName = "java/lang/IllegalArgumentException";
			jclass exClass = env->FindClass(exClassName);
			return env->ThrowNew(exClass, "lpcbData cannot be null when lpData is not null.");
		}

		DWORD dwlpcbData;

		if (lpcbData != NULL) {
			// Read lpcbData:
			jclass clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
			jfieldID fieldId = env->GetFieldID(clazz, "value", "I");
			dwlpcbData = env->GetIntField(lpcbData, fieldId);
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
		jclass clazz = env->FindClass("net/coderodde/windows/registry/LPWSTR");
		jfieldID fieldId = env->GetFieldID(clazz, "value", "Ljava/lang/String;");
		jstring jValueName = env->NewString((jchar*)valueName, cchValueName);
		env->SetObjectField(lpValueName, fieldId, jValueName);

		// Save lpcchValueName:
		clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
		fieldId = env->GetFieldID(clazz, "value", "I");
		env->SetIntField(lpcchValueName, fieldId, cchValueName);

		// Save lpType:
		if (lpType != NULL) {
			env->SetIntField(lpType, fieldId, type);
		}

		// Save lpData:
		if (lpData != NULL) {
			jbyteArray byteArray = env->NewByteArray((jsize)dwlpcbData);
			jbyte* arrayData = env->GetByteArrayElements(byteArray, 0);
			strcpy_s((char*)arrayData, (size_t) dwlpcbData, (char*) data);
			env->ReleaseByteArrayElements(byteArray, arrayData, 0);
			free(data);
		}

		// Save lpcbData:
		if (lpcbData != NULL) {
			clazz = env->FindClass("net/coderodde/windows/registry/LPDWORD");
			fieldId = env->GetFieldID(clazz, "value", "I");
			env->SetIntField(lpcbData, fieldId, dwlpcbData);
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