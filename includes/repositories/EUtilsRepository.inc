<?php

/**
 * Base EUtils repository class. All repository classes must extend this class.
 *
 * @defgroup repositories
 */
abstract class EUtilsRepository {


  /**
   * TripalJob object for error logging.
   *
   * @var TripalJob
   */
  protected $job = NULL;


  /**
   * Whether a DB:accession was visited during this run time.
   *
   * @var array
   */
  public static $visited = [];

  /**
   * Chado base table for this repository.
   *
   * For example, project, biosample, analysis.
   *
   * @var string
   */
  protected $base_table = NULL;

  /**
   * Chado base table record_id.  For example the project.project_id.
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
   * Whether to create linked records.
   *
   * @var bool
   */
  protected $create_linked_records;

  /**
   * EUtilsRepository constructor.
   *
   * @param bool $create_linked_records
   *   Whether to create linked records.
   */
  public function __construct($create_linked_records = TRUE) {
    $this->create_linked_records = $create_linked_records;
  }

  /**
   * Create a new resource.
   *
   * @param array $data
   *   Formatted data returned from the parser.
   *
   * @return object
   *   The chado base record object.
   */
  abstract public function create(array $data);

  /**
   * Determine whether required fields are provided.
   *
   * @param array $data
   *   Formatted data from parser.
   *
   * @throws \Exception
   */
  public function validateFields(array $data) {
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
   * Search for accession by name.
   *
   * Look up an accession in chado.dbxref. Retrieves record from cache if
   * predetermined.
   *
   * @param string $name
   *   The accession identifier (dbxref.accession).
   * @param int $db_id
   *   Name of the DB ID.
   *
   * @return object
   *   Accession record.
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
   *   The name of database.
   *
   * @return mixed The database object
   */
  public function getDB($name) {
    if (isset(static::$cache['db'][$name])) {
      return static::$cache['db'][$name];
    }

    $db = db_query(
      'SELECT * FROM chado.db WHERE UPPER(name) = :name',
      [':name' => strtoupper($name)]
    )->fetchObject();

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
   *
   * @throws \Exception
   */
  public function createProperty($cvterm_id, $value) {
    $this->validateBaseData();

    $record = [
      'table' => $this->base_table,
      'id' => $this->base_record_id,
    ];

    $property = [
      'type_id' => $cvterm_id,
      'value' => $value,
    ];

    $options = [];

    return chado_insert_property($record, $property, $options);
  }

  /**
   * Create a dbxref record.
   *
   * Creates a new accession record if it does not exist, and attaches
   * it to the given record.
   *
   * @param array $accession
   *   Expected keys: db and value, where the full accession is db:value.
   *
   * @return mixed
   *   An accession object
   *
   * @throws \Exception
   */
  public function createAccession(array $accession) {
    if (!isset($accession['db'])) {
      if (!isset($accession['db_label'])) {
        throw new Exception(
          'DB not provided for accession ' . $accession['value']
        );
      }
      else {
        return NULL;
      }
    }

    // Try getting the db record with the prefix NCBI.
    $db = $this->getDB("NCBI {$accession['db']}");

    // Not found! Try getting the DB without any prefixes.
    if (empty($db)) {
      $db = $this->getDB($accession['db']);
    }

    // Still not found! Alert the user.
    if (empty($db)) {
      throw new Exception(
        "Unable to find DB \"NCBI {$accession['db']}\" and \"{$accession['db']}\". Please create the DB first."
      );
    }

    $dbxref = $this->createDBXref($accession['value'], $db->db_id);

    if (!empty($dbxref)) {
      return static::$cache['accessions'][$accession['value']] = $dbxref;
    }

    return NULL;
  }

  /**
   * Create an assocation record.
   *
   * Inserts the dbxref into the appropriate linker table, eg, project_dbxref.
   * dbxrefs are formatted db:accession.
   *
   * @param string $accession
   *   Name.
   * @param int $db_id
   *   DB id.
   *
   * @return object
   *   the new dbxref object
   *
   * @throws \Exception
   */
  private function createDBXref($accession, $db_id) {
    $this->validateBaseData();

    $dbxref = [
      'accession' => $accession,
      'db_id' => $db_id,
    ];

    chado_associate_dbxref(
      $this->base_table, $this->base_record_id, $dbxref
    );

    return $this->getAccessionByName($accession, $db_id);
  }

  /**
   * Associates the XML with the record via the local:full_ncbi_xml term.
   *
   * @param string $xml
   *   string as returned by SimpleXMLElement.
   *
   * @return bool
   *   True on creation
   *
   * @throws \Exception
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
   *   The contact name.
   *
   * @return mixed
   *   contact record
   *
   * @throws \Exception
   */
  public function createContact($contact_name) {
    if (isset(static::$cache['contacts']) && isset(static::$cache['contacts'][$contact_name])) {
      $contact = static::$cache['contacts'][$contact_name];
    }
    else {
      $contact = db_select('chado.contact', 'C')->fields('C')->condition(
        'name', $contact_name
      )->execute()->fetchObject();

      if (empty($contact)) {
        $contact_id = db_insert('chado.contact')->fields(
          [
            'name' => $contact_name,
          ]
        )->execute();

        if (!$contact_id) {
          throw new Exception(
            'Unable to create a contact for ' . $contact_name
          );
        }

        $contact = db_select('chado.contact', 'C')->fields('C')->condition(
          'contact_id', $contact_id
        )->execute()->fetchObject();
      }

      static::$cache['contacts'][$contact_name] = $contact;
    }

    return $contact;
  }

  /**
   * Set the Chado record id.
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
   * Sets the TripalJob for error logging.
   *
   * @param TripalJob|NULL $job
   *   Tripal Job object.
   */
  public function setJob(TripalJob $job = NULL) {
    $this->job = $job;
  }

  /**
   * Set the Chado base table.
   *
   * @param string $table
   *   Valid examples include 'organism' , 'biomaterial', 'project'.
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

  /**
   * Given an ncbi taxon organism, return the organism (and create if
   * necessary).
   *
   * @param $accession
   *   NCBITaxon accession for organism.
   *
   * @return mixed
   *
   * @throws \Exception
   */
  public function getOrganism($accession) {

    $organism = $this->organismQuery($accession);

    if ($organism) {
      tripal_report_error('tripal_eutils', TRIPAL_INFO, 'Linking pre-existing organism !organism', ['!organism' => $accession], [
        'print' => TRUE,
        'job' => $this->job,
      ]);
      return $organism;
    }
    // Note: import_existing = TRUE causes the loader to time out.
    $run_args = [
      'taxonomy_ids' => $accession,
      'import_existing' => FALSE,
    ];

    module_load_include(
      'inc', 'tripal_chado', 'includes/TripalImporter/TaxonomyImporter'
    );

    $importer = new \TaxonomyImporter();
    $importer->create($run_args, $file_details = []);
    $variables = ['!db' => 'NCBI Taxon (organism)', '!accession' => $accession];
    tripal_report_error('tripal_eutils', TRIPAL_INFO, 'Inserting record into Chado: !db: !accession', $variables, [
      'print' => TRUE,
      'job' => $this->job,
    ]);

    $importer->run();

    $organism = $this->organismQuery($accession);

    if (!$organism) {
      throw new Exception('Could not create organism record for ' . $accession);
    }

    return $organism;
  }

  /**
   * Generate query to getch an organism.
   *
   * Query to check if an organism exists in the DB based on the NCBITaxon
   * accession.
   *
   * @param string $accession
   *   Accession name.
   *
   * @return object
   *   An organism
   */
  private function organismQuery($accession) {
    $db = chado_get_db(['name' => 'NCBITaxon']);

    $query = db_select('chado.organism_dbxref', 'od');
    $query->join('chado.organism', 'o', 'o.organism_id = od.organism_id');
    $query->fields('o');
    // $query->condition('od.organism_id', $munk->organism_id);.
    $query->join('chado.dbxref', 'd', 'd.dbxref_id = od.dbxref_id');
    $query->condition('d.accession', $accession);
    $query->condition('d.db_id', $db->db_id);
    $organism = $query->execute()->fetchObject();

    return $organism;
  }

  /**
   * Fetch and create NCBI records of the type DB.
   *
   * @param string $db
   *   The db name.
   * @param array $accessions
   *   The accessions for this db.
   *
   * @return array
   *   An array of chado base records, as returned by a repository.
   *
   * @throws \Exception
   */
  public function getNCBIRecord($db, array $accessions) {
    $return = [];

    foreach ($accessions as $accession) {
      if (isset(static::$visited[$db . ':' . $accession])) {
        $return[] = static::$visited[$db . ':' . $accession];
        continue;
      }

      $record = $this->lookupNcbiInChado($db, $accession);
      if ($record) {
        tripal_report_error('tripal_eutils', TRIPAL_INFO, 'Using existing record for  !db : !accession', [
          '!db' => $db,
          '!accession' => $accession,
        ], ['print' => TRUE, 'job' => $this->job]);

        $return[] = static::$visited[$db . ':' . $accession] = $record;
        continue;
      }

      if ($this->create_linked_records) {
        // Do not loop through linked records recursively. We only want the
        // the first level of linked records.
        // Logging already happens higher up in EUtils.
        $record = (new EUtils(FALSE))->get($db, $accession);
        $return[] = static::$visited[$db . ':' . $accession] = $record;
      }
    }

    return $return;
  }

  /**
   * Looks up a base record based on the accession.
   *
   *   The dbxref is the only reliable way to look up a record since each
   *   repository uses different parts of the XML for the base record name, etc.
   *
   * @param string $db
   *   NCBI (not Chado) db name.
   * @param string $accession
   *   NCBI accession.  This might be uid, or long form.
   *
   * @return mixed
   *   returns the chado object or FALSE.
   */
  public function lookupNcbiInChado(string $db, string $accession) {

    $base_lookup = [
      'bioproject' => 'project',
      'biosample' => 'biomaterial',
      'assembly' => 'analysis',
    ];

    $base_table = $base_lookup[$db];

    $dbx_table = $base_table . '_dbxref';

    $column = $base_table . '_id';

    $query = db_select('chado.' . $dbx_table, 'dbxl');

    $query->join('chado.' . $base_table, 'b', 'b.' . $column . ' = dbxl.' . $column);
    $query->fields('b');
    $query->join('chado.dbxref', 'dbx', 'dbx.dbxref_id = dbxl.dbxref_id');
    $query->condition('dbx.accession', $accession);
    $result = $query->execute()->fetchObject();

    return $result;
  }

  /**
   * Links project to the record, assuming a project_ linker table.
   *
   * @param array $projects
   *   Array of base chado record project objects.
   */
  public function linkProjects($projects) {

    $base_record = $this->base_record_id;

    $base_table = $this->base_table;
    // TODO: if we genericize this, would it always be linked this way?
    $table = 'project_' . $base_table;
    foreach ($projects as $project) {

      $exists = db_select('chado.' . $table, 'lt')->fields('lt')->condition(
        'project_id', $project->project_id
      )->condition($base_table . '_id', $base_record)->execute()->fetchObject();
      if (!$exists) {

        db_insert('chado.' . $table)->fields(
          [
            'project_id' => $project->project_id,
            $base_table . '_id' => $base_record,
          ]
        )->execute();
      }
    }
  }

}
