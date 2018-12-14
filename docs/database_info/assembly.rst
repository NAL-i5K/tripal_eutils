Assembly
========

- https://www.ncbi.nlm.nih.gov/assembly/
- https://laceysanderson.github.io/chado-docs/tables/analysis.html



.. csv-table:: Assembly to Chado.analysis mappings
   :file: ./assembly_info.csv
   :header-rows: 1


Is an assembly a Chado analysis or project?
-------------------------------------------

This is still an **open question**.  This module maps NCBI Assemblies into ``chado.analysis``, but it may split the NCBI assembly record into an analysis and project in the future.  This is because the current definition of a Chado analysis is a **single program run**.  Assemblies are typically many programs run in a pipeline.
