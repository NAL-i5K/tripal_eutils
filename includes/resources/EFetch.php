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
    // NCBI API private key, or NULL if it has not been set
    $api_key = \Drupal::config('tripal_eutils.settings')->get('tripal_eutils.ncbi_api_key');

    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi');
    $this->addParam('db', $db);
    if ($api_key) {
      $this->addParam('api_key', $api_key);
    }
  }
}
