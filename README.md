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
PreDrawModelExecute( number ent_index, number flags )  
PostDrawModelExecute( number ent_index, number flags )  
SendNetMsg( string msgname ) -> boolean  
ShouldUpdateAnimation( Player ply ) -> boolean, number  
OnMove() -> boolean  
OnImpact( table data )  
### Functions
ServerCmd( string command, boolean reliable )  
ClientCmd( string command )  
GetViewAngles() -> Angle  
SetViewAngles( Angle ang )  
ExecuteClientCmd( string command )  
RawClientCmdUnrestricted( string command )  
ClientCmdUnrestricted( string command )  
SetRestrictServerCommands( boolean restrict )  
SetRestrictClientCommands( boolean restrict )  
GetGameDirectory() -> string  
GetLocalPlayer() -> number  
GetTime() -> number  
GetLastTimeStamp() -> number  
IsBoxVisible( Vector mins, Vector maxs ) -> boolean  
IsBoxInViewCluster( Vector mins, Vector maxs ) -> boolean  
IsOccluded( Vector mins, Vector maxs ) -> boolean  
GetLastCommandAck() -> number  
SetLastCommandAck( number val )  
GetLastOutgoingCommand() -> number  
SetLastOutgoingCommand( number val )  
GetChokedCommands() -> number  
SetChokedCommands( number val )  
GetPreviousTick() -> number  
GetInterpolationTime() -> number  
GetCurTime() -> number  
SetCurTime( number val )  
GetFrameTime() -> number  
SetFrameTime( number val )  
GetRealTime() -> number  
SetRealTime( number val )  
GetFrameCount() -> number  
SetFrameCount( number val )  
GetAbsFrameTime() -> number  
SetAbsFrameTime( number val )  
GetInterpoloationAmount() -> number  
SetInterpoloationAmount( number val )  
ConVarSetValue( string name, number val ) -> boolean  
ConVarSetFlags( string name, number flags ) -> boolean  
SpoofConVar( string name ) -> boolean  
SpoofedConVarSetNumber( string name, number num ) -> boolean  
SetCommandNumber( CUserCmd cmd, number num )  
SetCommandTick( CUserCmd cmd, number tick )  
SetTyping( CUserCmd cmd, boolean typing )  
SetContextVector( CUserCmd cmd, Vector normal, boolean menu )  
GetRandomSeed( CUserCmd cmd ) -> number  
SetRandomSeed( CUserCmd cmd, number seed )  
PredictSpread( CUserCmd cmd, Angle ang, Vector vec ) -> Vector  
StartPrediction( CUserCmd cmd )  
FinishPrediction()  
RunPrediction()  
StartSimulation( Player ply )  
SimulateTick()  
GetSimulationData() -> table  
FinishSimulation()  
EditSimulationData( table data )  
GetBSendPacket() -> boolean  
SetBSendPacket( boolean send )  
SetInterpolation( boolean interp )  
SetSequenceInterpolation( boolean interp )  
EnableAnimFix( boolean fix )  
LoopMove()  
DrawModelExecute()  
GetClipboardText() -> string  
ExcludeFromCapture( boolean exclude )  
GetNetworkedVarInt( Entity ent, string table, string var ) -> number  
GetNetworkedVarFloat( Entity ent, string table, string var ) -> number  
GetNetworkedVarBool( Entity ent, string table, string var ) -> boolean  
GetNetworkedVarString( Entity ent, string table, string var ) -> string  
GetNetworkedVarVector( Entity ent, string table, string var ) -> Vector  
GetNetworkedVarAngle( Entity ent, string table, string var ) -> Angle  
GetNetworkedVarEnt( Entity ent, string table, string var ) -> Entity  
GetTickBase( Player ply ) -> number  
SetTickBase( Player ply, number tickbase )  
UpdateAnimations( Player ply, number yaw, number pitch )  
UpdateClientAnimation( Entity ent )  
SetCurrentLowerBodyYaw( Player ply, number yaw )  
SetTargetLowerBodyYaw( Player ply, number yaw )  
GetCurrentLowerBodyYaw( Player ply ) -> number  
GetTargetLowerBodyYaw( Player ply ) -> number  
GetSimulationTime( Entity ent ) -> number  
InvalidateBoneCache( Entity ent )  
GetNetName() -> string  
GetNetAdress() -> string  
GetNetTime() -> number  
GetNetTimeConnected() -> number  
GetNetBufferSize() -> number  
GetNetDataRate() -> number  
GetIsLoopback() -> boolean  
GetIsTimingOut() -> boolean  
SetOutSequenceNr( number val )  
SetTimeout( number val )  
NetShutdownStr( string reason )  
SetMaxRoutablePayloadSize( number val )  
GetOutSequenceNr() -> number  
SetRemoteFramerate( number frametime, number std_deviation )  
SetInterpolationAmount( number val )  
SetCompressionMode( boolean compression )  
SetInSequenceNr( number val )  
SetChallengeNr( number val )  
SetDataRate( number val )  
GetPacketBytes( number flow, number frame_num, number group ) -> number  
GetInSequenceNr( number val )  
GetPacketTime( number flow, number frame_num ) -> number  
IsValidPacket( number flow, number frame_num ) -> boolean  
GetOutSequenceNrAck() -> number  
SetOutSequenceNrAck( number val )  
SetNetChokedPackets( number val )  
GetNetChokedPackets() -> number  
SetPacketDrop( number val )  
SetOutReliableState( number val )  
GetOutReliableState() -> number  
GetPacketDrop() -> number  
SetInReliableState( number val )  
GetInReliableState() -> number  
GetSequenceNrFlow( number flow ) -> number  
GetTotalData( number flow ) -> number  
GetAvgPackets( number flow ) -> number  
GetAvgData( number flow ) -> number  
GetAvgChoke( number flow ) -> number  
GetAvgLoss( number flow ) -> number  
GetAvgLatency( number flow ) -> number  
GetLatency( number flow ) -> number  
SendFile( string filename, number transfer_id ) -> boolean  
RequestFile( number type, number crc ) -> number  
NetDisconnect( string reason )  
NetSetConVar( string convar, string value )  
NetSetConVarUnreliable( string convar, string value )  
PushSpecial( number type ) -> table  
