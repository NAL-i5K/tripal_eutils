<?php

class EUtils{

  ///**
  // * Set the NCBI db.  Only the following DBs are allowed:
  // * 'pubmed' - use the core publication importer instead.
  // *
  // * @param $db
  // */
  //public function setDB($db) {
  //
  //  $valid_dbs = array_flip($this->valid_dbs);
  //
  //  if (!array_key_exists($db, $valid_dbs)) {
  //    tripal_set_message('Invalid db: !db', [!'db' => $db], TRIPAL_ERROR);
  //    return FALSE;
  //  }
  //
  //  $this->db = $db;
  //  //if db isn't supported by efetch, do something...
  //
  //  if ($db == 'assembly') {
  //    $this->url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?';
  //  }
  //  return TRUE;
  //}

  /**
   * Search the set DB for the provided accessions
   *
   * @param string $db the DB
   * @param array $accessions An array of accessions
   *
   * @return \SimpleXMLElement
   * @throws \Exception
   */
  public function lookupAccessions($db, array $accessions) {
    $provider = $this->getResourceProvider($db);
    $accessions = implode(',', $accessions);
    $provider->addParam('id', $accessions);

    if ($db !== 'assembly') {
      $provider->addParam('retmode', 'xml')->addParam('rettype', 'xml');
    }

    $response = $provider->get();
    if ($response->isSuccessful()) {
      return $response->xml();
    }
    else {
      throw new Exception('Unable to retrieve accessions');
    }
  }

  public function getAttributes($accession = NULL) {

    $dom = $this->dom;

    $x = simplexml_import_dom($dom);
    //TODO:  Handle parsing differently for different database types.

  }

  /**
   * @param $db
   *
   * @return string
   * @throws \Exception
   */
  public function checkStatus($db) {
    $provider = $this->getResourceProvider($db);
    $provider->addParams([
      'retmode' => 'xml',
      'term' => 'all[SB]',
      'retstart' => 0,
      'retmax' => 0,
      'usehistory' => 'y',
    ]);
    $response = $provider->get();
    $dom = $response->dom();

    return $dom->getElementsByTagName('Count')->item(0)->textContent;
  }

  /**
   * @param $db
   *
   * @return \EFetch|\ESearch|\ESummary
   * @throws \Exception
   */
  protected function getResourceProvider($db) {
    if ($db === 'assembly') {
      return new ESummary($db);
    }

    if ($db === 'pmc') {
      return new ESearch($db);
    }

    return new EFetch($db);
  }
}
