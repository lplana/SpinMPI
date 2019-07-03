# ==========================================================================
#                                  SpinMPI
# ==========================================================================
# This file is part of SpinMPI.
#
# SpinMPI is Free Software: you can redistribute it and/or modify it
# under the terms found in the LICENSE[.md|.rst] file distributed
# together with this file.
#
# SpinMPI is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# ==========================================================================
# Autor: Francesco Barchi <francesco.barchi@polito.it>
# ==========================================================================
# SpinMPI.cpp: Main File for SpinMPI
# ==========================================================================

# -*- coding: utf-8 -*-
from __future__ import print_function
import os
import time

import spinnman.model.cpu_info
import spinnman.model.cpu_state
import spinnman.messages.scp.scp_signal
import spinn_storage_handlers.file_data_reader

import spynnaker_acp

from ._utils import Utils


class App(object):
    MAX_ATTEMPTS = 10

    DELAY_100ms = 100 * Utils.si['m']
    DELAY_500ms = 500 * Utils.si['m']

    STATUS_NONE = 0
    STATUS_ASSIGNED = 1
    STATUS_LAUNCHED = 2
    STATUS_STOPPED = 3

    def __init__(self, runtime, app_aplx, app_id=30):
        self._runtime = runtime
        self._app_aplx = str(app_aplx)
        self._app_id = str(app_id)

        self._app_size = os.stat(self._app_aplx).st_size
        self._app_executable = \
            spinn_storage_handlers.file_data_reader.FileDataReader(self._app_aplx)

        self._context = None
        self._acp = None
        self._status = App.STATUS_NONE
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
        print("SPINN_MPI_APP: Loading %s instance" % (self.app_aplx,))
        self._runtime.transceiver.execute_flood(
            self.context.cores_subsets,
            self.app_executable,
            self.app_id,
            self.app_size)
        time.sleep(self.DELAY_500ms)

        self._sync(state=spinnman.model.cpu_state.CPUState.SYNC0,
                   signal=spinnman.messages.scp.scp_signal.SCPSignal.SYNC0)

        # Phase 2a: Enable ACP Runtime
        self._acp = spynnaker_acp.ACPRuntime(self._runtime.transceiver,
                                             connection=self._runtime.scamp_connection)
        self._acp.add_reaction(0xF4, 0, lambda: print("READ-REPLY received on Var 0"))
        self._acp.add_reaction(0xFF, 0, self._acp.signal)
        self._acp.start()

        # Phase 2b: Send Context
        self._context.send_rank()

        print("SPINN_MPI_APP: Initialized")
        self._status = self.STATUS_ASSIGNED
        return

    def run(self):
        if self.status != self.STATUS_ASSIGNED:
            return

        # Phase 3: Application run
        self._sync(state=spinnman.model.cpu_state.CPUState.SYNC1,
                   signal=spinnman.messages.scp.scp_signal.SCPSignal.SYNC1)

        print("SPINN_MPI_APP: Launched")
        self._status = self.STATUS_LAUNCHED
        return

    def stop(self, get_buffers=False):
        buffers = list()

        if self.status != self.STATUS_LAUNCHED:
            print("SPINN_MPI_APP: Impossible to stop!")
            return buffers

        self._acp.wait()

        # Wait SYNC0 -> ... -> Signal SYNC0
        self._sync(state=spinnman.model.cpu_state.CPUState.SYNC0)
        # Get data and others stuff
        pass
        time.sleep(self.DELAY_100ms)
        self._sync(signal=spinnman.messages.scp.scp_signal.SCPSignal.SYNC0)
        time.sleep(self.DELAY_500ms)

        # Wait FINISHED -> ... -> Signal STOP
        self._sync(state=spinnman.model.cpu_state.CPUState.FINISHED)
        # Get data and others stuff
        if get_buffers:
            print("SPINN_MPI_APP: Getting %s buffers" % (self.app_aplx,))
            buffers = self._context.get_buffers()
        time.sleep(self.DELAY_100ms)
        self._sync(signal=spinnman.messages.scp.scp_signal.SCPSignal.STOP)
        time.sleep(self.DELAY_500ms)

        # Wait IDLE
        self._sync(state=spinnman.model.cpu_state.CPUState.IDLE)

        # Stop ACP runtime
        self._acp.stop()

        self._status = self.STATUS_STOPPED
        return buffers

    def _sync(self, state=None, signal=None):
        # State wait
        if state is not None:
            if self._check_state(state):
                print("SPINN_MPI_APP: All instance are in correct state!")
            else:
                print("SPINN_MPI_APP: [!] Not all instance are in correct state!")

        # Send signal
        if signal is not None:
            self._runtime.transceiver.send_signal(self.app_id, signal)

    def _check_state(self, state):
        attempts = 0
        count = self._context.get_state_count(state)

        while count != self.context.cores_number and attempts < self.MAX_ATTEMPTS:
            attempts += 1
            if attempts > self.MAX_ATTEMPTS/2:
                print("\t...Attempt %d" % (attempts,))
            count = self._context.get_state_count(state)
            time.sleep(self.DELAY_500ms)

        if count == self.context.cores_number:
            return True
        else:
            return False

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
        return int(self._app_id)
