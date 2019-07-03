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

Stop the application and retrieve the output print buffers:

::

	buffers = app.stop(get_buffers=True)


Authorship and copyright
------------------------

SpinMPI is being developed by `Francesco Barchi <mailto:francesco.barchi@polito.it>`__.

+------------------------------------------------------------------------------------------------------------------+--------------------------------------------------------------------------------------------------------------------+
|  **Please always cite the publication, also if using SpinMPI in comparisons and pipelines**                                                                                                                                           |
+------------------------------------------------------------------------------------------------------------------+--------------------------------------------------------------------------------------------------------------------+
| .. image:: https://user-images.githubusercontent.com/7613428/60581998-40d00b00-9d88-11e9-9a24-efd28e1bcaca.png   | *An Efficient MPI Implementation for Multi-Core Neuromorphic Platforms*;                                           |
|    :alt: Please cite                                                                                             | Francesco Barchi, Gianvito Urgese, Enrico Macii, Andrea Acquaviva;                                                 |
|    :target: https://ieeexplore.ieee.org/document/8676216                                                         | `2017 New Generation of CAS (NGCAS) <https://ieeexplore.ieee.org/document/8052322>`__;                             |
|    :width: 76px                                                                                                  | doi: 10.1109/NGCAS.2017.31                                                                                         |
+------------------------------------------------------------------------------------------------------------------+--------------------------------------------------------------------------------------------------------------------+
| **Please respect the license of the software**                                                                                                                                                                                        |
+------------------------------------------------------------------------------------------------------------------+--------------------------------------------------------------------------------------------------------------------+
| .. image:: https://user-images.githubusercontent.com/7613428/60581999-4168a180-9d88-11e9-87e3-ce5e127b84a1.png   | SpinMPI is free and open source software, so you can use it for any purpose, free of charge.                       |
|    :alt: Respect the license                                                                                     | However, certain conditions apply when you (re-)distribute and/or modify SpinMPI; please respect the               |
|    :target: https://github.com/neuromorphic-polito/SpinMPI/blob/master/LICENSE.rst                             | `license <https://github.com/neuromorphic-polito/SpinMPI/blob/master/LICENSE.rst>`__.                            |
|    :width: 76px                                                                                                  |                                                                                                                    |
+------------------------------------------------------------------------------------------------------------------+--------------------------------------------------------------------------------------------------------------------+

*icons on this page by Austin Andrews / https://github.com/Templarian/WindowsIcons*
