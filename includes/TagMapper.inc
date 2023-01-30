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
        break;

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
   * Biosample attributes.  derived from the "harmonized name" of <Attribute>s.
   *
   * @return array
   *   An array of XML tag -> cvterm object mappings.
   */
  private function provideBiosampleAttributeDict() {

    // Please keep this alphabetized for sanity.
    // If updated, please also delete/update terms in the install file.
    return [
      'age' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:age']),
      'bio_material' => NULL,
      'breed' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:breed']),
      'collection_date' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:collection_date']),
      'cultivar' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:cultivar']),
      'dev_stage' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:dev_stage']),
      'geo_loc_name' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:geo_loc_name']),
      'isolation_source' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:isolation_source']),
      'orgmod_note' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:orgmod_note']),
      'phenotype' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:phenotype']),
      'sex' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:sex']),
      'strain' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:strain']),
      'sub_species' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:sub_species']),
      'tissue' => $this->getTerm(['id' => 'NCBI_BioSample_Attributes:tissue']),
    ];
  }

  /**
   * @param array $term
   *
   * @return array|mixed
   */
  private function getTerm(array $term) {
    $term_id = $term['id'];
    if (isset(static::$cache[$term_id])) {
      return static::$cache[$term_id];
    }

    // Some of the terms listed in provideBiosampleAttributeDict()
    // may not exist in your database, in which case NULL is returned.
    // Should a warning be printed if there is no such term?
    $term = tripal_get_cvterm($term);
    if ($term) {
      static::$cache[$term->cvterm_id] = $term;
    }
    return $term;
  }

  /**
   *
   */
  private function provideAssemblyTagDict() {

  }

  /**
   * Looks up a BioSampleAttribute.
   *
   * @param array $record
   *   An associative array from the BiosampleXML Parser.  We hope for a value
   *   and a harmonized_name key but we aren't always so lucky.
   *
   * @return string|bool
   *   False if we couldn't get a label, otherwise, return the label machine
   *   name.
   */
  public function getLabel(array $record) {

    unset($record['value']);

    if (empty($record)) {
      return FALSE;
    }
    $label = $record['harmonized_name'] ?? NULL;

    if (!$label) {
      $label = $record['attribute_name'] ?? NULL;
    }
    if (!$label) {
      // I give up.  Use whatever you got.
      reset($record);
      $label = key($record);
    }
    return $label;
  }

  /**
   * Get the term for displaying to an end user via the Formatter.
   *
   * @param array $record
   *   Attribute array from XML parser.
   *
   * @return string|bool
   *   The String to display to user, or FALSE.
   */
  public function getDisplayLabel(array $record) {

    unset($record['value']);

    if (empty($record)) {
      return FALSE;
    }
    $label = $record['display_name'] ?? NULL;

    if (!$label) {
      $label = $record['attribute_name'] ?? NULL;
    }
    if (!$label) {
      // I give up.  Use whatever you got.
      reset($record);
      $label = key($record);
    }
    return $label;
  }

}
