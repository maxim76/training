"""
subprocess.call(args, *, stdin=None, stdout=None, stderr=None, shell=False)
Run the command described by args. Wait for command to complete, then return the returncode attribute.
"""

import subprocess
print( "Notepad will be called. Close it to continue..." )
subprocess.call(["notepad"])

subprocess.check_call()
