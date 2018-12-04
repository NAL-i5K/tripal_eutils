<?php

/**
 * Class TagMapper
 * Responsible for providing instructions for a given tag.
 *
 * And attribute.
 *
 * @todo should we rename this to just mapper?  attribute mapper, and have the tag mapper seperate?
 */

class TagMapper {

  private $db = NULL;

  private $dict = NULL;

  public function __construct($db) {
    $this->db = $db;
    $this->setDict();
  }

  private function setDict() {
    $db = $this->db;

    switch ($db) {
      case "assembly":
        $this->dict = $this->provideAssemblyAttributeDict();
        break;

      case 'biosample':
        $this->dict = $this->provideBiosampleAttributeDict();

      default:
        break;
    }
  }

  /**
   * @param $term_string
   *
   * @return bool
   */
  public function lookup($term_string) {
    $dict = $this->dict;

    if (!isset($dict[$term_string])) {
      return FALSE;
    }

    return $dict[$term_string];

  }

  private function provideAssemblyAttributeDict() {

    return [
      "alt_loci_count_all",
      "chromosome_count_all",
      "contig_count_all",
      "contig_l50_all",
      "contig_n50_all",
      "non_chromosome_replicon_count_all",
      "replicon_count_all",
      "scaffold_count_all",
      "scaffold_count_placed",
      "scaffold_count_unlocalized",
      "scaffold_count_unplaced",
      "scaffold_l50_all",
      "scaffold_n50_all",
      "total_length_all",
      "ungapped_length_all",
    ];

  }

  /**
   * Biosample attributes.  derived from the "harmonized name" of <Attribute>s
   *
   * @return array
   */
  private function provideBiosampleAttributeDict() {

    //please keep this alphabetized for sanity.
    //If updated, please also delete/update terms in the install file.
    return [
      'age' => tripal_get_cvterm(['id' => 'ncbi_properties:age']),
      'bio_material' => NULL,
      'breed' => tripal_get_cvterm(['id' => 'ncbi_properties:breed']),
      'collection_date' => tripal_get_cvterm(['id' => 'ncbi_properties:collection_date']),
      'cultivar' => tripal_get_cvterm(['id' => 'ncbi_properties:cultivar']),
      'dev_stage' => tripal_get_cvterm(['id' => 'ncbi_properties:dev_stage']),
      'geo_loc_name' => tripal_get_cvterm(['id' => 'ncbi_properties:geo_loc_name']),
      'isolation_source' => tripal_get_cvterm(['id' => 'ncbi_properties:isolation_source']),
      'orgmod_note' => tripal_get_cvterm(['id' => 'ncbi_properties:orgmod_note']),
      'phenotype' => tripal_get_cvterm(['id' => 'ncbi_properties:phenotype']),
      'sex' => tripal_get_cvterm(['id' => 'ncbi_properties:sex']),
      'strain' => tripal_get_cvterm(['id' => 'ncbi_properties:strain']),
      'sub_species' => tripal_get_cvterm(['id' => 'ncbi_properties:sub_species']),
      'tissue' => tripal_get_cvterm(['id' => 'ncbi_properties:tissue']),
    ];

  }

  private function provideAssemblyTagDict() {

  }


}
