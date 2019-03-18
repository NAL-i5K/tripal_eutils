SRA
=====

NCBI SRA records are mapped into ``chado.analysis``.
- **NCBI SRA**: https://www.ncbi.nlm.nih.gov/sra
- **Chado Analysis table**: https://laceysanderson.github.io/chado-docs/tables/analysis.html

.. csv-table:: SRA to Chado.analysis mappings
	   :file: ./sra.csv
	   :header-rows: 1


Cross references come from ``EXTERNAL_ID``: BioProject,


SRA type
~~~~~~~~~

SRA has multiple record subtypes.

There are multiple types of SRA records in NCBI: we're talking about records like https://www.ncbi.nlm.nih.gov/sra/DRX049157 vs the SRA study: https://trace.ncbi.nlm.nih.gov/Traces/sra/?study=DRP003980

searching for the study DRP003980  should yield the record DRX049157.  DRS057276 is the SAMPLE,  which points to SAMD00046318.  consists of multiple runs, DRR054308 and DRR054307.

The SRA record DRX049157 is an Experiment package.  It consists of the Experiment itself, the submission, the Study, Sample, Pool, and Run_set.

DRA004360 - submission.  we dont model this.

``   <SAMPLE alias="SAMD00046318" accession="DRS057276">`` we could add this as an additional dbxref for the biosample record.


POOL has lots of linked metadata if needed including tax_id.  note that we could have multiple members, for pooled samples.  In those cases the correct Chado way is to build multiple biomaterials via biomaterial_relationship.

.. code-block:: xml

  <Pool>
    <Member member_name="" accession="DRS057276" sample_name="SAMD00046318" sample_title="Irwin" spots="1513701" bases="1650906230" tax_id="29780" organism="Mangifera indica">
      <IDENTIFIERS>
        <PRIMARY_ID>DRS057276</PRIMARY_ID>
        <EXTERNAL_ID namespace="BioSample">SAMD00046318</EXTERNAL_ID>
      </IDENTIFIERS>
    </Member>
  </Pool>

RUN_SET
~~~~~~~

.. code-block:: xml

  <RUN_SET>
    <RUN alias="DRR054308" center_name="NIFTS" accession="DRR054308" total_spots="724319" total_bases="790234730" size="1854856875" load_done="true" published="2018-01-10 04:26:33" is_public="true" cluster_name="public" static_data_available="1">
      <IDENTIFIERS>
        <PRIMARY_ID>DRR054308</PRIMARY_ID>
      </IDENTIFIERS>
      <TITLE>454 GS FLX+ sequencing of SAMD00046318</TITLE><EXPERIMENT_REF refname="DRX049157" refcenter="NIFTS" accession="DRX049157"/>
      <Pool>
        <Member member_name="" accession="DRS057276" sample_name="SAMD00046318" sample_title="Irwin" spots="724319" bases="790234730" tax_id="29780" organism="Mangifera indica">
          <IDENTIFIERS>
            <PRIMARY_ID>DRS057276</PRIMARY_ID>
            <EXTERNAL_ID namespace="BioSample">SAMD00046318</EXTERNAL_ID>
          </IDENTIFIERS>
        </Member>
      </Pool>
      <Statistics nreads="1" nspots="724319"><Read index="0" count="724319" average="1091.00" stdev="197.60"/></Statistics>
      <Bases cs_native="false" count="790234730"><Base value="A" count="252681330"/><Base value="C" count="133152082"/><Base value="G" count="139436462"/><Base value="T" count="253440531"/><Base value="N" count="11524325"/></Bases>
    </RUN>
    <RUN alias="DRR054307" center_name="NIFTS" accession="DRR054307" total_spots="789382" total_bases="860671500" size="2009178726" load_done="true" published="2018-01-10 04:26:33" is_public="true" cluster_name="public" static_data_available="1">
      <IDENTIFIERS>
        <PRIMARY_ID>DRR054307</PRIMARY_ID>
      </IDENTIFIERS>
      <TITLE>454 GS FLX+ sequencing of SAMD00046318</TITLE><EXPERIMENT_REF refname="DRX049157" refcenter="NIFTS" accession="DRX049157"/>
      <Pool>
        <Member member_name="" accession="DRS057276" sample_name="SAMD00046318" sample_title="Irwin" spots="789382" bases="860671500" tax_id="29780" organism="Mangifera indica">
          <IDENTIFIERS>
            <PRIMARY_ID>DRS057276</PRIMARY_ID>
            <EXTERNAL_ID namespace="BioSample">SAMD00046318</EXTERNAL_ID>
          </IDENTIFIERS>
        </Member>
      </Pool>
      <Statistics nreads="1" nspots="789382"><Read index="0" count="789382" average="1090.31" stdev="191.37"/></Statistics>
      <Bases cs_native="false" count="860671500"><Base value="A" count="277391866"/><Base value="C" count="144641177"/><Base value="G" count="150659961"/><Base value="T" count="276069965"/><Base value="N" count="11908531"/></Bases>
    </RUN>
  </RUN_SET>
