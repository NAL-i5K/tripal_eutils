<?php

/**
 * Takes parsed bioproject XMLs and creates chado.projects.
 *
 * @class EutilsBioProjectRepository
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

    // TODO: no project_organism table...
    // $organism = $this->getOrganism($data['linked_records']['organism']);
    // $this->linkOrganism($organism);
    $this->createXMLProp($data['full_ncbi_xml']);

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

}
