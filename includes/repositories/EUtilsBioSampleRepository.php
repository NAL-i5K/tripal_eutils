<?php

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
    'db'         => [],
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

    if (!$data['organism']) {
      throw new Exception(
        'Unable to create BioSample due to lack of organism information'
      );
    }

    // getOrganism throws an exception so without try/catch, we will
    // automatically fail and exit
    $organism = $this->getOrganism($data['organism']['taxonomy_id']);
    $organism = $organism->organism_id;

    // Create the contact or get it from db if already exists
    $contact = $this->createContact($data['contact']);

    // Create the base record
    $description = $this->makeDescription($data['description']);
    $bio_sample  = $this->createBioSample(
      [
        'biosourceprovider_id' => $contact->contact_id,
        'name'                 => $data['name'],
        'description'          => $description,
        'taxon_id'             => $organism,
      ]
    );

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
   * Make the description string.
   *
   * @param array|string $description
   *
   * @return string
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

    $id = db_insert('chado.biomaterial')->fields($data)->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.biomaterial record');
    }

    $biosample = db_select('chado.biomaterial', 'B')->fields('B')->condition(
      'biomaterial_id', $id
    )->execute()->fetchObject();

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
   * @param array  $accessions
   *
   * @return array
   */
  public function createAccessions(array $accessions) {
    $data = [];

    foreach ($accessions as $accession) {
      try {
        $data[] = $this->createAccession($accession);
      } catch (Exception $exception) {
        // Log the error and continue
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
   *
   * @throws \Exception
   */
  public function createProps($attributes) {
    foreach ($attributes as $attribute) {
      $term_name = $attribute['harmonized_name'];
      $value     = $attribute['value'];

      // TODO: the term lookup class should handle this instead.
      $cvterm    = tripal_get_cvterm(['id' => 'ncbi_properties:' . $term_name]);
      $cvterm_id = $cvterm->cvterm_id;
      $this->createProperty($cvterm_id, $value);
    }
  }
}
