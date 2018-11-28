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
        $this->dict = $this->provideAssemblyDict();
        break;

      default:
        break;
    }
  }

  private function provideAssemblyDict() {
    
    [
      "alt_loci_countall",
      "chromosome_countall",
      "contig_countall",
      "contig_l50all",
      "contig_n50all",
      "non_chromosome_replicon_countall",
      "replicon_countall",
      "scaffold_countall",
      "scaffold_countplaced",
      "scaffold_countunlocalized",
      "scaffold_countunplaced",
      "scaffold_l50all",
      "scaffold_n50all",
      "total_lengthall",
      "ungapped_lengthall"
      ];

  }
}
