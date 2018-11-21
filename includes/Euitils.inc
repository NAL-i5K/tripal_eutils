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

  private $url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/';

  //efetch.fcgi?db=<database>&id=<uid_list>&rettype=<retrieval_type>
  //&retmode=<retrieval_mode>

  private $db = NULL;

  private $valid_dbs = ['pubmed', 'bioproject', 'biosample', 'genome'];

  private $dom = null;


  /**
   * Set the NCBI db.  Only the following DBs are allowed:
   * 'pubmed' - use the core publication importer instead.
   *
   * @param $db
   */
  public function set_db($db) {

    $valid_dbs = array_flip($this->valid_dbs);

    if (!array_key_exists($db, $valid_dbs)) {
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
   * @todo Only supports single accession.  Instead use array, but think about
   *   throttling issue first.
   * @return array
   */
  public function lookup_accessions($accessions) {


    $params = [
      'db' => $this->db,
      'retmode' => 'xml',
      'rettype' => 'xml',
      'id' => implode(',', $accessions),
    ];

    //Input: List of UIDs (&id); Entrez database (&db); Retrieval type (&rettype); Retrieval mode (&retmode)


    $address = $this->url . 'efetch.fcgi?';

    $headers = [];
    $method = 'GET';

    $data = [
      'db' => $this->db,
     // 'rettype' => 'json',
      'id' => implode(',', $accessions),
    ];


    $options = [
      'headers' => $headers,
      'method' => $method,
      'data' => http_build_query($data),
    ];


    //$response = drupal_http_request($address, $options);
    //
    //var_dump($response);
    // return $response;

    $url = $this->url . 'efetch.fcgi?' . http_build_query($params);
    $dom = new DOMDocument;
    $dom->load($url);


    $this->dom = $dom;
    //todo:we just check if we got xml back.  not great.
    if ($dom->xmlVersion){
      return TRUE;
    }
  }

  public function getAttributes($accession = null){

    $dom = $this->dom;

    $x= simplexml_import_dom($dom);

    //TODO:  Handle parsing differently for different database types.

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