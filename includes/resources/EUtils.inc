<?php

/**
 * @defgroup resources Classes for querying NCBI.
 * @{
 * Some explanation.
 */

/**
 * Factory class which returns the appropriate NCBI resource provider.
 *
 * @ingroup resources
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
  protected $preview = FALSE;

  /**
   * Whether to create linked records.
   *
   * This is passed down to the repository to handle.
   *
   * @var bool
   */
  protected $create_linked_records;

  /**
   *
   */
  public function __construct($create_linked_records = TRUE) {
    $this->create_linked_records = $create_linked_records;
  }

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
    $backup_accession = $accession;
    $accession = $this->convertAccessionsToUID($db, $accession);

    if (!$accession) {
      throw new Exception('Unable to find UID for ' . $db . ':' . $backup_accession);
    }

    if (isset(static::$visited[$db][$accession])) {
      return static::$visited[$db][$accession];
    }

    $provider = $this->getResourceProvider($db);
    $provider->addParam('id', $accession);
    if ($db == 'pubmed') {
      // For some reason we need to specify XML for pubmed.
      $provider->addParam('retmode', 'xml');
    }

    $response = $provider->get();
    $this->checkResponseSuccess($response, $db, $accession);
    $xml = $response->xml();

    $data = (new EUtilsXMLParserFactory())->get($db)->parse($xml);

    // When previewing, we want to return the parser values instead.
    if ($this->preview) {
      $formatter = (new EUtilsFormatterFactory())->get($db);
      return $formatter->format($data);
    }

    $repository = (new EUtilsRepositoryFactory($this->create_linked_records))->get($db);

    $variables = ['!db' => $db, '!accession' => $accession];
    tripal_report_error('tripal_eutils', TRIPAL_INFO, 'Inserting record into Chado: !db: !accession', $variables, ['print' => TRUE]);
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

  /**
   * Checks the accession and converts to uid accession if neccesary.
   *
   * @param string $accession
   *   The accession for the NCBI record.
   *
   * @return string
   */
  private function convertAccessionsToUID(string $db, string $accession) {

    if (preg_match("/[a-z]/i", $accession)) {

      // We have letters and need to convert to uid.
      $provider = new ESearch($db);
      $provider->addParam('retmode', 'xml');
      $provider->addParam('field', 'accession');
      $response = $provider->get();

      $this->checkResponseSuccess($response, $db, $accession);
      $xml = $response->xml();

      $count = (int) $xml->Count;
      if ($count !== 1) {

        $provider = new ESearch($db);
        $provider->addParam('retmode', 'xml');
        $provider->addParam('term', $accession . ' AND (latest[filter])');
        $response = $provider->get();
        $this->checkResponseSuccess($response, $db, $accession);
        $xml = $response->xml();

        $count = (int) $xml->Count;

        if ($count !== 1) {

          return FALSE;
        }
      }
      $accession = (string) $xml->IdList->Id;
    }

    return $accession;
  }

  /**
   * Checks that the resource was found.
   *
   * @param $response
   *   Response object.
   * @param string $db
   *   NCBI db string.
   * @param string $accession
   *   Accession.
   *
   * @throws \Exception
   */
  private function checkResponseSuccess($response, string $db, string $accession) {
    if (!$response->isSuccessful()) {
      $error_message = $response->hasError() ? $response->errorMessage() : 'Status: ' . $response->status();
      $variables = ['!db' => $db, '!accession' => $accession];
      $options = ['print' => TRUE];
      tripal_report_error('tripal_eutils', TRIPAL_ERROR, 'Cannot fetch NCBI record: !db : !accession', $variables, $options);

      throw new Exception('ERROR Could not make request: ' . $error_message);
    }
  }

}
