function getMemSegmentDefinition(platFormMem)
{
  var memory = new Array();

  memory[0] = ["VIDEO_M3_VECTOR",
  {
          name: "VIDEO_M3_VECTOR",
          base: 0x0,
          len:  0x400,
          space: "code"
  }];

  memory[1] = ["VPSS_M3_VECTOR",
  {
          name: "VPSS_M3_VECTOR",
          base: 0x400,
          len:  0x600,
          space: "code"
  }];

  memory[2] = ["DSP",
  {
          name: "DSP",
          base: 0x99500000,
          len:  0x00C00000,
          space: "code/data"
  }];

  memory[3] = ["VIDEO_M3_EVENT_BUFFER",
  {
          name: "VIDEO_M3_EVENT_BUFFER",
          base: 0x9E500000,
          len:  0x00010000,
          space: "data"
  }];
  
  memory[4] = ["VIDEO_M3_DATA0",
  {
          name: "VIDEO_M3_DATA0",
          base: 0x9E510000,
          len:  0x000F0000,
          space: "data"
  }];

  memory[5] = ["VIDEO_M3_DATA",
  {
          name: "VIDEO_M3_DATA",
          base: 0x9DD00000,
          len:  0x00630000,
          space: "data"
  }];

  memory[6] = ["VIDEO_M3_CODE",
  {
          name: "VIDEO_M3_CODE",
          base: 0x9E330000,
          len:  0x000d0000,
          space: "code"
  }];

  memory[7] = ["LOGGER_SM",
  {
          name: "LOGGER_SM",
          base: 0x9E400000,
          len:  0x00100000,
          space: "data"
  }];

  memory[8] = ["VPSS_M3_EVENT_BUFFER",
  {
          name: "VPSS_M3_EVENT_BUFFER",
          base: 0x9E600000,
          len:  0x00100000,
          space: "data"
  }];

  memory[9] = ["VPSS_M3_DATA",
  {
          name: "VPSS_M3_DATA",
          base: 0x9E700000,
          len:  0x00E90000,
          space: "data"
  }];

  memory[10] = ["VPSS_M3_CODE",
  {
          name: "VPSS_M3_CODE",
          base: 0x9F590000,
          len:  0x00170000,
          space: "code"
  }];


  memory[11] = ["IPC_SR_COMMON",
  {
          name: "IPC_SR_COMMON",
          base: 0x9F700000,
          len:  0x00200000,
          space: "data"
  }];

  memory[12] = ["HDVPSS_NOTIFY_MEM",
  {
          name: "HDVPSS_NOTIFY_MEM",
          base: 0xBF900000,
          len:  0x00200000,
          space: "data"
  }];

  memory[13] = ["HDVPSS_V4L2_FBDEF_MEM",
  {
          name: "HDVPSS_V4L2_FBDEF_MEM",
          base: 0xBFB00000,
          len:  0x00200000,
          space: "data"
  }];

  memory[14] = ["HDVPSS_DESC",
  {
          name: "HDVPSS_DESC",
          base: 0xBFD00000,
          len:  0x00200000,
          space: "data"
  }];

  memory[15] = ["MEMCFG_SPACE",
  {
          name: "MEMCFG_SPACE",
          base: 0xBFF00000,
          len:  0x000FF000,
          space: "data"
  }];
  
  memory[16] = ["VIDEO_M3_LOAD_TABLE",
  {
          name: "VIDEO_M3_LOAD_TABLE",
          base: 0xBFFFF000,
          len:  0x00000800,
          space: "data"
  }];

  memory[17] = ["VPSS_M3_LOAD_TABLE",
  {
          name: "VPSS_M3_LOAD_TABLE",
          base: 0xBFFFF800,
          len:  0x00000800,
          space: "data"
  }];

  return (memory);
}
