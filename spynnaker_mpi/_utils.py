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
import sys
import spinnman.transceiver
import spinnman.model.bmp_connection_data
import spinnman.exceptions


class Utils(object):
    si = {'T': 10**(+12),
          'G': 10**(+9),
          'M': 10**(+6),
          'k': 10**(+3),
          'h': 10**(+2),
          'da': 10**(+1),
          'd': 10**(-1),
          'c': 10**(-2),
          'm': 10**(-3),
          'u': 10**(-6),
          'n': 10**(-9),
          'p': 10**(-12)}

    @classmethod
    def spinn5_radius_processors(cls, r, p):
        _r = [1, 4, 9, 16, 24, 32, 40, 48]
        return _r[r] * p

    @classmethod
    def print_io_buffers(cls, io_buffer_dict):
        print("SpiNNaker MPI runtime - Buffers")
        chip_list = sorted(io_buffer_dict.keys())
        for x, y, p in chip_list:
            for line in io_buffer_dict[(x, y, p)]:
                print("%d:%d:%d > %s" % (x, y, p, line))

    @classmethod
    def get_io_buffers(cls, io_buffer_dict):
        s = ["SpiNNaker MPI runtime - Buffers"]
        chip_list = sorted(io_buffer_dict.keys())
        for x, y, p in chip_list:
            for line in io_buffer_dict[(x, y, p)]:
                s.append("%d:%d:%d > %s" % (x, y, p, line))
        return s

    @classmethod
    def runtime_check(cls, runtime):
        t = runtime.transceiver
        version_info = t.ensure_board_is_ready()
        print(version_info)

    @classmethod
    def get_transceiver(cls, spinn_url, bmp_url, version):
        """A very simple helper method to createa spinnaker tranceiver
        :param spinn_url:
        :param bmp_url:
        :param version:
        :return:
        :raise:
        """
        bmp = spinnman.model.bmp_connection_data.BMPConnectionData(0, 0, bmp_url, [0], None)
        try:
            transceiver = spinnman.transceiver.create_transceiver_from_hostname(
                spinn_url, version,
                bmp_connection_data=[bmp],
                auto_detect_bmp=False)
        except (spinnman.exceptions.SpinnmanIOException,
                spinnman.exceptions.SpinnmanInvalidPacketException,
                spinnman.exceptions.SpinnmanInvalidParameterException,
                spinnman.exceptions.SpinnmanUnexpectedResponseCodeException,
                spinnman.exceptions.SpinnmanGenericProcessException) as e:
            print("Error: Impossible to comunicate with the SpiNNaker Board %s" % (spinn_url,))
            sys.exit(e)
        return transceiver

    # @classmethod
    # def print_reinjection_status(cls, status):
    #     print("Dropped Packets captured:", status.n_dropped_packets)
    #     print("Missed drop packets:", status.n_missed_dropped_packets)
    #     print("Dropped Packet overflows:", status.n_dropped_packet_overflows)
    #     print("Reinjected packets:", status.n_reinjected_packets)
    #     print("Router timeout: {}  emergency timeout {}".format(
    #         status.router_timeout, status.router_emergency_timeout))
    #     print("Re-injecting multicast: {}  "
    #           "point_to_point: {}  "
    #           "nearest_neighbour: {}  "
    #           "fixed_route: {}".format(
    #         status.is_reinjecting_multicast,
    #         status.is_reinjecting_point_to_point,
    #         status.is_reinjecting_nearest_neighbour,
    #         status.is_reinjecting_fixed_route))
    #     return

    # @classmethod
    # def print_filter(cls, d_filter):
    #     cls.print_word_as_binary(
    #         "Filter word:", d_filter.filter_word,
    #         fields=[(31, 31), (30, 30), (29, 29), (28, 25), (24, 16), (15, 14),
    #                 (13, 12), (11, 10), (9, 9), (8, 8), (7, 4), (3, 0)])
    #     print("Enable Interrupt:", d_filter.enable_interrupt_on_counter_event)
    #     print("Emergency Routing Status on Incoming:",
    #           d_filter.match_emergency_routing_status_to_incoming_packet)
    #     cls.print_enums("Destinations:", d_filter.destinations)
    #     cls.print_enums("Sources:", d_filter.sources)
    #     cls.print_enums("Payloads:", d_filter.payload_statuses)
    #     cls.print_enums("Default Routing:", d_filter.default_routing_statuses)
    #     cls.print_enums("Emergency Routing:", d_filter.emergency_routing_statuses)
    #     cls.print_enums("Packet Types:", d_filter.packet_types)
    #     return
    #
    # @classmethod
    # def print_enums(cls, name, enum_list):
    #     string = ""
    #     for enum_value in enum_list:
    #         string += enum_value.name + "; "
    #     print(name, string)
    #     return
    #
    # @classmethod
    # def print_word_as_binary(cls, name, word, start=0, end=32, fields=None):
    #     start_fields = set()
    #     end_fields = set()
    #     if fields is not None:
    #         for field in fields:
    #             start_fields.add(field[0])
    #         for field in fields:
    #             if (field[1] - 1) not in start_fields:
    #                 end_fields.add(field[1])
    #     prefix = ""
    #     for i in range(len(name)):
    #         prefix += " "
    #     values = ""
    #     for i in reversed(range(start, end)):
    #         if i in start_fields:
    #             values += "|"
    #         if i % 10 == 0:
    #             values += str(i / 10)
    #         else:
    #             values += " "
    #         if i in end_fields:
    #             values += "|"
    #     print(prefix, values)
    #     values = ""
    #     for i in reversed(range(start, end)):
    #         if i in start_fields:
    #             values += "|"
    #         values += str(i % 10)
    #         if i in end_fields:
    #             values += "|"
    #     print(prefix, values)
    #     for i in reversed(range(start, end)):
    #         if i in start_fields:
    #             prefix += "|"
    #         prefix += "="
    #         if i in end_fields:
    #             prefix += "|"
    #     print("", prefix)
    #     string = ""
    #     for i in reversed(range(start, end)):
    #         if i in start_fields:
    #             string += "|"
    #         string += str((word >> i) & 0x1)
    #         if i in end_fields:
    #             string += "|"
    #     print(name, string)
    #     return
