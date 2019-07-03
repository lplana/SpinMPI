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
from distutils.core import setup

setup(
    name='spynnaker_mpi',
    version='0.1.0',
    author='Francesco Barchi',
    author_email='francesco.barchi@polito.it',
    packages=[
        'spynnaker_mpi',
        'spynnaker_mpi.types',
    ],
)
