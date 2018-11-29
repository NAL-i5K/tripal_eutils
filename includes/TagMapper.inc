<?php

/**
 * Class TagMapper
 * Responsible for providing instructions for a given tag.
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
   * @return array
   */
  private function provideBiosampleAttributeDict() {

    return [
      'age',
      'bio_material',
      'breed',
      'collection_date',
      'cultivar',
      'dev_stage',
      'geo_loc_name',
      'isolation_source',
      'orgmod_note',
      'phenotype',
      'sex',
      'strain',
      'sub_species',
      'tissue',
    ];

  }

  private function provideAssemblyTagDict() {

  }


}
