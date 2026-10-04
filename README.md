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
## Lua API
### Hooks
PreCreateMove( CUserCmd cmd )  
PostCreateMove( CUserCmd cmd )  
PreFrameStageNotify( number stage )  
PostFrameStageNotify( number stage )  
PreRunCommand( Player ply, CUserCmd cmd )  
PostRunCommand( Player ply, CUserCmd cmd )  
PreDrawModelExecute( string model_name, number entity_index, number flags )  
PostDrawModelExecute( string model_name, number entity_index, number flags )  
SendNetMsg( string msgname ) -> boolean  
ShouldUpdateAnimation( Player ply ) -> boolean, number  
CL_Move() -> boolean  
Effect_&lt;name&gt;( CEffectData edata ) -> boolean  
### Functions
ServerCmd( string command, boolean reliable = true )  
ClientCmd( string command )  
GetLocalPlayer() -> number  
GetTime() -> number  
GetLastTimeStamp() -> number  
GetViewAngles() -> Angle  
SetViewAngles( Angle ang )  
IsBoxVisible( Vector mins, Vector maxs ) -> boolean  
IsBoxInViewCluster( Vector mins, Vector maxs ) -> boolean  
GetGameDirectory() -> string  
WorldToScreenMatrix() -> VMatrix  
WorldToViewMatrix() -> VMatrix  
IsOccluded( Vector mins, Vector maxs ) -> boolean  
ExecuteClientCmd( string command )  
ClientCmdUnrestricted( string command )  
SetRestrictServerCommands( boolean restrict )  
SetRestrictClientCommands( boolean restrict )  
RawClientCmdUnrestricted( string command )  
GetPreviousTick() -> number  
GetLastOutgoingCommand() -> number  
SetLastOutgoingCommand( number val )  
GetChokedCommands() -> number  
SetChokedCommands( number val )  
GetLastCommandAck() -> number  
SetLastCommandAck( number val )  
GetInterpolationTime() -> number  
GetRealTime() -> number  
SetRealTime( number val )  
GetFrameTime() -> number  
SetFrameTime( number val )  
GetAbsFrameTime() -> number  
SetAbsFrameTime( number val )  
GetCurTime() -> number  
SetCurTime( number val )  
GetFrameCount() -> number  
SetFrameCount( number val )  
GetInterpolationAmount() -> number  
SetInterpolationAmount( number val )  
ConVarSetValue( string name, number val ) -> boolean  
ConVarSetFlags( string name, number flags ) -> boolean  
SpoofConVar( string name ) -> boolean  
SpoofedConVarSetNumber( string name, number num ) -> boolean  
SetCommandNumber( CUserCmd cmd, number num )  
SetCommandTick( CUserCmd cmd, number tick )  
GetRandomSeed( CUserCmd cmd ) -> number  
SetRandomSeed( CUserCmd cmd, number seed )  
GetTyping( CUserCmd cmd ) -> boolean  
SetTyping( CUserCmd cmd, boolean typing )  
GetContextMenu( CUserCmd cmd ) -> boolean  
SetContextMenu( CUserCmd cmd, boolean enable )  
GetContextVector( CUserCmd cmd ) -> Vector  
SetContextVector( CUserCmd cmd, Vector normal )  
FindCommandNumber( CUserCmd cmd, number seed ) -> number, number  
MD5PseudoRandom( number seed ) -> number  
PredictSpread( CUserCmd cmd, Vector spread ) -> Vector  
StartPrediction( CUserCmd cmd )  
FinishPrediction()  
RunPrediction()  
StartSimulation( Player ply )  
SimulateTick()  
GetSimulationData() -> CMoveData  
FinishSimulation()  
GetBSendPacket() -> boolean  
SetBSendPacket( boolean send )  
SetInterpolation( boolean interp )  
SetSequenceInterpolation( boolean interp )  
EnableAnimFix( boolean fix )  
LoopMove()  
SetCustomDisconnect( string reason )  
DrawModelExecute()  
GetClipboardText() -> string  
ExcludeFromCapture( boolean exclude )  
NetSetConVar( string convar, string value, boolean reliable = true )  
SendAchievement( number id )  
NetDisconnect( string reason )  
GetLatency( number flow ) -> number  
GetAvgLatency( number flow ) -> number  
GetAvgLoss( number flow ) -> number  
GetAvgChoke( number flow ) -> number  
GetAvgData( number flow ) -> number  
GetAvgPackets( number flow ) -> number  
GetTotalData( number flow ) -> number  
GetSequenceNr( number flow ) -> number  
IsValidPacket( number flow, number frame_num ) -> boolean  
GetPacketTime( number flow, number frame_num ) -> number  
GetPacketBytes( number flow, number frame_num, number group ) -> number  
GetStreamProgress( number flow ) -> number, number  
GetCommandInterpolationAmount( number flow, number frame_num ) -> number  
GetPacketResponseLatency( number flow, number frame_num ) -> number, number  
GetRemoteFramerate() -> number, number  
SetDataRate( number val )  
SetTimeout( number val )  
SetChallengeNr( number val )  
NetShutdown( string reason )  
SendFile( string filename, number transfer_id ) -> boolean  
Transmit( only_reliable = false ) -> boolean  
SetFileTransmissionMode( boolean transmission )  
SetCompressionMode( boolean compression )  
RequestFile( number type, number crc ) -> number  
SetMaxBufferSize( boolean reliable, number bytes, boolean voice = false )  
GetNumBitsWritten( boolean reliable )  
SetNetInterpolationAmount( number val )  
SetRemoteFramerate( number frametime, number std_deviation )  
SetMaxRoutablePayloadSize( number val )  
GetNetName() -> string  
GetNetAddress() -> string  
GetNetTime() -> number  
GetTimeConnected() -> number  
GetBufferSize() -> number  
GetDataRate() -> number  
IsLoopback() -> boolean  
IsTimingOut() -> boolean  
IsPlayback() -> boolean  
GetTimeSinceLastReceived() -> number  
GetTimeoutSeconds() -> number  
GetChallengeNr() -> number  
CanPacket() -> boolean  
IsOverflowed() -> boolean  
IsTimedOut() -> boolean  
HasPendingReliableData() -> boolean  
IsNull() -> boolean  
GetMaxRoutablePayloadSize() -> number  
GetOutSequenceNr() -> number  
SetOutSequenceNr( number val )  
GetInSequenceNr() -> number  
SetInSequenceNr( number val )  
GetOutSequenceNrAck() -> number  
SetOutSequenceNrAck( number val )  
GetOutReliableState() -> number  
SetOutReliableState( number val )  
GetInReliableState() -> number  
SetInReliableState( number val )  
GetChokedPackets( number val )  
SetChokedPackets() -> number  
GetPacketDrop() -> number  
SetPacketDrop( number val )  
GetNetworkedVarInt( Entity ent, string table, string var ) -> number  
GetNetworkedVarFloat( Entity ent, string table, string var ) -> number  
GetNetworkedVarBool( Entity ent, string table, string var ) -> boolean  
GetNetworkedVarString( Entity ent, string table, string var ) -> string  
GetNetworkedVarVector( Entity ent, string table, string var ) -> Vector  
GetNetworkedVarAngle( Entity ent, string table, string var ) -> Angle  
GetNetworkedVarEntity( Entity ent, string table, string var ) -> Entity  
GetSimulationTime( Entity ent ) -> number  
InvalidateBoneCache( Entity ent )  
GetTargetLowerBodyYaw( Player ply ) -> number  
GetCurrentLowerBodyYaw( Player ply ) -> number  
SetTargetLowerBodyYaw( Player ply, number yaw )  
SetCurrentLowerBodyYaw( Player ply, number yaw )  
UpdateClientAnimation( Entity ent )  
UpdateAnimations( Player ply, number yaw, number pitch )  
GetTickBase( Player ply ) -> number  
SetTickBase( Player ply, number tickbase )  
PushSpecial( number type ) -> table  
