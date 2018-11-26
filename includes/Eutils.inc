<?php

class Eutils{

  /**
   * Eutils interface.
   *
   */

  /**
   * The Eutils url
   *
   * @var string
   */

  private $url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?';

  //efetch.fcgi?db=<database>&id=<uid_list>&rettype=<retrieval_type>
  //&retmode=<retrieval_mode>

  private $db = NULL;

  private $valid_dbs = [
    'pubmed',
    'bioproject',
    'biosample',
    'genome',
    'assembly',
  ];

  private $dom = NULL;

  /**
   * Set the NCBI db.  Only the following DBs are allowed:
   * 'pubmed' - use the core publication importer instead.
   *
   * @param $db
   */
  public function setDB($db) {

    $valid_dbs = array_flip($this->valid_dbs);

    if (!array_key_exists($db, $valid_dbs)) {
      tripal_set_message('Invalid db: !db', [!'db' => $db], TRIPAL_ERROR);
      return FALSE;
    }

    $this->db = $db;
    //if db isn't supported by efetch, do something...

    if ($db == 'assembly') {
      $this->url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?';
    }
    return TRUE;
  }

  /**
   * Search the set DB for the provided accessions.
   *
   * @param $accessions - array of accession strings.
   *
   * @todo Only supports single accession.  Instead use array, but think about
   *   throttling issue first.
   * @return array
   */
  public function lookupAccessions($accessions) {


    $params = [
      'db' => $this->db,
      'retmode' => 'xml',
      'rettype' => 'xml',
      'id' => implode(',', $accessions),
    ];

    if ($this->db == 'assembly') {
      $params = [
        'db' => $this->db,
        'id' => implode(',', $accessions),
      ];
    }

    $url = $this->url . http_build_query($params);

    var_dump($url);
    $dom = new DOMDocument;
    $dom->load($url);

    $this->dom = $dom;
    //todo:we just check if we got xml back.  not great.
    if ($dom->xmlVersion) {

      $x = simplexml_import_dom($dom);

      return $x;
    }
  }

  public function getAttributes($accession = NULL) {

    $dom = $this->dom;

    $x = simplexml_import_dom($dom);
    //TODO:  Handle parsing differently for different database types.

  }

  public function checkStatus() {
    $params = [
      'db' => 'pmc',
      'retmode' => 'xml',
      'term' => 'all[SB]',
      'retstart' => 0,
      'retmax' => 0,
      'usehistory' => 'y',
    ];

    $url = $this->url . 'esearch.fcgi?' . http_build_query($params);

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