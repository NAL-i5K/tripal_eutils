<?php

/**
 * Https://www.ncbi.nlm.nih.gov/books/NBK25499/ NCBI Efetch docs.
 *
 * @ingroup resources
 */
class EFetch extends EUtilsRequest {

  /**
   * EFetch constructor.
   *
   * @param string $db
   *   NCBI database string.
   *
   * @throws \Exception
   */
  public function __construct(string $db) {
    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi');
    $this->addParam('db', $db);
  }

}
