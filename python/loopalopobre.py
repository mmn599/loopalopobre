import ctypes
import time
import serial
import io
import os
import pyfiglet
import time


script_dir = os.path.dirname(os.path.realpath(__file__))
lib_loc = os.path.join(script_dir, "libpedal.so")
pedal_lib = ctypes.CDLL(lib_loc)
pedal_lib.begin()


def _find_getch():
    try:
        import termios
    except ImportError:
        # Non-POSIX. Return msvcrt's (Windows') getch.
        import msvcrt
        return msvcrt.getch

    # POSIX system. Create and return a getch that manipulates the tty.
    import sys, tty
    def _getch():
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

    return _getch


getch = _find_getch()
charmap = {
    'a': 0,
    'f': 1,
    'j': 2,
    'l': 3
}


ascii_banner = pyfiglet.figlet_format("Loop a lo Pobre")
print(ascii_banner)
while True:
    val = getch()
    if val == '\x03':
        break
    if val in charmap:
        btn_id = charmap[val]
        pedal_lib.btn_pressed(btn_id)