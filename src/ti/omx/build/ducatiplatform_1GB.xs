var memory = [];

memory[0] = ["VECS_CORE_0",
{
        name: "VECS_CORE_0",
        base: 0x0,
        len:  0x400,
        space: "code"
}];

memory[1] = ["VECS_CORE_1",
{
        name: "VECS_CORE_1",
        base: 0x400,
        len:  0xc00,
        space: "code"
}];

memory[2] = ["L2_SRAM",
{
        name: "L2_SRAM",
        base: 0x1000,
        len:  (0x40000 - 0x1000),
        space: "code"
}];

memory[3] = ["OCMC_RAM0",
{
        name: "OCMC_RAM0",
        base: 0x00300000,
        len:  0x40000,
        space: "data"
}];

memory[4] = ["OCMC_RAM1",
{
        name: "OCMC_RAM1",
        base: 0x00400000,
        len:  0x40000,
        space: "data"
}];

memory[5] = ["CONFIG_REGISTER_SPACE",
{
        name: "CONFIG_REGISTER_SPACE",
        base: 0x40000000,
        len:  0x20000000,
        space: "data"
}];

memory[6] = ["TILER_SYSTEM_SPACE",
{
        name: "TILER_SYSTEM_SPACE",
        base: 0x60000000,
        len:  0x20000000,
        space: "data"
}];

memory[7] = ["LINUX_PHYSICAL_ADDRESS_GAP",
{
        name: "LINUX_PHYSICAL_ADDRESS_GAP",
        base: 0x80000000,
        len:  0x08000000,
        space: "data"
}];

memory[8] = ["SHARED_CTRL",
{
        name: "SHARED_CTRL",
        base: 0x88000000,
        len:  0x01000000,
        space: "data"
}];

memory[9] = ["EXTMEM_CORE0",
{
        name: "EXTMEM_CORE0",
        base: 0x89000000,
        len:  0x00800000,
        space: "code"
}];


memory[10] = ["EXTMEM_CORE1",
{
        name: "EXTMEM_CORE1",
        base: 0x89800000,
        len:  0x00800000,
        space: "code"
}];


memory[11] = ["EVENT_LIST_CORE0",
{
        name: "EVENT_LIST_CORE0",
        base: 0x8A000000,
        len:  0x00A00000,
        space: "data"
}];

memory[12] = ["EVENT_LIST_CORE1",
{
        name: "EVENT_LIST_CORE1",
        base: 0x8AA00000,
        len:  0x00A00000,
        space: "data"
}];

memory[13] = ["SHARED_DATA",
{
        name: "SHARED_DATA",
        base: 0x8B400000,
        len:  0x00100000,
        space: "data"
}];

memory[14] = ["BITSTRM_DEBUG_BUF",
{
        name: "BITSTRM_DEBUG_BUF",
        base: 0x8B500000,
        len:  0x04B00000,
        space: "data"
}];

memory[15] = ["PRIVATE_CORE0_DATA",
{
        name: "PRIVATE_CORE0_DATA",
        base: 0x90000000,
        len:  0x08000000,
        space: "data"
}];


memory[16] = ["PRIVATE_CORE1_DATA",
{
        name: "PRIVATE_CORE1_DATA",
        base: 0x98000000,
        len:  0x08000000,
        space: "data"
}];

var memmap_non_cached_start_offset = 0;

print ("Confguring memmap for platform: " + omx_platform);

if (omx_platform == "zebu") {
  memmap_non_cached_start_offset = 0x20000000;
}

memory[17] = ["TILE8_PHY_ADDR",
{
        name: "TILE8_PHY_ADDR",
        base: (0xA0000000 + memmap_non_cached_start_offset),
        len:  0x08000000,
        space: "data"
}];

memory[18] = ["TILE16_PHY_ADDR",
{
        name: "TILE16_PHY_ADDR",
        base: (0xA8000000 + memmap_non_cached_start_offset),
        len:   0x08000000,
        space: "data"
}];

memory[19] = ["TILEPG_PHY_ADDR",
{
        name: "TILEPG_PHY_ADDR",
        base: (0xB0000000 + memmap_non_cached_start_offset),
        len:   0x02A00000,
        space: "data"
}];


memory[20] = ["HDVPSS_DESCRIPTOR_NON_CACHED",
{
        name: "HDVPSS_DESCRIPTOR_NON_CACHED",
        base: (0xB2A00000 + memmap_non_cached_start_offset),
        len:  0x000100000,
        space: "code"
}];


memory[21] = ["FQMEM_CORE1",
{
        name: "FQMEM_CORE1",
        base: (0xB2B00000 + memmap_non_cached_start_offset),
        len:   0x09500000,
        space: "data"
}];

memory[22] = ["FQMEM_CORE0",
{
        name: "FQMEM_CORE0",
        base: (0xBC000000 + memmap_non_cached_start_offset),
        len:   0x04000000,
        space: "data"
}];


Build.platformTable["ti.platforms.generic:DucatiPlatform_Core0"] =
{
    clockRate: 250.0,
    catalogName: "ti.catalog.arm",
    deviceName: "TMS320CDM740",
    /* mem: memory, */
    customMemoryMap: memory,
    codeMemory:"EXTMEM_CORE0",
    dataMemory:"PRIVATE_CORE0_DATA",
    stackMemory:"PRIVATE_CORE0_DATA"
};

Build.platformTable["ti.platforms.generic:DucatiPlatform_Core1"] =
{
    clockRate: 250.0,
    catalogName: "ti.catalog.arm",
    deviceName: "TMS320CDM740",
    /* mem: memory, */
    customMemoryMap: memory,
    codeMemory:"EXTMEM_CORE1",
    dataMemory:"PRIVATE_CORE1_DATA",
    stackMemory:"PRIVATE_CORE1_DATA"
};

