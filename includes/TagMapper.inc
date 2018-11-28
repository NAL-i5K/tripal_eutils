<?php

/**
 * Class TagMapper
 * Responsible for providing instructions for a given tag.
 */

class TagMapper{

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
    return [
      'alt_loci_count' => NULL,
      'chromosome_count' => NULL,
      'contig_count' => NULL,
      'contig_l50' => NULL,
      'contig_n50' => NULL,
      'non_chromsome_replicon_count' => NULL,
    ];
  }
}
