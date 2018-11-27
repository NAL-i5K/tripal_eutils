<?php

class EFetch extends EUtilsRequest{

  protected $valid_dbs = [
    'pubmed',
    'bioproject',
    'biosample',
    'genome',
  ];

  /**
   * EFetch constructor.
   *
   * @throws \Exception
   */
  public function __construct($db) {
    parent::__construct($db);

    $this->setBaseURL('https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi');
  }
}
