<?php

class EUtilsBioSampleRepository extends EUtilsRepository{

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
   * @throws \Exception
   * @see \EUtilsBioSampleParser::parse() to get the data array needed.
   */
  public function create($data) {
    // Throw an exception if a required field is missing
    $this->validateFields($data);

    // Create the base record
    $description = is_array($data['description']) ? implode("\n",
      $data['description']) : $data['description'];

    $bio_sample = $this->createBioSample([
      'name' => $data['name'],
      'description' => $description,
    ]);

    // Set class base record stuff
    $this->setBaseTable('biomaterial');
    $this->setBaseRecordId($bio_sample->biomaterial_id);

    // Create the accessions
    $this->createAccessions($data['accessions']);

    // Create the props (from attributes)
    $this->createProps($data['attributes']);
    $this->createXMLProp($data['full_ncbi_xml']);

    return $bio_sample;
  }

  /**
   * Create a bio sample record.
   *
   * @param array $data See chado.biomaterial schema
   *
   * @return mixed
   * @throws \Exception
   */
  public function createBioSample(array $data) {
    // Name is unique so find the biomaterial first.
    $biosample = $this->getBioSample($data['name']);

    if (!empty($biosample)) {
      return $biosample;
    }

    $id = db_insert('chado.biomaterial')->fields([
      'name' => $data['name'] ?? '',
      'description' => $data['description'] ?? '',
    ])->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.biomaterial record');
    }

    $biosample = db_select('chado.biomaterial', 'B')
      ->fields('B')
      ->condition('biomaterial_id', $id)
      ->execute()
      ->fetchObject();

    return static::$cache['biosamples'][$biosample->name] = $biosample;
  }

  /**
   * Get biosample from db or cache.
   *
   * @param string $name
   *
   * @return null
   */
  public function getBioSample($name) {
    // If the biosample is available in our static cache, return it
    if (isset(static::$cache['biosamples'][$name])) {
      return static::$cache['biosamples'][$name];
    }

    // Find the biosample and add it to the cache
    $biosample = db_select('chado.biomaterial', 'b')
      ->fields('b')
      ->condition('name', $name)
      ->execute()
      ->fetchObject();

    if ($biosample) {
      return static::$cache['biosamples'][$name] = $biosample;
    }

    return NULL;
  }

  /**
   * Creates a set of accessions attaches them with the given biosample.
   *
   * @param object $bio_sample The BioSample created by createBioSample()
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
   * Iterates through the attributes array and creates properties.
   *
   * @param $attributes
   *
   * CVterm info from the Attributes area
   */
  public function createProps($attributes) {
    foreach ($attributes as $attribute) {
      $term_name = $attribute['harmonized_name'];
      $value = $attribute['value'];

      //TODO: the term lookup class should handle this instead.
      $cvterm = tripal_get_cvterm(['id' => 'ncbi_properties:' . $term_name]);
      $cvterm_id = $cvterm->cvterm_id;
      $this->createProperty($cvterm_id, $value);
    }
  }
}
