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
          base: 0x89500000,
          len:  0x00C00000,
          space: "code/data"
  }];

  memory[3] = ["VIDEO_M3_EVENT_BUFFER",
  {
          name: "VIDEO_M3_EVENT_BUFFER",
          base: 0x8D500000,
          len:  0x00100000,
          space: "data"
  }];

  memory[4] = ["VIDEO_M3_DATA",
  {
          name: "VIDEO_M3_DATA",
          base: 0x8D600000,
          len:  0x00D00000,
          space: "data"
  }];

  memory[5] = ["VIDEO_M3_CODE",
  {
          name: "VIDEO_M3_CODE",
          base: 0x8E300000,
          len:  0x00100000,
          space: "code"
  }];

  memory[6] = ["LOGGER_SM",
  {
          name: "LOGGER_SM",
          base: 0x8E400000,
          len:  0x00200000,
          space: "data"
  }];

  memory[7] = ["VPSS_M3_EVENT_BUFFER",
  {
          name: "VPSS_M3_EVENT_BUFFER",
          base: 0x8E600000,
          len:  0x00100000,
          space: "data"
  }];

  memory[8] = ["VPSS_M3_DATA",
  {
          name: "VPSS_M3_DATA",
          base: 0x8E700000,
          len:  0x00E00000,
          space: "data"
  }];

  memory[9] = ["VPSS_M3_CODE",
  {
          name: "VPSS_M3_CODE",
          base: 0x8F500000,
          len:  0x00200000,
          space: "code"
  }];


  memory[10] = ["IPC_SR_COMMON",
  {
          name: "IPC_SR_COMMON",
          base: 0x8F700000,
          len:  0x00200000,
          space: "data"
  }];

  memory[11] = ["HDVPSS_NOTIFY_MEM",
  {
          name: "HDVPSS_NOTIFY_MEM",
          base: 0xAF900000,
          len:  0x00200000,
          space: "data"
  }];

  memory[12] = ["HDVPSS_V4L2_FBDEF_MEM",
  {
          name: "HDVPSS_V4L2_FBDEF_MEM",
          base: 0xAFB00000,
          len:  0x00200000,
          space: "data"
  }];

  memory[13] = ["HDVPSS_DESC",
  {
          name: "HDVPSS_DESC",
          base: 0xAFD00000,
          len:  0x00200000,
          space: "data"
  }];

  memory[14] = ["MEMCFG_SPACE",
  {
          name: "MEMCFG_SPACE",
          base: 0xAFF00000,
          len:  0x000FF000,
          space: "data"
  }];
  
  memory[15] = ["VIDEO_M3_LOAD_TABLE",
  {
          name: "VIDEO_M3_LOAD_TABLE",
          base: 0xAFFFF000,
          len:  0x00000800,
          space: "data"
  }];

  memory[16] = ["VPSS_M3_LOAD_TABLE",
  {
          name: "VPSS_M3_LOAD_TABLE",
          base: 0xAFFFF800,
          len:  0x00000800,
          space: "data"
  }];

  return (memory);
}
