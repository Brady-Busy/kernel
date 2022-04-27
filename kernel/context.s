.global context

# This function is called to issue a system call
# Arguments are:
#  function point

context:

  # Trigger the system call interrupt
  int $0x90

  # Return from the function
  retq
