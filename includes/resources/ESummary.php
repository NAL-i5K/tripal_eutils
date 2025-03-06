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
    $api_key = \Drupal::config('tripal_eutils.settings')->get('tripal_eutils.ncbi_api_key');
    if ($api_key) {
      $this->addParam('api_key', $api_key);
    }
  }

  }
