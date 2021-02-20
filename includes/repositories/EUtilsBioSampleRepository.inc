<?php

/**
 * Class EUtilsBioSampleRepository.
 *
 * @ingroup repositories
 */
class EUtilsBioSampleRepository extends EUtilsRepository {

  /**
   * Required attributes when using the create method.
   *
   * @var array
   */
  protected $required_fields = [
    'name',
    'description',
  ];

  /**
   * Cache of data per run.
   *
   * @var array
   */
  protected static $cache = [
    'db' => [],
    'accessions' => [],
    'biosamples',
  ];

  /**
   * Takes data from the EUtilsBioSampleParser and creates the chado records
   * needed including biosample, accessions and props.
   *
   * @param array $data
   *
   * @return object
   *
   * @throws \Exception
   *
   * @see \EUtilsBioSampleParser::parse() to get the data array needed.
   */
  public function create(array $data) {
    // Throw an exception if a required field is missing.
    $this->validateFields($data);

    if (!$data['organism']) {
      throw new Exception(
        'Unable to create BioSample due to lack of organism information'
      );
    }

    // getOrganism throws an exception so without try/catch, we will
    // automatically fail and exit.
    $organism = NULL;
    $organism = $this->getOrganism($data['organism']['taxonomy_id']);

    // Create the contact or get it from db if already exists.
    $contact = NULL;
    $contact = $this->createContact($data['contact']);

    // Create the base record.
    $description = $this->makeDescription($data['description']);
    $bio_sample = $this->createBioSample(
      [
        'biosourceprovider_id' => $contact ? $contact->contact_id : NULL,
        'name' => $data['name'],
        'description' => $description,
        'taxon_id' => $organism ? $organism->organism_id : NULL,
      ]
    );

    // Set class base record stuff.
    $this->setBaseTable('biomaterial');
    $this->setBaseRecordId($bio_sample->biomaterial_id);

    // Create the accessions.
    $this->createAccessions($data['accessions']);

    // Add linked records.
    $projects = $this->getNCBIRecord('bioproject', $data['projects']);

    foreach ($projects as $project) {

      $this->linkProject($project->project_id);
    }

    // Create the props (from attributes)
    $this->createProps($data['attributes']);
    $this->createXMLProp($data['full_ncbi_xml']);

    return $bio_sample;
  }

  /**
   * Make the description string.
   *
   * @param string|array $description
   *   The parser returns an array of paragraphs (or string).  We store them in a single field.
   *
   * @return string
   *
   *   A string suitable for the chado biomaterial table description column.
   */
  protected function makeDescription($description) {
    if (is_array($description)) {
      return implode("\n", $description);
    }

    return $description;
  }

  /**
   * Create a bio sample record.
   *
   * @param array $data
   *   See chado.biomaterial schema.
   *
   * @return mixed
   *
   * @throws \Exception
   */
  public function createBioSample(array $data) {
    // Name is unique so find the biomaterial first.
    $biosample = $this->getBioSample($data['name']);

    if (!empty($biosample)) {
      return $biosample;
    }

    $id = db_insert('chado.biomaterial')->fields($data)->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.biomaterial record');
    }

    $query = chado_db_select('biomaterial', 'B');

    $biosample = $query->fields('B')->condition(
      'biomaterial_id', $id
    )->execute()->fetchObject();

    return static::$cache['biosamples'][$biosample->name] = $biosample;
  }

  /**
   * Get biosample from db or cache.
   *
   * @param string $name
   *   Name for the name field of biomaterial table.
   *
   * @return mixed
   *   The Chado biosample record.
   */
  public function getBioSample($name) {
    // If the biosample is available in our static cache, return it.
    if (isset(static::$cache['biosamples'][$name])) {
      return static::$cache['biosamples'][$name];
    }

    // Find the biosample and add it to the cache.
    $query = chado_db_select('biomaterial', 'b');

    // Name is in fact a unique key.
    $biosample = $query->fields('b')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();

    if ($biosample) {
      return static::$cache['biosamples'][$name] = $biosample;
    }
  }

  /**
   * Creates a set of accessions attaches them with the given biosample.
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
      }
      catch (Exception $exception) {
        // Log the error and continue.
      }
    }

    return $data;
  }

  /**
   * Iterates through the attributes array and creates properties.
   *
   * @param array $attributes
   *
   *   CVterm info from the Attributes area.
   *
   * @throws \Exception
   */
  public function createProps(array $attributes) {

    $mapper = new TagMapper('biosample');
    foreach ($attributes as $attribute) {

      $term_name = $mapper->getLabel($attribute);
      $value = $attribute['value'];

      // TODO: the term lookup class should handle this instead.
      $cvterm = chado_get_cvterm(['id' => 'ncbi_properties:' . $term_name]);
      $cvterm_id = $cvterm->cvterm_id;
      $this->createProperty($cvterm_id, $value);
    }
  }

  /**
   * Link a project to this biomaterial.
   *
   * @param string $project_id
   *   Chado.project project_id.
   */
  protected function linkProject(string $project_id) {
    $values = [
      'project_id' => $project_id,
      'biomaterial_id' => $this->base_record_id,
    ];

    $exists = chado_generate_var('biomaterial_project', $values);
    if (!$exists) {
      chado_insert_record('biomaterial_project', $values);
    }

  }

}
