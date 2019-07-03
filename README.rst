SpinMPI: Message Passing Interface for SpiNNaker
------------------------------------------------

SpinMPI is the implementation of a standard MPI inferface for parallel programming of the SpiNNaker neuromorphic multicore architecture.
The library features a toroid interconnection and packet support for broadcast communication.

Installation
------------

1. Download SpinMPI to your project folder. 
2. In the SpinMPI folder, setup the Python libraries:

::

	% python setup.py

3. In the SpinMPI folder, build and install the C libraries:

::

	% make install

Usage instructions
------------------

To use MPI in your SpiNNaker application, include the header files in your C file:

.. codeblock:: C
	#include "mpi.h"

authorship and copyright
------------------------




