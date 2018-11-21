<?php


class Euitils {

  /**
   * Euitils interface.
   *
   */

  /**
   * The euitils url
   *
   * @var string
   */
  //private $url = 'http://eutils.be-md.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?';

  private $url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?';

  private $db = NULL;

  private $valid_dbs = ['pubmed', 'nuccore'];


  /**
   * Set the NCBI db.  Only the following DBs are allowed:
   * 'pubmed' - use the core publication importer instead.
   *
   * @param $db
   */
  public function set_db($db) {

    $valid_dbs = array_flip($this->valid_dbs);

    if (!array_key_exists( $db, $valid_dbs)) {
      tripal_set_message('Invalid db: !db', [!'db' => $db], TRIPAL_ERROR);
      return FALSE;
    }

    $this->db = $db;
    return TRUE;
  }

  /**
   * Search the set DB for the provided accessions.
   *
   * @param $accessions - array of accession strings.
   *
   * @return array
   */
  public function lookup_accessions($accessions) {

    $values = [];


    return $values;

  }

  public function check_status() {
    $params = [
      'db' => 'pmc',
      'retmode' => 'xml',
      'term' => 'all[SB]',
      'retstart' => 0,
      'retmax' => 0,
      'usehistory' => 'y',
    ];
    $url = $this->url . http_build_query($params);

    $dom = new DOMDocument;
    $dom->load($url);
    $result = [
      'count' => $dom->getElementsByTagName('Count')->item(0)->textContent,
      'webenv' => $dom->getElementsByTagName('WebEnv')->item(0)->textContent,
      'querykey' => $dom->getElementsByTagName('QueryKey')
        ->item(0)->textContent,
    ];
    return $result['count'];

  }

}