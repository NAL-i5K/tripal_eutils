<?php

/**
 * A helper for using EUtils in a test environment.
 *
 * It ensures that external calls are never made.
 */
class EUTilsLocalHelper {

  private $xmls = [];

  /**
   *
   */
  public function add_xml($xml, $db, $accession) {

    $this->xmls[$db][$accession] = $xml;

  }

  /**
   *
   */
  public function import() {

    // Mirror the eutils class.
    $repo = new \EUtils();

  }

}
