<?php

/**
 * Takes parsed bioproject XMLs and creates chado.projects.
 *
 * @ingroup repositories.
 */
class EUtilsBioProjectRepository extends EUtilsRepository {

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
    'projects' => [],
  ];

  /**
   * Creates a project and linked records.
   *
   * @param array $data
   *   Data from bioproject parser.
   *
   * @return object
   *   chado project record.
   *
   * @throws \Exception
   */
  public function create(array $data) {
    // Throw an exception if a required field is missing.
    $this->validateFields($data);

    // Create the base record.
    $description = is_array($data['description']) ? implode("\n",
      $data['description']) : $data['description'];

    $project = $this->createproject([
      'name' => $data['name'],
      'description' => $description,
    ]);

    $this->base_record_id = $project->project_id;
    $this->createAccessions($data['accessions']);
    $this->createProps($data['attributes']);

    $this->createXMLProp($data['full_ncbi_xml']);

    if ($this->create_linked_records) {

      $links = $data['linked_records'];
      $pubs = $links['pubs'];
      $biosamples = $links['biosample'];
      if (!empty($biosamples) && $this->create_linked_records) {
        $this->createLinkedRecords($biosamples, 'biosample');
      }

      // Pubs are linked records but we always want to import them since they wont cause a link-loop.
      if (!empty($pubs)) {
        $this->createPubs($pubs, 'pub');
      }

      // TODO: what about organism and assembly?
    }

    return $project;
  }

  /**
   * Create a project record.
   *
   * @param array $data
   *   See chado.project schema.
   *
   * @return mixed
   *
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
    // If the project is available in our static cache, return it.
    if (isset(static::$cache['projects'][$name])) {
      return static::$cache['projects'][$name];
    }

    // Find the project and add it to the cache.
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

  /**
   * Iterate through the properties and insert.
   *
   * TODO:  How do we get the accessions from what we have here?
   * What we probably have for project is a set of XML attributes or tags...
   *
   * @param array $properties
   *   Properties in form machine name => value.
   *
   * @return bool
   *   True if successful.
   *
   * @throws \Exception
   */
  public function createProps(array $properties) {
    foreach ($properties as $property_name => $value) {
      $accession = 'local:' . $property_name;

      // TODO:  this is not what we want to do.  we want to be smarter about mapping the terms...
      $cvterm = chado_get_cvterm(['id' => $accession]);

      $this->createProperty($cvterm->cvterm_id, $value);
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

    foreach ($accessions as $db => $accession) {
      try {
        $data[] = $this->createAccession(['db' => $db, 'value' => $accession]);
      }
      catch (Exception $exception) {
        // For the time being, ignore all exceptions.
      }
    }
    return $data;
  }

  /**
   * Links this base record to various other records.
   *
   * @param array $records
   *   Array of record ids.
   * @param string $type
   *   The NCBI record type.
   *
   * @throws \Exception
   */
  public function createLinkedRecords(array $records, string $type) {

    $objects = $this->getNCBIRecord($type, $records);
    foreach ($objects as $object) {

      switch ($type) {
        case 'biosample':
          $this->linkBiomaterial($object);
      }
    }
  }

  /**
   * Links this record to a biosample/biomaterial.
   *
   * @param $record
   *   A record object returned from getNCBIRecord.
   */
  public function linkBiomaterial($record) {
    $biomaterial_id = $record->biomaterial_id;

    db_insert('chado.biomaterial_project')->fields([
      'biomaterial_id' => $biomaterial_id,
      'project_id' => $this->base_record_id,
    ]);

  }

  /**
   * Inserts and links publications to the project.
   *
   * @param array $pubs
   *   An array of pubmed IDs.
   *
   * @throws \Exception
   */
  private function createPubs(array $pubs) {

    foreach ($pubs as $accession) {

      $search = new EUtils(FALSE);
      $pub = $search->get('pubmed', $accession);

      if (property_exists($pub, 'pub_id')) {

        db_insert('chado.project_pub')->fields(['project_id' => $this->base_record_id, 'pub_id' => $pub->pub_id])->execute();
      }
    }
  }

}
