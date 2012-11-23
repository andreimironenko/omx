
var MemSegDefine = xdc.loadCapsule ("MemSegmentDefinition.xs");

var soc = java.lang.System.getenv("SOC");
print ("Soc Selected: " + soc);
if (soc == 'ti814x') {
 var device = 'TMS320DM8148';
 Build.platformTable['ti.platforms.evmDM8148'] =
  {
    l1DMode:"32k",
    l1PMode:"32k",
    l2Mode:"128k",            
    externalMemoryMap: MemSegDefine.getMemSegmentDefinition(platFormMem),
    codeMemory:"DSP",
    dataMemory:"DSP",
    stackMemory:"DSP"
  }; 
 
}
else if (soc == 'ti816x') {
var device = 'TMS320TI816X';
Build.platformTable['ti.platforms.generic:plat'] =
{
  clockRate: 800.0,
  catalogName: 'ti.catalog.c6000',
  deviceName: device,
  regs: {l1DMode: "32k"},
  regs: {l1PMode: "32k"},
  regs:  {l2Mode: "64k"},
  customMemoryMap: MemSegDefine.getMemSegmentDefinition(platFormMem),
  codeMemory:"DSP",
  dataMemory:"DSP",
  stackMemory:"DSP"
};
}
else {
throw new Error("Unsupported value for SOC : " + soc);
}

