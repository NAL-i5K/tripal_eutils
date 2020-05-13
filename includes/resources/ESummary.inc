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
    $api_key = variable_get('tripal_eutils_ncbi_api_key');
    if ($api_key && $this->api_key_validate($api_key,$base_url)) {
      $this->addParam('api_key', $api_key);
    }
  }

  }
