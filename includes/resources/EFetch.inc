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

    $api_key = variable_get('tripal_eutils_ncbi_api_key');
    if ($api_key && $this->api_key_validate($api_key,$base_url)) {
      $this->addParam('api_key', $api_key);
    }
  }
}
