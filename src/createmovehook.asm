.code

extern ?bSendPacket@globals@@3_NA:BYTE
extern ?CL_MoveRetAddr@globals@@3PEAXEA:QWORD
extern ?CreateMoveHookFunc@detours@@YAXPEAVCHLClient@@HM_N@Z:PROC

CreateMoveHookFuncNaked proc

	mov rax, [rsp + 10E0h]
	mov ?CL_MoveRetAddr@globals@@3PEAXEA, rax
	sub rsp, 28h
	mov ?bSendPacket@globals@@3_NA, dil
	call ?CreateMoveHookFunc@detours@@YAXPEAVCHLClient@@HM_N@Z
	mov dil, ?bSendPacket@globals@@3_NA
	add rsp, 28h
	mov rax, ?CL_MoveRetAddr@globals@@3PEAXEA
	mov [rsp + 10E0h], rax
	ret

CreateMoveHookFuncNaked endp

end