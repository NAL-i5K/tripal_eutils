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

    if (array_key_exists('linked_records', $data)) {
      $links = $data['linked_records'];
      if (array_key_exists('biosample', $links)) {
        $biosamples = $links['biosample'];
        $this->createLinkedRecords($biosamples, 'biosample');
      }
      if (array_key_exists('assembly', $links)) {
        $assemblies = $links['assembly'];
        $this->createLinkedRecords($assemblies, 'assembly');
      }
      // Pubs are linked records but we always want to import them
      // since they won't cause a link-loop.
      if (array_key_exists('pubs', $links)) {
        $pubs = $links['pubs'];
        $this->createPubs($pubs);
      }
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
      } catch (Exception $exception) {
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
          break;

        case 'assembly':
          $this->linkAssembly($object);
          break;
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
   * Links an analysis to the base project record.
   *
   * @param $record
   *   Assembly record object.
   */
  private function linkAssembly($record) {
    $analysis_id = $record->analysis_id;

    $exists = db_select('chado.project_analysis', 't')
      ->fields('t')
      ->condition('project_id', $this->base_record_id)
      ->condition('analysis_id', $analysis_id)
      ->execute()
      ->fetchObject();
    if (!$exists) {
      $values = [
        'analysis_id' => $analysis_id,
        'project_id' => $this->base_record_id,
      ];
      chado_insert_record('project_analysis', $values);
    }
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

      if ($pub and property_exists($pub, 'pub_id')) {
        $values = [
          'project_id' => $this->base_record_id,
          'pub_id' => $pub->pub_id,
        ];
        $exists = db_select('chado.project_pub', 't')
          ->fields('t')
          ->condition('project_id', $this->base_record_id)
          ->condition('pub_id', $pub->pub_id)
          ->execute()
          ->fetchObject();

        if (!$exists) {
          chado_insert_record('project_pub', $values);
        }
      }
    }
  }

}
