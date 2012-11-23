# Defines properties of the example/demo/application

# IPC Modes: 
#     local: only this core 
#     remote: only intra-ducati (between two M3 cores)
#     remoteWithHOST: ducati cores, dsp and host (A8)
IPC_MODE = remoteWithHOST

# Codecs required for this demo:
#     Eg: mp3dec aaclcdec...
CODECS_IN_APP = aaclcdec
# Nothing beyond this point
