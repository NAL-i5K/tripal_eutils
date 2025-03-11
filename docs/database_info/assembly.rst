Assembly
========

- https://www.ncbi.nlm.nih.gov/assembly/
- https://laceysanderson.github.io/chado-docs/tables/analysis.html

.. csv-table:: Assembly to Chado.analysis mappings
   :file: ./assembly_info.csv
   :header-rows: 1

Note that the program and program version are not found directly in the XML.
Instead they are extracted from the FTP attribute.

Analysis type
-------------

The `RefSeq_category` tag is used to determine the analysis type.
Currently, only the value `representative genome` is supported and
mapped to the "Genome Assembly" bundle via the controlled vocabulary term
'genome assembly' (operation:0525), which is stored in the `analysisprop` table.
We have thus far come across no other values for this key in the NCBI database.
If one should later be used, the analysis will be mapped to a
generic Analysis content type.

Is an assembly a Chado analysis or project?
-------------------------------------------

This is still an **open question**. This module maps NCBI Assemblies
into ``chado.analysis``, but it may split the NCBI assembly record into
an analysis and project in the future. This is because the current
definition of a Chado analysis is a **single program run**.
Assemblies are typically many programs run in a pipeline.

Undecided mappings
-------------------

We don't currently know how we will map analyses to biomaterials in Chado.
BioSamples that are listed in Assembly records are therefore ignored currently.
