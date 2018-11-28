<?php

class EUtilsBioSampleRepository extends EUtilsRepositoryInterface{

  /**
   * Required attributes when using the create method.
   *
   * @var array
   */
  protected $required_fields = [
    'name',
    'description',
    'attributes',
    'accessions',
  ];

  /**
   * Cache of data per run.
   *
   * @var array
   */
  protected static $cache = [
    'db' => [],
    'accessions' => [],
  ];

  /**
   * @param array $data
   *
   * @return object
   * @throws \Exception
   */
  public function create($data) {
    $this->validateFields($data);

    // Create the base record
    $bio_sample = $this->createBioSample([
      'name' => $data['name'],
      'description' => $data['description'],
    ]);

    // Create the accessions
    $this->createAccessions($bio_sample, $data['accessions']);

    // Create the props (from attributes)
    $this->createProps($bio_sample, $data['attributes']);

    return $bio_sample;
  }

  public function createBioSample(array $data) {
    $id = db_insert('chado.biomaterial')->values($data)->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.biomaterial record');
    }

    return db_select('chado.biomaterial', 'B')
      ->condition('B.biomaterial_id', $id)
      ->execute()
      ->fetchObject();
  }

  public function createAccessions($bio_sample, array $accessions) {
    $data = [];

    foreach ($accessions as $accession) {
      $data[] = $this->createAccession($bio_sample, $accession);
    }

    return $data;
  }

  public function createAccession($bio_sample, $accession) {
    $dbxref = $this->getAccessionByName($accession);

    if ($dbxref) {
      $this->linkBioSampleToAccession($bio_sample->biomaterial_id,
        $dbxref->dbxref_id);

      return $dbxref;
    }

    $id = db_insert('chado.dbxref')->values([
      'db_id' => $this->getDB('NCBI BioSample')->db_id,
      'accession' => $accession,
    ])->execute();

    static::$cache['accessions'][$accession] = $this->getAccessionByID($id);

    $this->linkBioSampleToAccession($bio_sample->biomaterial_id, $id);

    return static::$cache['accessions'][$accession];
  }

  public function linkBioSampleToAccession($bio_sample_id, $accession_id) {
    return db_insert('chado.biomaterial_dbxref')->values([
      'biomaterial_id' => $bio_sample_id,
      'dbxref_id' => $accession_id,
    ])->execute();
  }

  public function getAccessionByID($id) {
    return db_select('chado.dbxref', 'd')
      ->fields('d')
      ->condition('dbxref_id', $id)
      ->execute()
      ->fetchObject();
  }

  public function getAccessionByName($name) {
    if (isset(static::$cache['accessions'][$name])) {
      return static::$cache['accessions'][$name];
    }

    $accession = db_select('chado.dbxref', 'd')
      ->fields('d')
      ->condition('accession', $name)
      ->execute()
      ->fetchObject();

    if ($accession) {
      static::$cache['accessions'][$accession->accession] = $accession;
    }

    return $accession;
  }

  public function getDB($name) {
    if (isset(static::$cache['db'][$name])) {
      return static::$cache['db'][$name];
    }

    $db = db_select('chado.db', 'db')
      ->fields('db')
      ->condition('name', $name)
      ->fetchObject();

    if ($db) {
      static::$cache['db'][$name] = $db;
    }

    return static::$cache['db'][$name];
  }

  public function createProps($props) {

  }
}
