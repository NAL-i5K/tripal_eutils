<?php

/**
 * Queries the NCBI ESummary API.
 *
 * @ingroup resources
 */
  class ESummary extends EUtilsRequest {

  /**
   * ESummary constructor.
   *
   * @param string $db
   *
   * @throws \Exception
   */
  public function __construct($db) {
    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi');
    $this->addParam('db', $db);
  }

}
