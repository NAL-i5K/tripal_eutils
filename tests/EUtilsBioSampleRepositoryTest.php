<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsBioSampleRepositoryTest extends TripalTestCase {

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
    $repo   = new \EUtilsBioSampleRepository();

    // Testing accessions that exist.
    $id    = $dbxref->dbxref_id;
    $name  = $dbxref->accession;
    $db_id = $dbxref->db_id;

    $accession = $repo->getAccessionByID($id);
    $this->assertNotEmpty($accession);

    $accession = $repo->getAccessionByName($name, $db_id);
    $this->assertNotEmpty($accession);

    // Testing accessions that do not exist
    $name  = uniqid();
    $db_id = random_int(0, 1000);

    $accession = $repo->getAccessionByName($name, $db_id);
    $this->assertEmpty($accession);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testThatCreatingAccessionsWork() {
    $bio_sample      = factory('chado.biomaterial')->create();
    $repo            = new \EUtilsBioSampleRepository();
    $accession_value = uniqid();

    // Base record not set so expect an exception.
    $this->expectException(\Exception::class);
    $repo->createAccession(
      [
        'value' => $accession_value,
        'db'    => 'BioSample',
      ]
    );

    $accession =
      $repo->setBaseRecordId($bio_sample->biomaterial_id)->setBaseTable(
        'biomaterial'
      )->createAccession(
        [
          'value' => $accession_value,
          'db'    => 'BioSample',
        ]
      );
    $this->assertNotEmpty($accession);
    $this->assertObjectHasAttribute('dbxref_id', $accession);

    $dbxref = db_select('chado.dbxref', 'd')->fields('d')->condition(
      'dbxref_id', $accession->dbxref_id
    )->execute()->fetchObject();

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
    $name      = uniqid();
    $biosample = $repo->createBioSample(
      [
        'name'        => $name,
        'description' => uniqid(),
      ]
    );

    $this->assertNotEmpty($biosample);
    $this->assertEquals($biosample->name, $name);
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testCreatingPrexistingBioSample() {
    $repo      = new \EUtilsBioSampleRepository();
    $biosample = factory('chado.biomaterial')->create();

    $biosample2 = $repo->createBioSample(
      [
        'name' => $biosample->name,
      ]
    );

    $this->assertEquals(
      $biosample2->biomaterial_id, $biosample->biomaterial_id
    );
  }

  /**
   * @group biosample
   * @throws \Exception
   */
  public function testEntireCreationProcess() {
    $path   = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    $parser = new \EUtilsBioSampleParser();
    $repo   = new \EUtilsBioSampleRepository();

    foreach (glob("$path/examples/biosamples/*.xml") as $file) {
      $data = $parser->parse(simplexml_load_file($file));
      $repo->create($data);

      // Check that BioSample got created
      $biosample = $repo->getBioSample($data['name']);
      $this->assertNotEmpty(
        $biosample, "Unable to find Bio Sample: {$data['name']}. File $file"
      );

      // Check that Biosample props got inserted
      $props = db_select('chado.biomaterialprop', 't')->fields('t')->condition(
        't.biomaterial_id', $biosample->biomaterial_id
      )->execute()->fetchAll();
      $this->assertNotEmpty($props);

      // Some of our XML dont have properties.
      if ($biosample->name == 'Rubber genome') {
        $this->assertGreaterThan(2, count($props));
      }

      // Make sure the organism got created
      $organism = db_select('chado.organism', 'O')->fields('O')->condition(
        'O.organism_id', $biosample->taxon_id
      )->execute()->fetchObject();
      $this->assertNotEmpty($organism);
    }
  }

  /**
   * @group organism
   * @throws \Exception
   */
  public function testGetOrganism() {

    $repo = new \EUtilsBioSampleRepository();

    $accession = '499546';

    $organism = $repo->getOrganism($accession);

    $this->assertNotFalse($organism);
    $this->assertEquals('Tamias', $organism->genus);
  }
}
