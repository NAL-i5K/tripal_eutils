<?php

abstract class EUtilsRepository {

  /**
   * Chado base table for this repository.  For example, project, biosample,
   * analysis
   *
   * @var string
   */
  protected $base_table = NULL;

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
        throw new Exception(
          'Required field ' . $field . ' is missing in ' . $class_name
        );
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
    return db_select('chado.dbxref', 'd')->fields('d')->condition(
      'dbxref_id', $id
    )->execute()->fetchObject();
  }

  /**
   * Look up an accession in chado.dbxref. Retrieves record from cache
   * if predetermined.
   *
   * @param string $name  The accession identifier (dbxref.accession).
   * @param int    $db_id Name of the DB ID.
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
   * @throws \Exception
   */
  public function createProperty($cvterm_id, $value) {
    $this->validateBaseData();

    $record = [
      'table' => $this->base_table,
      'id'    => $this->base_record_id,
    ];

    $property = [
      'type_id' => $cvterm_id,
      'value'   => $value,
    ];

    $options = [];

    return chado_insert_property($record, $property, $options);
  }

  /**
   * Creates a new accession record if does not exist and attaches it to
   * the given record.
   *
   * @param array $accession
   *
   * @return mixed
   * @throws \Exception
   */
  public function createAccession($accession) {
    if (!isset($accession['db'])) {
      throw new Exception(
        'DB not provided for accession ' . $accession['value']
      );
    }

    $db = $this->getDB('NCBI ' . $accession['db']);

    if (empty($db)) {
      $db = $this->getDB($accession['db']);
    }

    if (empty($db)) {
      throw new Exception(
        "Unable to find DB NCBI {$accession['db']}. Please create the DB first."
      );
    }

    $dbxref = $this->getAccessionByName($accession['value'], $db->db_id);

    if (!empty($dbxref)) {
      $this->createDBXref($accession['values'], $db->db_id);

      return $dbxref;
    }

    if (!empty($db)) {
      $id = db_insert('chado.dbxref')->fields(
        [
          'db_id'     => $db->db_id,
          'accession' => $accession['value'],
        ]
      )->execute();

      $this->createDBXref($accession['value'], $db->db_id);

      return static::$cache['accessions'][$accession['value']] =
        $this->getAccessionByID($id);
    }

    return NULL;
  }

  /**
   * Inserts the dbxref into the appropriate linker table, eg, project_dbxref.
   * dbxrefs are formatted db:accession.
   *
   * @param array $accession
   * @param int   $db_id
   *
   * @return bool
   * @throws \Exception
   */
  private function createDBXref($accession, $db_id) {
    $this->validateBaseData();

    $dbxref = [
      'accession' => $accession,
      'db_id'     => $db_id,
    ];

    return chado_associate_dbxref(
      $this->base_table, $this->base_record_id, $dbxref
    );
  }

  /**
   * Associates the XML with the record via the local:full_ncbi_xml term.
   *
   * @param string $xml string as returned by SimpleXMLElement.
   */
  public function createXMLProp($xml) {
    if (!isset(static::$cache['accessions']['local:full_ncbi_xml'])) {
      static::$cache['accessions']['local:full_ncbi_xml'] =
        tripal_get_cvterm(['id' => 'local:full_ncbi_xml']);
    }

    $xml_term = static::$cache['accessions']['local:full_ncbi_xml'];

    return $this->createProperty($xml_term->cvterm_id, $xml);
  }

  /**
   * Get contact name.
   *
   * @param static $contact_name
   *
   * @return mixed
   * @throws \Exception
   */
  public function createContact($contact_name) {
    $this->validateBaseData();

    if (static::$cache['contacts'][$contact_name]) {
      $contact = static::$cache['contacts'][$contact_name];
    }
    else {
      $contact = db_select('chado.contact', 'C')->fields('C')->condition(
        'name', $contact_name
      )->fetchObject();

      if (empty($contact)) {
        $contact_id = db_insert('chado.contact')->fields(
          [
            'name' => $contact_name,
          ]
        );

        $contact = db_select('chado.contact', 'C')->fields('C')->condition(
          'contact_id', $contact_id
        )->fetchObject();
      }

      static::$cache['contacts'][$contact_name] = $contact;
    }

    return $contact;
  }

  /**
   * Set the record id.
   *
   * @param int $id
   *
   * @return $this
   */
  public function setBaseRecordId($id) {
    $this->base_record_id = $id;
    return $this;
  }

  /**
   * Set the base table.
   *
   * @param string $table
   *
   * @return $this
   */
  public function setBaseTable($table) {
    $this->base_table = $table;

    return $this;
  }

  /**
   * Verifies that both base_record_id and base_table are set.
   *
   * @throws \Exception
   */
  protected function validateBaseData() {
    if (is_null($this->base_record_id)) {
      throw new Exception('Base record id was not set.');
    }

    if (is_null($this->base_table)) {
      throw new Exception('Base table was not set.');
    }
  }
}
