Assembly
========

- https://www.ncbi.nlm.nih.gov/assembly/
- https://laceysanderson.github.io/chado-docs/tables/analysis.html

.. csv-table:: Assembly to Chado.analysis mappings
   :file: ./assembly_info.csv
   :header-rows: 1

Note that the program and program version are not found directly in the XML.  Instead they are extracted from the FTP attribute.

Analysis type
-------------
The analysis table has no `type_id` column.  The type is therefore set with the `rdfs:type` property.

The `RefSeq_category` tag is used to determine the analysis type.  Currently, only the value `representative genome` is supported and mapped to the bundle Genome Assembly (operation:0525), via the type value 'genome_assembly'.  We have thus far come across no other values for this key in the database.

Is an assembly a Chado analysis or project?
-------------------------------------------

This is still an **open question**.  This module maps NCBI Assemblies into ``chado.analysis``, but it may split the NCBI assembly record into an analysis and project in the future.  This is because the current definition of a Chado analysis is a **single program run**.  Assemblies are typically many programs run in a pipeline.

Undecided mappings
-------------------

We don't currently know how we will map analyses to biomaterials in Chado.  BioSamples that are listed in Assembly records are therefore ignored currently.
