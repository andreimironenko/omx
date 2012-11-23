var MemSegDefine = xdc.loadCapsule ("MemSegmentDefinition_256MB.xs");

var soc = java.lang.System.getenv("SOC");
print ("Soc Selected: " + soc);
if (soc == 'ti814x') {
 var clock = 200.0;
 var device = "TMS320DM8148";
}
else if (soc == 'ti816x') {
var device = "TMS320TI816X";
var clock = 250.0;
}
else {
throw new Error("Unsupported value for SOC : " + soc);
}

Build.platformTable["ti.platforms.generic:DucatiPlatform_Core0"] =
{
    clockRate: clock,
    catalogName: "ti.catalog.arm.cortexm3",
    deviceName: device,
    mem: MemSegDefine.getMemSegmentDefinition(platFormMem),
    customMemoryMap: MemSegDefine.getMemSegmentDefinition(platFormMem),
    codeMemory:"VIDEO_M3_CODE",
    dataMemory:"VIDEO_M3_DATA",
    stackMemory:"VIDEO_M3_DATA"
};

Build.platformTable["ti.platforms.generic:DucatiPlatform_Core1"] =
{
    clockRate: clock,
    catalogName: "ti.catalog.arm.cortexm3",
    deviceName: device,
    mem: MemSegDefine.getMemSegmentDefinition(platFormMem),
    customMemoryMap: MemSegDefine.getMemSegmentDefinition(platFormMem),
    codeMemory:"VPSS_M3_CODE",
    dataMemory:"VPSS_M3_DATA",
    stackMemory:"VPSS_M3_DATA"
};
