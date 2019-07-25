import math
import ctypes
import io

from spynnaker_acp.types import ACPAbstractType


class ACPStructProcessorType(ACPAbstractType):
    def __init__(self, x, y, p, r):
        self._val = self._factory()()
        self._val.x = x
        self._val.y = y
        self._val.p = p
        self._val.rank = r

    def __len__(self):
        return int(ctypes.sizeof(self._val))

    def _factory(self):
        class Cls(ctypes.LittleEndianStructure):
            _fields_ = [('x', ctypes.c_uint8),
                        ('y', ctypes.c_uint8),
                        ('p', ctypes.c_uint8),
                        ('rank', ctypes.c_uint32)]
        return Cls

    @property
    def value(self):
        return self._val.x, self._val.y, self._val.p, self._val.rank

    @value.setter
    def value(self, v):
        self._val.x = v[0]
        self._val.y = v[1]
        self._val.p = v[2]
        self._val.rank = v[3]

    @property
    def bytecode(self):
        bytes_stream = io.BytesIO()
        bytes_stream.write(self._val)
        bytes_stream.flush()
        bytes_stream.seek(0)

        return bytes_stream.read()

    def process_message(self, message):
        print("[MPI-TYPE-PROCESSOR] Process message")
        
        pass
