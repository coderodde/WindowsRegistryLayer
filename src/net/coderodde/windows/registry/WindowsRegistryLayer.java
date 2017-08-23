package net.coderodde.windows.registry;

/**
 * This class provides the access and manipulation of the Windows registry.
 * 
 * @author Rodion "rodde" Efremov
 * @version 1.6 (Aug 17, 2017)
 */
public class WindowsRegistryLayer {

    public static final int HKEY_CURRENT_USER = 0x80000001;
    
    public static final int REG_OPTION_BACKUP_RESTORE   = 0x00000004;
    public static final int REG_OPTION_CREATE_LINK      = 0x00000002;
    public static final int REG_OPTION_VOLATILE         = 0x00000001;
    
    static {
        try {
            System.loadLibrary("JavaWindowsRegistry");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Cannot load JavaWindowsRegistry.dll: " + e.getMessage());
            System.exit(1);
        }
    }
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724387(v=vs.85).aspx">
     * GetSystemRegistryQuota function</a>
     */
    public native boolean GetSystemRegistryQuota(LPDWORD pdwQuotaAllowed,
                                                 LPDWORD pdwQuotaUsed);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724837(v=vs.85).aspx">
     * RegCloseKey function</a>
     */
    public native int RegCloseKey(int hKey);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/aa379768(v=vs.85).aspx">
     * RegCopyTree function</a>
     */
    public native int RegCopyTree(int hKeySrc, String lpSubKey, int hKeyDest);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724844(v=vs.85).aspx">
     * RegCreateKeyEx function</a>
     */
    public native int RegCreateKeyEx(int hKey,
                                     String lpSubKey,
                                     int Reserved,
                                     String lpClass,
                                     int dwOptions,
                                     int samDesired,
                                     SECURITY_ATTRIBUTES lpSecurityAttributes,
                                     PHKEY phkResult,
                                     LPDWORD lpdwDisposition);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724845(v=vs.85).aspx">
     * RegDeleteKey function</a>
     */
    public native int RegDeleteKey(int hKey, String lpSubKey);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724847(v=vs.85).aspx">
     * RegDeleteKeyEx function</a>
     */
    public native int RegDeleteKeyEx(int hKey, 
                                     String lpSubKey, 
                                     int samDesired, 
                                     int Reserved);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724848(v=vs.85).aspx">
     * RegDeleteKeyValue function</a>
     */
    public native int RegDeleteKeyValue(int hKey,
                                        String lpSubKey,
                                        String lpValueName);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/aa379776(v=vs.85).aspx">
     * RegDeleteTree function</a> 
     */
    public native int RegDeleteTree(int hKey, String lpSubKey);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724851(v=vs.85).aspx">
     * RegDeleteValue function</a> 
     */
    public native int RegDeleteValue(int hKey, String lpValueName);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724862(v=vs.85).aspx">
     * RegEnumKeyEx function</a> 
     */
    public native int RegEnumKeyEx(int hKey,
                                   int dwIndex,
                                   LPWSTR lpName,
                                   LPDWORD lpcName,
                                   LPDWORD lpReserved,
                                   LPWSTR lpClass,
                                   LPDWORD lpcClass,
                                   PFILETIME lpftLastWriteTime);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724865(v=vs.85).aspx">
     * RegEnumValue function</a> 
     */
    public native int RegEnumValue(int hKey,
                                   int dwIndex,
                                   LPWSTR lpValueName,
                                   LPDWORD lpcchValueName,
                                   LPDWORD lpReserved,
                                   LPDWORD lpType,
                                   LPBYTE lpData,
                                   LPDWORD lpcbData);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724867(v=vs.85).aspx">
     * RegFlushKey function</a> 
     */
    public native int RegFlushKey(int hKey);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724868(v=vs.85).aspx">
     * RegGetValue function</a> 
     */
    public native int RegGetValue(int hKey,
                                  LPWSTR lpSubKey,
                                  LPWSTR lpValue,
                                  int dwFlags,
                                  LPDWORD pdwType,
                                  PVOID pvData,
                                  LPDWORD pcbData);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724894(v=vs.85).aspx">
     * RegOpenCurrentUser function</a> 
     */
    public native int RegOpenCurrentUser(int samDesired,
                                         PHKEY phkResult);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724897(v=vs.85).aspx">
     * RegOpenKeyEx function</a> 
     */
    public native int RegOpenKeyEx(int hKey,
                                   LPWSTR lpSubKey,
                                   int ulOptions,
                                   int samDesired,
                                   PHKEY phkResult);
    
    /**
     * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724899(v=vs.85).aspx">
     * RegOpenUserClassesRoot function</a> 
     */
    public native int RegOpenUserClassesRoot(int hToken,
                                             int dwOptions,
                                             int samDesired,
                                             PHKEY phkResult);
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        LPDWORD pdwQuotaAllowed = new LPDWORD();
        LPDWORD pdwQuotaUsed = new LPDWORD();
        WindowsRegistryLayer wrl = new WindowsRegistryLayer();
        boolean success = wrl
                .GetSystemRegistryQuota(pdwQuotaAllowed, pdwQuotaUsed);
     
        System.out.println(Integer.toUnsignedLong(pdwQuotaAllowed.value) + " " + pdwQuotaUsed.value + " " + success);
        // TODO code application logic 
        
        PHKEY phKey = new PHKEY();
        LPDWORD lpDword = new LPDWORD();
        //int ret2 = wrl.RegDeleteKey(phKey.value, "Console\\rodde");
        int ret = wrl.RegCreateKeyEx(
                HKEY_CURRENT_USER, 
                "Console\\rodde", 
                0, 
                "class", 
                REG_OPTION_VOLATILE,
                0,
                null,
                phKey,
                lpDword);
        wrl.RegCloseKey(phKey.value);
        System.out.println(ret);
    }
    
}
