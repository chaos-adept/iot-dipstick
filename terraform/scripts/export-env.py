#!/usr/bin/python
# -*- coding: utf-8 -*-

import platform
import sys
from subprocess import Popen, PIPE, STDOUT

if len(sys.argv) == 1 or len(sys.argv) >= 4:
    raise Exception('Incorrect number of params')

is_remove_mode = len(sys.argv) == 2

args = sys.argv

os_name = platform.system()
key = str(args[1]).replace('"', '').replace('-', '_').upper()
value = "" if is_remove_mode else str(args[2].replace('"', ''))

if os_name == 'Windows':
    Popen(f'setx {key} {value}"', shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=False).wait()
elif os_name == 'Darwin' or os_name == 'Linux':
    Popen(f'export {key}={value}"', shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True).wait()
