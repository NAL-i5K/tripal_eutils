<?php

class ESummary extends EUtilsRequest{

  /**
   * ESummary constructor.
   *
   * @throws \Exception
   */
  public function __construct($db) {
    parent::__construct($db);

    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi');
  }
}
