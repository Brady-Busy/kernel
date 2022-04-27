.global context_entry
.global context_handler

# This is the interrupt handler routine called when a system call is issued
context_entry:
  push %rdx
  push %rcx
  push %rbx
  push %rax
  push %rbp
  push %r8
  push %r9
  push %r10
  push %r11
  push %r12
  push %r13
  push %r14
  push %r15
  push %rdi
  push %rsi
  mov %rsp, %rdi

  # Call the C-land syscall handler
  call context_handler
 
  pop %rsi
  pop %rdi
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %r11
  pop %r10
  pop %r9
  pop %r8
  pop %rbp
  pop %rax
  pop %rbx
  pop %rcx
  pop %rdx

  # Return from the interrupt handler
  iretq