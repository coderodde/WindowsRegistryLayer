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
     * @see <a href="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms724844(v=vs.85).aspx">
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
        wrl.RegCreateKeyEx(
                HKEY_CURRENT_USER, 
                "Console\rodde", 
                0, 
                "class", 
                REG_OPTION_VOLATILE,
                0,
                null,
                phKey,
                lpDword);
    }
    
}
