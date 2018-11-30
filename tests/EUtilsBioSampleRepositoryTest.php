<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsBioSampleRepositoryTest extends TripalTestCase{

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testThatGettingDBWorks() {
    $repo = new \EUtilsBioSampleRepository();

    $this->assertNull($repo->getDB(uniqid()));

    $db = factory('chado.db')->create();

    $this->assertNotEmpty($repo->getDB($db->name));
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testGettingAccessionWorks() {
    $dbxref = factory('chado.dbxref')->create();
    $repo = new \EUtilsBioSampleRepository();

    // Testing accessions that exist.
    $id = $dbxref->dbxref_id;
    $name = $dbxref->accession;
    $db_id = $dbxref->db_id;

    $accession = $repo->getAccessionByID($id);
    $this->assertNotEmpty($accession);

    $accession = $repo->getAccessionByName($name, $db_id);
    $this->assertNotEmpty($accession);

    // Testing accessions that do not exist
    $name = uniqid();
    $db_id = random_int(0, 1000);

    $accession = $repo->getAccessionByName($name, $db_id);
    $this->assertEmpty($accession);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testThatCreatingAccessionsWork() {
    $bio_sample = factory('chado.biomaterial')->create();
    $repo = new \EUtilsBioSampleRepository();
    $accession_value = uniqid();

    $accession = $repo->createAccession($bio_sample, [
      'value' => $accession_value,
      'db' => 'BioSample',
    ]);

    $this->assertNotEmpty($accession);

    $dbxref = db_select('chado.dbxref', 'd')
      ->fields('d')
      ->condition('dbxref_id', $accession->dbxref_id)
      ->execute()
      ->fetchObject();

    $this->assertNotEmpty($dbxref);

    $this->assertEquals($accession_value, $dbxref->accession);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testCreatingBioSample() {
    $repo = new \EUtilsBioSampleRepository();

    // Make sure creating a new one works
    $name = uniqid();
    $biosample = $repo->createBioSample([
      'name' => $name,
      'description' => uniqid(),
    ]);

    $this->assertNotEmpty($biosample);
    $this->assertEquals($biosample->name, $name);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testCreatingPrexistingBioSample() {
    $repo = new \EUtilsBioSampleRepository();
    $biosample = factory('chado.biomaterial')->create();

    $biosample2 = $repo->createBioSample([
      'name' => $biosample->name,
    ]);

    $this->assertEquals($biosample2->biomaterial_id,
      $biosample->biomaterial_id);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testEntireCreationProcess() {
    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    $parser = new \EUtilsBioSampleParser();
    $repo = new \EUtilsBioSampleRepository();

    foreach (glob("$path/examples/biosamples/*.xml") as $file) {
      $data = $parser->parse(simplexml_load_file($file));
      $repo->create($data);

      // Check that BioSample got created
      $biosample = $repo->getBioSample($data['name']);
      $this->assertNotEmpty($biosample, "Unable to find Bio Sample: {$data['name']}. File $file");
    }
  }
}
