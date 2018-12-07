<?php

/**
 * Class EUtils
 *
 * Factory class which returns the appropriate NCBI resource provider.
 *
 */

class EUtils {

  /**
   * @var array
   *
   * structure of this array will be
   *
   *  [ncbi_db_name => [ $accession (uid) => $record_id]] = chado base record
   */
  static public $visited = [];


  /**
   * @param $db
   * @param $accession
   *
   * @return mixed
   * @throws \Exception
   */

  public function get($db, $accession) {

    if (isset(static::$visited[$db][$accession])) {
      return static::$visited[$db][$accession];
    }

    $provider = $this->getResourceProvider($db);
    $provider->addParam('id', $accession);

    $response = $provider->get();

    if (!$response->isSuccessful()) {
      throw new Exception('ERROR Could not make request status ' . $response->status());
    }
    $xml = $response->xml();

    $data = (new EUtilsXMLParserFactory())->get($db, $xml);
    
    $repository = (new EutilsRepositoryFactory())->get($db);

    $record = $repository->create($data);

    static::$visited[$db][$accession] = $record;

    return $record;

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
