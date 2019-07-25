# -*- coding: utf-8 -*-
from __future__ import print_function
from spynnaker_acp import ACPRuntime


class MPIRuntime(ACPRuntime):
    def stop_app(self, app):
        return app.stop()

    def launch_app(self, app, context):
        return app.run(context)
