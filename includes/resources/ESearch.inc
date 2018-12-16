<?php

/**
 * Queries the EUtils esearch API.
 *
 * @ingroup resources
 */
class ESearch extends EUtilsRequest {

  /**
   * ESearch constructor.
   *
   * @param string $db
   *
   * @throws \Exception
   */
  public function __construct(string $db) {
    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi');
    $this->addParam('db', $db);
  }

}
