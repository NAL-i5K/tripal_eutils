<?php

/**
 * Class TagMapper.
 *
 * And attribute.
 *
 * @todo should we rename this to just mapper?  attribute mapper, and have the
 *   tag mapper separate?
 */
class TagMapper {

  /**
   * Cache array of terms to facilitate lookup.
   *
   * @var array
   */
  private static $cache = [
    'terms' => [],
  ];

  /**
   * NCBI database for lookup.
   *
   * @var null
   */
  private $db = NULL;

  /**
   * Dictionary.  The lookup for the configured database.
   *
   * @var null
   */
  private $dict = NULL;

  /**
   * TagMapper constructor.
   *
   * @param string $db
   *   NCBI database for lookup.
   */
  public function __construct($db) {
    $this->db = $db;
    $this->setDict();
  }

  /**
   * Set the dictionary.
   */
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
   * Lookup function, searches for a term based on a string.
   *
   * @param string $term_string
   *   The string to lookup.
   *
   * @return bool
   *   Not sure what it should return, the cvterm_id?
   */
  public function lookup($term_string) {
    $dict = $this->dict;

    if (!isset($dict[$term_string])) {
      return FALSE;
    }

    return $dict[$term_string];
  }

  /**
   * @return array
   */
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
      'age' => $this->getTerm(['id' => 'ncbi_properties:age']),
      'bio_material' => NULL,
      'breed' => $this->getTerm(['id' => 'ncbi_properties:breed']),
      'collection_date' => $this->getTerm(['id' => 'ncbi_properties:collection_date']),
      'cultivar' => $this->getTerm(['id' => 'ncbi_properties:cultivar']),
      'dev_stage' => $this->getTerm(['id' => 'ncbi_properties:dev_stage']),
      'geo_loc_name' => $this->getTerm(['id' => 'ncbi_properties:geo_loc_name']),
      'isolation_source' => $this->getTerm(['id' => 'ncbi_properties:isolation_source']),
      'orgmod_note' => $this->getTerm(['id' => 'ncbi_properties:orgmod_note']),
      'phenotype' => $this->getTerm(['id' => 'ncbi_properties:phenotype']),
      'sex' => $this->getTerm(['id' => 'ncbi_properties:sex']),
      'strain' => $this->getTerm(['id' => 'ncbi_properties:strain']),
      'sub_species' => $this->getTerm(['id' => 'ncbi_properties:sub_species']),
      'tissue' => $this->getTerm(['id' => 'ncbi_properties:tissue']),
    ];
  }

  /**
   * @param array $term
   *
   * @return array|mixed
   */
  private function getTerm(array $term) {
    if (isset(static::$cache[$term['id']])) {
      return static::$cache[$term['id']];
    }

    $term = tripal_get_cvterm($term);
    static::$cache[$term->cvterm_id] = $term;
    return $term;
  }

  private function provideAssemblyTagDict() {

  }
}
