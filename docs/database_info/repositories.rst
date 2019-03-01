Repositories
============

Repositories take the output from an XML parser and insert the record into Chado.

For linked records, repositories will spawn new EUtils objects and insert the linked records into Chado.


EUtilsRepository
----------------

.. doxygenclass:: EUtilsRepository
  :members:

EUtilsRepositoryFactory
----------------------

.. doxygenclass:: EUtilsRepositoryFactory
  :members:

EUtilsAssemblyRepository
-----------------------

.. doxygenclass:: EUtilsAssemblyRepository
  :members:

EUtilsBioProjectRepository
-------------------------
.. doxygenclass:: EUtilsBioProjectRepository
  :members:

EUtilsBioSampleRepository
------------------------

.. doxygenclass:: EUtilsBioSampleRepository
  :members:


EUtilsPubmedRepository
----------------------

Pubmed records are imported via the Tripal Core API.


.. doxygenclass:: EUtilsPubmedRepository
  :members:
