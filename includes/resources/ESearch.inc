<?php

class ESearch extends EUtilsRequest{
  /**
   * ESearch constructor.
   *
   * @param string $db
   *
   * @throws \Exception
   */
  public function __construct($db) {
    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi');
    $this->addParam('db', $db);
  }
}
