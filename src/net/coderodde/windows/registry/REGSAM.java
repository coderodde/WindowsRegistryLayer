package net.coderodde.windows.registry;

/**
 * This class specifies all the flags for a Windows REGSAM.
 * 
 * @author Rodion "rodde" Efremov
 * @version 1.6 (Aug 17, 2017)
 * @see <a target="_blank" href="https://msdn.microsoft.com/en-us/library/windows/desktop/bb773480(v=vs.85).aspx">REGSAM</a> 
 */
public final class REGSAM {
    
    public static final int STANDARD_RIGHTS_ALL     = 0x001F0000;
    public static final int SYNCHRONIZE             = 0x00100000;
    
    public static final int KEY_QUERY_VALUE         = 0x0001;
    public static final int KEY_SET_VALUE           = 0x0002;
    public static final int KEY_CREATE_SUB_KEY      = 0x0004;
    public static final int KEY_ENUMERATE_SUB_KEYS  = 0x0008;
    public static final int KEY_NOTIFY              = 0x0010;
    public static final int KEY_CREATE_LINK         = 0x0020;
    
    public static final int KEY_READ = (STANDARD_RIGHTS_ALL |
                                        KEY_QUERY_VALUE |
                                        KEY_ENUMERATE_SUB_KEYS |
                                        KEY_NOTIFY) & SYNCHRONIZE;
    
    public static final int KEY_EXECUTE = KEY_READ & SYNCHRONIZE;
    public static final int KEY_WRITE = (STANDARD_RIGHTS_ALL |
                                         KEY_SET_VALUE |
                                         KEY_CREATE_SUB_KEY) & SYNCHRONIZE;
    
    public static final int KEY_ALL_ACCESS = (STANDARD_RIGHTS_ALL |
                                              KEY_QUERY_VALUE |
                                              KEY_ENUMERATE_SUB_KEYS |
                                              KEY_NOTIFY |
                                              KEY_CREATE_SUB_KEY |
                                              KEY_CREATE_LINK |
                                              KEY_SET_VALUE) & SYNCHRONIZE;
    private REGSAM() {}
}
