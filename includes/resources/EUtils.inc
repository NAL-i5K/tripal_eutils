<?php

/**
 * Factory class which returns the appropriate NCBI resource provider.
 *
 * @Class EUtils
 */
class EUtils {

  /**
   * Tracks what objects have been imported so far.
   *
   * @var array
   *
   * structure of this array will be
   *
   *  [ncbi_db_name => [ $accession (uid) => $record_id]] = chado base record
   */
  static public $visited = [];

  /**
   * Don't insert into database.
   *
   * @var bool
   */
  private $preview = FALSE;

  /**
   * Queries and parses an NCBI record.
   *
   * @param string $db
   *   NCBI database.
   * @param string $accession
   *   Numeric only accession.
   *
   * @return mixed
   *   Chado object record.
   *
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

    $data = (new EUtilsXMLParserFactory())->get($db)->parse($xml);

    // When previewing, we want to return the parser values instead.
    // TODO: we miss out on querying linked records.
    // Secondarily linked records wont be reported.
    if ($this->preview) {

      $formatter = (new EUtilsFormatterFactory())->get($db);
      return $formatter->format($data);
    }

    $repository = (new EUtilsRepositoryFactory())->get($db);

    $record = $repository->create($data);

    static::$visited[$db][$accession] = $record;

    return $record;

  }

  /**
   * Sets the object to not insert, but only preview the XML.
   *
   * @param bool $preview
   *   TRUE will set to preview mode, FALSE unsets.
   */
  public function setPreview($preview = TRUE) {
    $this->preview = $preview;
  }

  /**
   * Returns the appropriate NCBI query method given the database.
   *
   * @param string $db
   *   NCBI database.
   *
   * @return \EFetch|\ESearch|\ESummary
   *   NCBI DB query object.
   *
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
