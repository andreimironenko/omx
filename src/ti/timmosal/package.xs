/*
 *  Copyright 2009 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== package.xs ========
 *
 */


/*
 *  ======== Package.getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 */

/*
function getLibs(prog)
{
    var pkg = this;
    var commonBld = xdc.loadCapsule("ti/omx/build/common.bld");
    var dir = xdc.getPackageBase("ti.timmosal");
    var lib = commonBld.commonGetLibs(prog, false, pkg, dir);
    return lib;
    
}
*/
/*
 *  ======== package.close ========
 */
function close()
{
}


