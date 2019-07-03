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

To use SpinMPI in your SpiNNaker application, include the header files in your C file:

::

	#include "PATH_TO/mpi.h"

MPI is an open standard. Information on how to use the MPI primitives is widely available, for example on sites such as `MPI Tutorial <https://mpitutorial.com/>`__.

In the host-side Python code, first create the MPI runtime and context with the number of rings of cores to be used and declare the application binary:

::

	runtime = spynnaker_mpi.MPIRuntime.create("BOARD_IP_ADDRESS", "BOARD_BMP_IP_ADDRESS")
	context = spynnaker_mpi.MPIContext(runtime, NRINGS, NCORES)
	app = spynnaker_mpi.MPIApp(runtime, "APP_NAME.aplx")


Initialize and run the app:

::

	app.init(context)
	app.run()



authorship and copyright
------------------------




