.global create_thread

# This function is called to issue a system call
# Arguments are:
#  function point

create_thread:

  # Trigger the system call interrupt
  int $0x90

  # Return from the function
  retq
