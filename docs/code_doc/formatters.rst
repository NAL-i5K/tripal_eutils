Formatters
============

Formatters take the output from an XML parser and return a Drupal-form friendly array.
Typically a formatter will return a table each for:

- The base Chado record
- Properties
- DBXrefs
- New Chado records created and linked, including organisms, contacts, projects, analyses, etc

EUtilsFormatter
----------------

.. doxygenclass:: EUtilsFormatter
  :members:

EUtilsFormatterFactory
----------------------

.. doxygenclass:: EUtilsFormatterFactory
  :members:

EUtilsAssemblyFormatter
-----------------------

.. doxygenclass:: EUtilsAssemblyFormatter
  :members:

EUtilsBioProjectFormatter
-------------------------
.. doxygenclass:: EUtilsBioProjectFormatter
  :members:

EUtilsBioSampleFormatter
------------------------

.. doxygenclass:: EUtilsBioSampleFormatter
  :members:


EUtilsPubmedFormatter
----------------------

Pubmed records are not directly imported via this module, as this functionality is already provided via Tripal core.

