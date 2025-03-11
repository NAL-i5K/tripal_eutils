Pubmed
======

NCBI pubmed records are mapped into ``chado.pub``.

- **NCBI Pubmed**: https://www.ncbi.nlm.nih.gov/pubmed
- **Chado pub table**: https://laceysanderson.github.io/chado-docs/tables/pub.html

.. note::

  Developer's note: publications are imported using the Tripal core publication importer plugin.
  Any suggestions or modifications about publication importing should be made at the
  `Tripal core repo <https://github.com/tripal/tripal>`_.

.. csv-table:: Pubmed to Chado.pub mappings
    :file: ./pubmed.csv
    :header-rows: 1


.. csv-table:: pubmed XML keys to Chado.pubprop mappings
    :file: ./pubmed_properties.csv
    :header-rows: 1
