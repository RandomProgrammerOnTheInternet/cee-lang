.intel_syntax noprefix
.global _start
_start:
	mov eax, 5 # generate_bin_expr
	mov ecx, 2 # generate_bin_expr
	sub eax, ecx # generate_bin_expr
	mov dword ptr [rsp-4], eax # generate_var_decl
	mov eax, dword ptr [rsp-4] # generate_bin_expr
	mov ecx, 1 # generate_bin_expr
	sub eax, ecx # generate_bin_expr
	mov dword ptr [rsp-8], eax # generate_var_decl
	mov eax, dword ptr [rsp-8] # generate_bin_expr
	mov ecx, 3 # generate_bin_expr
	sub eax, ecx # generate_bin_expr
	mov dword ptr [rsp-8], eax # generate_assignment
	mov edi, dword ptr [rsp-8] # generate_return
	mov rax, 60 # generate_return
	syscall # generate_return
