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
    $this->set_dict();
  }

  private function set_dict() {

    $db = $this->db;


    switch ($db) {

      case "assembly":

        $this->dict = $this->provide_assembly_dict();
        break;

      case 'default':
        break;
    }
  }

  private function provide_assembly_dict() {

    return [

      'alt_loci_count' => null,
      'chromosome_count' => null,
      'contig_count' => null,
      'contig_l50' => null,
      'contig_n50' => null,
      'non_chromsome_replicon_count' => null,
    ];
  }


}