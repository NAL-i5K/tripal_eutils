Mapping NCBI content into Chado
===============================

Unfortunately it isn't always clear how NCBI data should map into Chado.

This section describes what to expect when running the EUtils importer.


NCBI to Chado
-------------

.. csv-table:: NCBI to Chado mappings
   :file: ./mapping_table.csv
   :widths: 30, 70
   :header-rows: 1

Database specific mappings
---------------------------


.. toctree::
  :glob:
  :maxdepth: 2
  :caption: NCBI database descriptions:

  database_info/*



Linked content
--------------


The EUtils admin form has a checkbox to insert linked content.  This will only insert content that is **directly linked** to the accession you are importing.

Consider a BioProject with many BioSamples and analyses listed.  If you import that BioProject and choose to include linked records, all the directly associated BioSamples and Assemblies will also be imported.

If, however, you only wanted a subset of BioSamples in the database, you could import them individually: each BioSample would link to the BioProject, but the undesired BioSamples would not be imported into Chado.  If all the BioSamples of interest were listed in an Assembly project, you could import that Assembly.

.. note::

	 Pay attention to the importer preview!  The preview lets you double check the correct record will be inserted into the database.  It also demonstrates which additional records will be inserted if the "Insert Linked Records" box is checked.


Problematic Links
~~~~~~~~~~~~~~~~~

Linking some content is problematic for the current release of Chado.  In cases where a link cannot be made, the this module should **not** insert the content.  Instead, the formatter should notify the user that those accessions should be added directly.

This is the case for the following links:

* Assembly and Biomaterial

In other cases, records may be linked **indirectly** via Chado.  For example, project and organism cannot be directly linked, but, they are indirectly linked via biomaterials.  In those cases, the linked records **will** be inserted.  The user does not need to be notified at the preview step.  The exception should be documented on that database's page in the documentation.

This is the case for the following links:

* BioProject and NCBI Taxon
