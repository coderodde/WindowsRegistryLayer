package net.coderodde.windows.registry;

/**
 * This class implements the security attributes Windows structure.
 * 
 * @author Rodion "rodde" Efremov
 * @version 1.6 (Aug 17, 2017)
 */
public class SECURITY_ATTRIBUTES {
    public int nLength;
    public Object ignoredSecurityDescriptor; // Ignored. To be added later.
    public boolean bInheritHandle;
}
