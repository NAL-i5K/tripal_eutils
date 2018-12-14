BioProject
==========

- **NCBI BioProject**: https://www.ncbi.nlm.nih.gov/bioproject
- **Chado Project**: https://laceysanderson.github.io/chado-docs/tables/project.html

The Chado project table will provide many more linkers in Chado 1.4: until that discussion is resolved, this module will not take full advantage of NCBI BioProjects.

.. csv-table:: BioProject to Chado.bioproject mappings
   :file: ./bioproject_info.csv
   :header-rows: 1


Notes and details
-----------------

Multiple organisms
~~~~~~~~~~~~~~~~~~

We do not insert all organisms when importing a project accession.

Sometimes, a project will specify a different species and ``taxID`` in the Organism tag: ``<Organism species="57918" taxID="101020">``.
In these cases, the actual biomaterial is derived from the ``taxID``, so thats what this module imports.
