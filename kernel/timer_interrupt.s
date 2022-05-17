.global timer_interrupt

# This function is called to issue a system call
# Arguments are:
#  function point

timer_interrupt:

  # Trigger the system call interrupt
  int $0x20

  # Return from the function
  retq
