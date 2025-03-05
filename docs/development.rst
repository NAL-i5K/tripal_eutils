Developer Guide
======================================


Adding support for a new NCBI database
--------------------------------------


- Implement an ``EUtilsParserInterface``
- Add the interface to ``EUtilsXMLParserFactory``
- Create a formatter extending ``EUtilsFormatter`` for displaying previews to the user.
- Create a repository extending ``EUtilsRepository`` for inserting into Chado.
- Add the formatter and repository to their respective factory classes.
- Add the database to the ``tripal_eutils_import.form.php`` database list.
- Modify ``tripal_eutils.install``, inserting any new databases necessary for the cross references.  Also be sure to insert any new cvterms your repository will need.


.. toctree::
  :maxdepth: 2
  :caption: Code Documentation:
  :glob:

  code_doc/*
