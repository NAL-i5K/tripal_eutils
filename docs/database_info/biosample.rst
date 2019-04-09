BioSample
=========

NCBI BioSamples are mapped into the Chado.biomaterial table.

- **NCBI database**: https://www.ncbi.nlm.nih.gov/biosample/
- **Chado biomaterial table**:  https://laceysanderson.github.io/chado-docs/tables/biomaterial.html
- **Chado MAGE module**:  http://gmod.org/wiki/Chado_Mage_Module


.. csv-table:: BioSample to Chado.biomaterial mappings
   :file: ./biosample_info.csv
   :header-rows: 1

.. note::

	In the above table, XML tags are described as Parent_tag->Child_tag.  If the value comes from the attribute of a tag, it is written lowercase, as Parent_tag->attribute.


Undecided mappings
------------------

We don't currently know how we will mapping analyses to biomaterials in Chado.  Assemblies that are listed in BioSample records are therefore ignored currently.


Attributes
----------

This module does not currently map attributes to ontology terms.  Instead, all attributes are put into a "NCBI Property" controlled vocabulary.  The full attribute set can be downloaded at `NCBI <https://www.ncbi.nlm.nih.gov/biosample/docs/attributes/?format=xml>`_
