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

We don't currently know how we will map analyses to biomaterials in Chado.  Assemblies that are listed in BioSample records are therefore ignored currently.


Attributes
----------

This module does not currently map attributes to ontology terms.  Instead, all attributes are put into a "NCBI Property" controlled vocabulary. Suggested attribute - ontology term mappings for the `Plant 1.0 <https://www.ncbi.nlm.nih.gov/biosample/docs/packages/Plant.1.0/>`_ and `Invertebrate 1.0 <https://www.ncbi.nlm.nih.gov/biosample/docs/packages/Invertebrate.1.0/>`_ BioSample packages are available here: https://data.nal.usda.gov/dataset/data-tripal-eutils-tripal-module-increase-exchange-and-reuse-genome-assembly-metadata.  The full attribute set can be downloaded at `NCBI <https://www.ncbi.nlm.nih.gov/biosample/docs/attributes/?format=xml>`_.
