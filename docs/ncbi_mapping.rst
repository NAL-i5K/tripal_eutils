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

	 Pay attention to the importer preview!
