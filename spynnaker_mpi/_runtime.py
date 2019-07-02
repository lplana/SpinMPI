# -*- coding: utf-8 -*-
from __future__ import print_function
from ._utils import Utils
import time

from spinnman.connections.udp_packet_connections.udp_sdp_connection import UDPSDPConnection
from spinn_machine.tags.iptag import IPTag


class Runtime(object):
    DELAY_500ms = 500 * Utils.si['m']
    DELAY_100ms = 100 * Utils.si['m']
    DELAY_THR = 300 * Utils.si['u']

    def __init__(self, spinn_url, bmp_url, version):
        self._spinn_url = str(spinn_url)
        self._spinn_version = int(version)
        self._bmp_url = str(bmp_url)
        self._transceiver = Utils.get_transceiver(spinn_url, bmp_url, version)
        self._scamp_version = self._check()

        self._transceiver.set_ip_tag(IPTag(None, 1, ".", 50000))
        self._connection = UDPSDPConnection(chip_x=255, chip_y=255,
                                            remote_host=spinn_url, remote_port=17893,
                                            local_port=50000)
        return

    def __del__(self):
        self._connection._socket.close()

    def __str__(self):
        _str = ""
        _str += "SpiNNaker MPI Runtime\n"
        _str += "\tspinn: %s\n" % (self.spinn_url,)
        _str += "\tbmp: %s\n" % (self.bmp_url,)
        _str += "\tversion: %s\n" % (self.spinn_version,)
        _str += "\tKernel: %s\n" % (self._scamp_version.version_string,)
        return _str

    def _check(self):
        version_info = self.transceiver.ensure_board_is_ready(enable_reinjector=False)
        return version_info

    def send_sdp(self, message):
        self.transceiver.send_sdp_message(message, connection=self.scamp_connection)
        time.sleep(self.DELAY_THR)
        return

    def stop_app(self, app):
        return app.stop()

    def launch_app(self, app, context):
        return app.run(context)

    @property
    def scamp_connection(self):
        return self._connection

    @property
    def spinn_version(self):
        return int(self._spinn_version)

    @property
    def spinn_url(self):
        return str(self._spinn_url)

    @property
    def bmp_url(self):
        return str(self._bmp_url)

    @property
    def transceiver(self):
        return self._transceiver


