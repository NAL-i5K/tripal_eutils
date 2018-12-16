Formatters
============

Formatters take the output from an XML parser and return a Drupal-form friendly array.
Typically a formatter will return a table each for:

- The base Chado record
- Properties
- DBXrefs
- New Chado records created and linked, including organisms, contacts, projects, analyses, etc

.. doxygengroup:: formatters
  :members:
