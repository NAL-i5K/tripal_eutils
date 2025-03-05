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
   * @var Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoCvtermBuddy
   */
  protected $cvterm_instance;

  /**
   * TagMapper constructor.
   *
   *   NCBI database for lookup.
   */
  public function __construct($db) {
    $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
    $this->cvterm_instance = $buddy_service->createInstance('chado_cvterm_buddy', []);
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
dpm("CP101 lookup was called!!!"); //@@@
    $dict = $this->dict;

    if (!isset($this->dict[$term_string])) {
      return FALSE;
    }

    return $this->dict[$term_string];
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
   *   An array of XML tag -> cvterm buddy object mappings.
   */
  private function provideBiosampleAttributeDict() {

    // Please keep this alphabetized for sanity.
    // If updated, please also delete/update terms in the install file.
    return [
      'age' => $this->getTerm('NCBI_BioSample_Attributes:age'),
      'bio_material' => NULL,
      'breed' => $this->getTerm('NCBI_BioSample_Attributes:breed'),
      'collection_date' => $this->getTerm('NCBI_BioSample_Attributes:collection_date'),
      'cultivar' => $this->getTerm('NCBI_BioSample_Attributes:cultivar'),
      'dev_stage' => $this->getTerm('NCBI_BioSample_Attributes:dev_stage'),
      'geo_loc_name' => $this->getTerm('NCBI_BioSample_Attributes:geo_loc_name'),
      'isolation_source' => $this->getTerm('NCBI_BioSample_Attributes:isolation_source'),
      'orgmod_note' => $this->getTerm('NCBI_BioSample_Attributes:orgmod_note'),
      'phenotype' => $this->getTerm('NCBI_BioSample_Attributes:phenotype'),
      'sex' => $this->getTerm('NCBI_BioSample_Attributes:sex'),
      'strain' => $this->getTerm('NCBI_BioSample_Attributes:strain'),
      'sub_species' => $this->getTerm('NCBI_BioSample_Attributes:sub_species'),
      'tissue' => $this->getTerm('NCBI_BioSample_Attributes:tissue'),
    ];
  }

  /**
   * @param string $term
   *
   * @return array|mixed
   */
  private function getTerm(string $term) {
    if (isset(static::$cache[$term])) {
      return static::$cache[$term];
    }

    // Some of the terms listed in provideBiosampleAttributeDict()
    // may not exist in your database, in which case NULL is returned.
    // Should a warning be printed if there is no such term?
    $parts = explode(':', $term);
    $terms = $this->cvterm_instance->getCvterm(['db.name' => $parts[0], 'dbxref.accession' => $parts[1]], []); 
    $term_record = $terms[0] ?? NULL;
    static::$cache[$term] = $term_record;
    return $term_record;
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
