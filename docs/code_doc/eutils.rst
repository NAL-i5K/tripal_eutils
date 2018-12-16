Connecting to NCBI: Eutils
===========================

The Eutils **resource** classes (located in ``includes/resources``) are for connecting to the NCBI repository.
Not all databses are supported by the ESearch API: we therefore use the Eutils class to provide the correct service.

.. doxygenclass:: EUtils
  :members:
  :protected-members:

.. doxygenclass:: EUtilsRequest
  :members:
  :protected-members:

.. doxygenclass:: EUtilsResource
  :members:
  :protected-members:

.. doxygenclass:: ESummary
  :members:
  :protected-members:

.. doxygenclass:: ESearch
  :members:
  :protected-members:

.. doxygenclass:: EFTP
  :members:
  :protected-members:
