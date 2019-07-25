# -*- coding: utf-8 -*-
from __future__ import print_function
import os
import time

from spynnaker_acp import Utils
from spynnaker_acp.spinnaker_interface \
    import CPUState, SCPSignal, FileDataReader


class MPIApp(object):
    DELAY_100ms = 100 * Utils.si['m']
    DELAY_500ms = 500 * Utils.si['m']

    STATUS_NONE = 0
    STATUS_ASSIGNED = 1
    STATUS_LAUNCHED = 2
    STATUS_STOPPED = 3

    def __init__(self, runtime, app_aplx, app_id=30):
        self._runtime = runtime
        self._app_aplx = app_aplx
        self._app_id = app_id

        self._app_size = os.stat(self._app_aplx).st_size
        self._app_executable = FileDataReader(self._app_aplx)

        self._context = None
        self._status = self.STATUS_NONE
        return

    def __str__(self):
        _str = ""
        _str += "SpiNNaker MPI runtime - APP\n"
        _str += "\taplx: %s\n" % (self.app_aplx,)
        _str += "\tid: %d\n" % (self.app_id,)
        _str += "\tstatus: %d\n" % (self.status,)
        return _str

    def init(self, context):
        if self.status != self.STATUS_NONE:
            return

        self._context = context

        # Phase 1: Load APLX
        print("[MPI_APP]: Init (1) Loading %s instance" % (self.app_aplx,))
        self._runtime.transceiver.execute_flood(
            self.context.cores_subsets,
            self.app_executable,
            self.app_id,
            self.app_size)
        time.sleep(self.DELAY_500ms)

        self._runtime.board_sync(
            self.app_id,
            self._context,
            state=CPUState.SYNC0,
            signal=SCPSignal.SYNC0)

        # Phase 2a: Enable ACP Runtime
        print("[MPI_APP]: Init (2) ACP Configuration")

        print("[MPI_APP]: Init (2a) Runtime Enabling")
        self._runtime.enable()
        self._runtime.add_reaction(0x0100, 0, lambda msg: self._runtime.signal())
        
        # Phase 2b: Start ACP Runtime
        print("[MPI_APP]: Init (2b) Runtime Starting")
        self._runtime.start()

        # Phase 2c: Send MPI Context
        print("[MPI_APP]: Init (2c) Runtime Initializing")
        self._context.send_rank()

        print("[MPI_APP]: Init DONE")
        self._status = self.STATUS_ASSIGNED
        return

    def run(self):
        if self.status != self.STATUS_ASSIGNED:
            return

        # Phase 3: Application run
        print("[MPI_APP]: Run (3) Check SYNC1 ")
        self._runtime.board_sync(
            self.app_id,
            self._context,
            state=CPUState.SYNC1,
            signal=SCPSignal.SYNC1)
        time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Run (3) Send SYNC1 ")
        self._runtime.board_sync(
            self.app_id,
            self._context,
            signal=SCPSignal.SYNC1)
        time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Run (3) DONE")
        self._status = self.STATUS_LAUNCHED
        return

    def stop(self, get_buffers=False):
        buffers = list()

        if self.status != self.STATUS_LAUNCHED:
            print("[MPI_APP]: Impossible to stop!")
            return buffers

        print("[MPI_APP]: Stop (4) Wait ...")
        self._runtime.wait()

        print("[MPI_APP]: Stop (4) Check SYNC0...")
        # Wait SYNC0 -> ... 
        self._runtime.board_sync(
            self.app_id,
            self._context,
            state=CPUState.SYNC0)
        time.sleep(self.DELAY_500ms)
        
        # TODO: Call a user defined get_data
        pass

        print("[MPI_APP]: Stop (4) Send SYNC0...")
        # Send ... -> Signal SYNC0
        self._runtime.board_sync(
            self.app_id,
            self._context,
            signal=SCPSignal.SYNC0)
        time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Stop (4) Check FINISHED...")
        # Wait FINISHED -> ... -> Signal STOP
        self._runtime.board_sync(
            self.app_id,
            self._context,
            state=CPUState.FINISHED)
        time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Stop (4) Get IOBUF...")
        # Get data
        if get_buffers:
            print("[MPI_APP]: Getting %s buffers" % (self.app_aplx,))
            buffers = self._context.get_buffers()
            time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Stop (4) Send STOP...")
        #... -> Signal STOP
        self._runtime.board_sync(
            self.app_id,
            self._context,
            signal=SCPSignal.STOP)
        time.sleep(self.DELAY_500ms)

        print("[MPI_APP]: Stop (4) Check IDLE...")
        # Wait IDLE
        self._runtime.board_sync(
            self.app_id,
            self._context,
            state=CPUState.IDLE)

        print("[MPI_APP]: Stop (4) stopping runtime...")
        # Stop ACP runtime
        self._runtime.stop()

        print("[MPI_APP]: Stop (4) DONE")
        self._status = self.STATUS_STOPPED
        return buffers

    @property
    def runtime(self):
        return self._runtime

    @property
    def status(self):
        return self._status

    @property
    def context(self):
        return self._context

    @property
    def app_size(self):
        return self._app_size

    @property
    def app_executable(self):
        return self._app_executable

    @property
    def app_aplx(self):
        return str(self._app_aplx)

    @property
    def app_id(self):
        return self._app_id
