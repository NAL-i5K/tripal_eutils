<?php

class EUtilsBioProjectRepository extends EUtilsRepositoryInterface {

  /**
   * Required attributes when using the create method.
   *
   * @var array
   */
  protected $required_fields = [
    'name',
    'description',
  ];

  protected $base_table = 'project';


  /**
   * Cache of data per run.
   *
   * @var array
   */
  protected static $cache = [
    'db' => [],
    'accessions' => [],
    'projects',
  ];

  /**
   * Takes data from the EUtilsBioProjectParser and creates the chado records
   * needed including project, accessions and props.
   *
   * @param array $data
   *
   * @return object
   * @throws \Exception
   * @see \EUtilsBioProjectParser::parse() to get the data array needed.
   */
  public function create($data) {
    // Throw an exception if a required field is missing
    $this->validateFields($data);

    // Create the base record
    $description = is_array($data['description']) ? implode("\n",
      $data['description']) : $data['description'];

    $project = $this->createproject([
      'name' => $data['name'],
      'description' => $description,
    ]);

    $this->base_record_id = $project->project_id;

    $this->createAccessions($project, $data['accessions']);

    $this->insertProps($project, $data['attributes']);

    return $project;
  }

  /**
   * Create a project record.
   *
   * @param array $data See chado.project schema
   *
   * @return mixed
   * @throws \Exception
   */
  public function createProject(array $data) {
    // Name is unique so find project.
    $project = $this->getProject($data['name']);

    if (!empty($project)) {
      return $project;
    }

    $id = db_insert('chado.project')->fields([
      'name' => $data['name'] ?? '',
      'description' => $data['description'] ?? '',
    ])->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.project record');
    }

    $project = db_select('chado.project', 't')
      ->fields('t')
      ->condition('project_id', $id)
      ->execute()
      ->fetchObject();

    return static::$cache['projects'][$project->name] = $project;
  }

  /**
   * Get project from db or cache.
   *
   * @param string $name
   *
   * @return null
   */
  public function getProject($name) {
    // If the project is available in our static cache, return it
    if (isset(static::$cache['projects'][$name])) {
      return static::$cache['projects'][$name];
    }

    // Find the project and add it to the cache
    $project = db_select('chado.project', 'p')
      ->fields('p')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();

    if ($project) {
      return static::$cache['projects'][$name] = $project;
    }

    return NULL;
  }

  public function insertProps($project, $properties) {

    foreach ($properties as $property) {

      $cvterm_id = $property['cvterm_id'];
      $value = $property['value'];

      $this->insertProperty($cvterm_id, $value);

    }

    return TRUE;
  }

  /**
   * Creates a set of accessions attaches them with the given project.
   *
   * @param array $accessions
   *
   * @return array
   */
  public function createAccessions(array $accessions) {
    $data = [];

    foreach ($accessions as $accession) {
      try {
        $data[] = $this->createAccession($accession);
      } catch (Exception $exception) {
        // For the time being, ignore all exceptions
      }
    }
    return $data;
  }

  /**
   * Creates a new accession record if does not exist and attaches it to
   * the given project.
   *
   * @param object $accession
   *
   * @return mixed
   * @throws \Exception
   */
  public function createAccession($accession) {

    $project_id = $this->base_record_id;

    if (!isset($accession['db'])) {
      throw new Exception('DB not provided for accession ' . $accession['value']);
    }

    $db = $this->getDB('NCBI ' . $accession['db']);

    if (empty($db)) {
      throw new Exception('Unable to find DB NCBI ' . $accession['db'] . '. Please create DB first.');
    }

    $dbxref = $this->getAccessionByName($accession['value'], $db->db_id);

    if (!empty($dbxref)) {
      $this->linkProjectToAccession($project_id,
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


}
