.code

extern ?bSendPacket@globals@@3_NA:BYTE
extern ?CreateMoveHookFunc@detours@@YAXPEAVCHLClient@@HM_N@Z:PROC

CreateMoveHookFuncNaked proc

	sub rsp, 28h
	mov ?bSendPacket@globals@@3_NA, dil
	call ?CreateMoveHookFunc@detours@@YAXPEAVCHLClient@@HM_N@Z
	mov dil, ?bSendPacket@globals@@3_NA
	add rsp, 28h
	ret

CreateMoveHookFuncNaked endp

end