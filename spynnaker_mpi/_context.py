# -*- coding: utf-8 -*-
from __future__ import print_function
import time

from .types import ACPStructProcessorType
from spynnaker_acp import ACPContext, RemoteMemoryEntity
from spynnaker_acp.types import ACPIntegerType


class MPIContext(ACPContext):
    ACP_VARCODE_RANK = 0x00
    ACP_VARCODE_RANK_MAX = 0x01
    ACP_VARCODE_PROCESSOR = 0x0A
    ACP_VARCODE_SYNC1 = 0x05
    ACP_VARCODE_SYNC2 = 0x06
    ACP_VARCODE_SYNC3 = 0x07

    def send_rank(self):
        root_x, root_y, root_p = self._ranked_cores_dict[0]

        # --- Variables
        rank = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_RANK)
        rank_max = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_RANK_MAX)
        sync1 = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_SYNC1)
        sync2 = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_SYNC2)
        sync3 = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_SYNC3)
        processor = RemoteMemoryEntity(self._runtime, self.ACP_VARCODE_PROCESSOR)

        var_uint32 = ACPIntegerType(32, False)
        var_processor = ACPStructProcessorType(root_x, root_y, root_p, 0)

        # --- Send rank
        print("Send rank ...")
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            var_uint32.value = r
            #print("[MPI] Send runtime to {} {} {}".format(x,y,p))
            rank.update(x, y, p, var_uint32, True)
        print("Send rank DONE")

        # --- Send rank max
        print("Send rank max ...")
        var_uint32.value = self._cores_number
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            rank_max.update(x, y, p, var_uint32, True)
        print("Send rank max DONE")

        # --- Send sync1
        print("Send sync1 ...")
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            var_uint32.value = 0
            if p == 1:
                var_uint32.value = self._context_cores-1
            sync1.update(x, y, p, var_uint32, True)
        print("Send sync1 DONE")

        # --- Send sync2
        print("Send sync2 ...")
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            var_uint32.value = 0
            if p == 1 and x == y:
                var_uint32.value = self._chips_in_radius[x]-1
            sync2.update(x, y, p, var_uint32, True)
        print("Send sync2 DONE")

        # --- Send sync3
        print("Send sync3 ...")
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            var_uint32.value = 0
            if p == 1 and x == 0 and x == y:
                var_uint32.value = self._context_radius
            sync3.update(x, y, p, var_uint32, True)
        print("Send sync3 DONE")

        # --- Send Context
        print("Send context ...")
        for r in self._ranked_cores_dict.keys():
            x, y, p = self._ranked_cores_dict[r]
            var_processor.value = (x, y, p, r)
            processor.update(0, 0, 1, var_processor, True)
        print("Send context DONE")

        return
