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
    $api_key = variable_get('tripal_eutils_ncbi_api_key');
    if ($api_key && $this->api_key_validate($api_key)) {
      $this->addParam('api_key', $api_key);
    }
  }

}
