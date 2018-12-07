<?php

class EFetch extends EUtilsRequest {

  /**
   * EFetch constructor.
   *
   * @param string $db
   *
   * @throws \Exception
   */
  public function __construct($db) {
    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi');
    $this->addParam('db', $db);
  }
}
