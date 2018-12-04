<?php

abstract class EUtilsRepository {

  /**
   * Chado base table for this repository.  For example, project, biosample,
   * analysis
   *
   * @var string
   */
  protected $base_table = '';

  /**
   * Chado base table record_id.  for example the project.project_id.
   *
   * @var int
   */
  protected $base_record_id = NULL;

  /**
   * List of fields required for the base Chado record.
   *
   * @var array
   */
  protected $required_fields = [];

  /**
   * Array of Chado properties.
   *
   * @var array
   */
  protected $properties = [];

  /**
   * Array of DBXrefs.
   *
   * @var array
   */
  protected $dbxrefs = [];


  /**
   * Cache of inserted primary and secondary chado records.  Used to speed up
   * multiple look-ups.
   *
   * @var array
   */
  protected static $cache = [];


  /**
   * Create a new resource.
   *
   * @param array $data
   *
   * @return object
   */
  abstract public function create($data);

  /**
   * Determine whether required fields are provided.
   *
   * @param array $data
   *
   * @throws \Exception
   */
  public function validateFields($data) {
    foreach ($this->required_fields as $field) {
      if (!isset($data[$field])) {
        $class_name = get_class($this);
        throw new Exception('Required field ' . $field . ' is missing in ' . $class_name);
      }
    }
  }


  /**
   * Get accession by dbxref id.
   *
   * @param int $id
   *
   * @return mixed
   */
  public function getAccessionByID($id) {
    return db_select('chado.dbxref', 'd')
      ->fields('d')
      ->condition('dbxref_id', $id)
      ->execute()
      ->fetchObject();
  }

  /**
   * Look up an accession in chado.dbxref. Retrieves record from cache
   * if predetermined.
   *
   * @param string $name The accession identifier (dbxref.accession).
   * @param int $db_id Name of the DB ID.
   *
   * @return mixed
   */
  public function getAccessionByName($name, $db_id) {
    if (isset(static::$cache['accessions'][$name])) {
      return static::$cache['accessions'][$name];
    }

    $accession = db_select('chado.dbxref', 'd')
      ->fields('d')
      ->condition('accession', $name)
      ->condition('db_id', $db_id)
      ->execute()
      ->fetchObject();

    if (!empty($accession)) {
      static::$cache['accessions'][$accession->accession] = $accession;
    }

    return $accession;
  }

  /**
   * Get chado.db record by name. Retrieves data from cache if predetermined.
   *
   * @param string $name
   *
   * @return mixed
   */
  public function getDB($name) {
    if (isset(static::$cache['db'][$name])) {
      return static::$cache['db'][$name];
    }

    $db = db_select('chado.db', 'db')
      ->fields('db')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();

    if ($db) {
      return static::$cache['db'][$name] = $db;
    }

    return NULL;
  }


  /**
   * Inserts a property associated with the interface using the tripal API.
   *
   * @param $cvterm_id
   * @param $value
   *
   * @return bool
   */
  public function createProperty($cvterm_id, $value) {

    $base_record_id = $this->base_record_id;
    $base_table = $this->base_table;

    $record = ['table' => $base_table, 'id' => $base_record_id];
    $property = [
      'type_id' => $cvterm_id,
      'value' => $value,
    ];

    $options = [];

    return chado_insert_property($record, $property, $options);

  }


  /**
   * Creates a new accession record if does not exist and attaches it to
   * the given record.
   *
   * @param object $accession
   *
   * @return mixed
   * @throws \Exception
   */
  public function createAccession($accession) {

    $record_id = $this->base_record_id;

    if (!isset($accession['db'])) {
      throw new Exception('DB not provided for accession ' . $accession['value']);
    }

    $db = $this->getDB('NCBI ' . $accession['db']);

    if (empty($db)) {
      throw new Exception('Unable to find DB NCBI ' . $accession['db'] . '. Please create DB first.');
    }

    $dbxref = $this->getAccessionByName($accession['value'], $db->db_id);

    if (!empty($dbxref)) {
      $this->linkBaseRecordToAccession($record_id,
        $dbxref->dbxref_id);

      return $dbxref;
    }

    if (!empty($db)) {
      $id = db_insert('chado.dbxref')->fields([
        'db_id' => $db->db_id,
        'accession' => $accession['value'],
      ])->execute();

      $this->insertDBXref($accession['value'], $db->db_id);

      return static::$cache['accessions'][$accession['value']] = $this->getAccessionByID($id);
    }

    return NULL;
  }


  /**
   * Inserts tdbxref into the appropriate linker table, eg, project_dbxref.
   * dbxrefs are formatted db:accession.
   *
   * @param $accession
   *
   * @param $db_id
   */
  private function insertDBXref($accession, $db_id) {

    $dbxref = [
      'accession' => $accession,
      'db_id' => $db_id,
    ];

    chado_associate_dbxref($this->base_table, $this->base_record_id, $dbxref);

  }

  /**
   * Associates the XML with the record via the local:full_ncbi_xml term.
   * @param $xml
   * xml string as returned by simpleXML.
   */
  public function createXMLProp($xml) {

    $xml_term = tripal_get_cvterm(['id' => 'local:full_ncbi_xml']);

    $this->createProperty($xml_term->cvterm_id, $xml);

  }
}
