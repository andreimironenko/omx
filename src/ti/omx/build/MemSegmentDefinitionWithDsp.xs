function getMemSegmentDefinition(platFormMem)
{
  var memory = new Array();

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
          len:  0x600,
          space: "code"
  }];

  memory[2] = ["L2_SRAM",
  {
          name: "L2_SRAM",
          base: 0x55020000 + 0x0A00,
          len:  (0x40000 - 0x0A00),
          space: "code/data"
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
          len:  0x10000000,
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
//          len:  0x00800000,
          len:  0x00100000,  // 1MB; saved 6MB (after allocating 1MB to EXTMEM_CORE2)
          space: "code"
  }];

// EXTMEM_CORE2 -  base: 0x89100000,  len:  0x00100000, - code

  memory[10] = ["PRIVATE_CORE0_DATA",
  {
          name: "PRIVATE_CORE0_DATA",
//          base: 0x89800000,
//          len:  0x08000000,
          base: 0x89200000,
          len:  0x06800000, // saved 0x18 MB
          space: "data"
  }];


  memory[11] = ["EXTMEM_CORE1",
  {
          name: "EXTMEM_CORE1",
//          base: 0x91800000,
//          len:  0x00800000,
          base: 0x8FA00000,
          len:  0x00100000, // saved 7MB
          space: "code"
  }];

  memory[12] = ["PRIVATE_CORE1_DATA",
  {
          name: "PRIVATE_CORE1_DATA",
//          base: 0x92000000,
//          len:  0x08000000,
          base: 0x8FB00000,
          len:  0x06000000, // saved 0x20 MB
          space: "data"
  }];

//  PRIVATE_CORE2_DATA    base: 0x95B00000,  len:  0x04500000, - data

  memory[13] = ["SHARED_CTRL_DUCATI_ONLY_ACCESS",
  {
          name: "SHARED_CTRL_DUCATI_ONLY_ACCESS",
          base: 0x9A000000,
          len:  0x00B00000,
          space: "data"
  }];

  memory[14] = ["SHARED_DATA",
  {
          name: "SHARED_DATA",
          base: 0x9AB00000,
          len:  0x00100000,
          space: "data"
  }];


  memory[15] = ["EVENT_LIST_CORE0",
  {
          name: "EVENT_LIST_CORE0",
          base: 0x9AC00000,
//          len:  0x00A00000,
          len:  0x00500000,          // saved 5MB (given to EVENT_LIST_CORE2)
          space: "data"
  }];

// EVENT_LIST_CORE2   base: 0x9B100000,    len:  0x00500000 - data

  memory[16] = ["EVENT_LIST_CORE1",
  {
          name: "EVENT_LIST_CORE1",
          base: 0x9B600000,
          len:  0x00A00000,
          space: "data"
  }];


  memory[17] = ["BITSTRM_DEBUG_BUF",
  {
          name: "BITSTRM_DEBUG_BUF",
          base: 0x9C000000,
          len:  0x04000000,
          space: "data"
  }];


  memory[18] = ["TILE8_PHY_ADDR",
  {
          name: "TILE8_PHY_ADDR",
          base: 0xA0000000,
          len:  0x08000000,
          space: "data"
  }];

  memory[19] = ["TILE16_PHY_ADDR",
  {
          name: "TILE16_PHY_ADDR",
          base: 0xA8000000,
          len:   0x08000000,
          space: "data"
  }];

  memory[20] = ["TILEPG_PHY_ADDR",
  {
          name: "TILEPG_PHY_ADDR",
          base: 0xB0000000,
          len:   0x02A00000,
          space: "data"
  }];


  memory[21] = ["HDVPSS_DESCRIPTOR_NON_CACHED",
  {
          name: "HDVPSS_DESCRIPTOR_NON_CACHED",
          base: 0xB2A00000,
          len:  0x00200000,
          space: "code"
  }];


  memory[22] = ["FBDEV_V4L2_Mem",
  {
          name: "FBDEV_V4L2_Mem",
          base: 0xB2C00000,
          len:   0x00200000,
          space: "data"
  }];

  memory[23] = ["FQMEM_BUFFERS_NON_CACHED",
  {
          name: "FQMEM_BUFFERS_NON_CACHED",
          base: 0xB2E00000,
          len:   0x0D200000,
          space: "data"
  }];

  memory[24] = ["EXTMEM_CORE2",
  {
          name: "EXTMEM_CORE2",
          base: 0x89100000,
          len:  0x00100000,
          space: "code"
  }];

  memory[25] = ["PRIVATE_CORE2_DATA",
  {
          name: "PRIVATE_CORE2_DATA",
          base: 0x95B00000,
          len:  0x04500000,
          space: "data"
  }];

  memory[26] = ["EVENT_LIST_CORE2",
  {
          name: "EVENT_LIST_CORE2",
          base: 0x9B100000,
          len:  0x00500000,
          space: "data"
  }];

  return (memory);
}

