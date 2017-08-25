package net.coderodde.windows.registry;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import static net.coderodde.windows.registry.WindowsRegistryLayer.*;

public class WindowsRegistryLayerTest {
    
    private static final String TEST_PATH = "WRLTests";

    private final PHKEY mainKey = new PHKEY();
    private final WindowsRegistryLayer wrl = new WindowsRegistryLayer();
   
    public WindowsRegistryLayerTest() {
        LPDWORD lpDword = new LPDWORD();
        int err = 
                wrl.RegCreateKeyEx(HKEY_CURRENT_USER,
                                   TEST_PATH,
                                   0,
                                   "myClass",
                                   REG_OPTION_VOLATILE,
                                   REGSAM.KEY_ALL_ACCESS,
                                   null,
                                   mainKey,
                                   lpDword);
        System.out.println(err);
    }
    
    @Test
    public void testRegCreateKeyEx() {
    
    }
}
