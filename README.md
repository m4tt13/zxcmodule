# About
Cheat module for Garry's Mod (x64, Client-state)  
Author: https://github.com/serejaga

## Code example
```
require("zxcmodule")

// ConVar manipulation
ded.ConVarSetFlags( "mat_fullbright", 0 )

// Disable interp
ded.SetInterpolation(false)

// Disable sequence interp
ded.SetSequenceInterpolation(false)
```

### Lua API
ServerCmd  
ClientCmd  
SetViewAngles  
ExecuteClientCmd  
RawClientCmdUnrestricted  
ClientCmdUnrestricted  
SetRestrictServerCommands  
SetRestrictClientCommands  
GetGameDirectory  
GetLocalPlayer  
GetTime  
GetLastTimeStamp  
IsBoxVisible  
IsBoxInViewCluster  
IsOccluded  
GetLastCommandAck  
SetLastCommandAck  
GetLastOutgoingCommand  
SetLastOutgoingCommand  
GetChokedCommands  
SetChokedCommands  
GetPreviousTick  
GetInterpolationTime  
GetCurTime  
SetCurTime  
GetFrameTime  
SetFrameTime  
GetRealTime  
SetRealTime  
GetFrameCount  
SetFrameCount  
GetAbsFrameTime  
SetAbsFrameTime  
GetInterpoloationAmount  
SetInterpoloationAmount  
ConVarSetValue  
ConVarSetFlags  
SpoofConVar  
SpoofedConVarSetNumber  
SetCommandNumber  
SetCommandTick  
SetTyping  
SetContextVector  
GetRandomSeed  
SetRandomSeed  
PredictSpread  
GetServerTime  
StartPrediction  
FinishPrediction  
RunPrediction  
StartSimulation  
SimulateTick  
GetSimulationData  
FinishSimulation  
EditSimulationData  
GetBSendPacket  
SetBSendPacket  
SetInterpolation  
SetSequenceInterpolation  
EnableAnimFix  
LoopMove  
GetClipboardText  
ExcludeFromCapture  
Read  
Write  
GetNetworkedVar  
SetEntityFlags  
GetTickBase  
SetTickBase  
UpdateAnimations  
UpdateClientAnimation  
SetCurrentLowerBodyYaw  
SetTargetLowerBodyYaw  
GetCurrentLowerBodyYaw  
GetTargetLowerBodyYaw  
GetSimulationTime  
InvalidateBoneCache  
GetNetName  
GetNetAdress  
GetNetTime  
GetNetTimeConnected  
GetNetBufferSize  
GetNetDataRate  
GetIsLoopback  
GetIsTimingOut  
SetOutSequenceNr  
SetTimeout  
NetShutdownStr  
SetMaxRoutablePayloadSize  
GetOutSequenceNr  
SetRemoteFramerate  
SetInterpolationAmount  
SetCompressionMode  
SetInSequenceNr  
SetChallengeNr  
SetDataRate  
GetPacketBytes  
GetInSequenceNr  
GetPacketTime  
IsValidPacket  
GetOutSequenceNrAck  
SetOutSequenceNrAck  
SetNetChokedPackets  
GetNetChokedPackets  
SetPacketDrop  
SetOutReliableState  
GetOutReliableState  
GetPacketDrop  
SetInReliableState  
GetInReliableState  
GetSequenceNrFlow  
GetTotalData  
GetAvgPackets  
GetAvgData  
GetAvgChoke  
GetAvgLoss  
GetAvgLatency  
GetLatency  
SendFile  
RequestFile  
NetDisconnect  
NetSetConVar  
NetSetConVarUnreliable  
PushSpecial  
