<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsBioProjectRepositoryTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group bioproject
   * @throws \Exception
   */
  public function testCreatingBioProject() {
    $repo = new \EUtilsBioProjectRepository();

    // Make sure creating a new one works
    $name = uniqid();
    $project = $repo->createProject([
      'name' => $name,
      'description' => uniqid(),
    ]);

    $this->assertNotEmpty($project);
    $this->assertEquals($project->name, $name);

    $result = db_select('chado.project', 't')
      ->fields('t')
      ->condition('t.name', $name)
      ->execute()
      ->fetchobject();

    $this->assertNotFalse($result);
  }


  /**
   * @group bioproject
   */
  public function testProjectFromXML() {


    $file = __DIR__ . '/../examples/bioprojects/12384_bioproject.xml';

    $parser = new \EUtilsBioProjectParser();
    $project = $parser->parse(simplexml_load_file($file));

    $repo = new \EUtilsBioProjectRepository();

    // Make sure creating a new one works
    $name = 'Canis lupus familiaris: Reference genome sequence';
    $result = $repo->create($project);

    $this->assertObjectHasAttribute('project_id', $result);


    $props = db_select('chado.projectprop', 't')
      ->fields('t')
      ->condition('t.project_id', $result->project_id)
      ->execute()
      ->fetchAll();

    //at a minimum we have the raw XML prop.
    $this->assertNotEmpty($props);

    $xml_term = tripal_get_cvterm(['id' => 'local:full_ncbi_xml']);

    $props = db_select('chado.projectprop', 't')
      ->fields('t')
      ->condition('t.project_id', $result->project_id)
      ->condition('t.type_id', $xml_term->cvterm_id)
      ->execute()
      ->fetchObject();

    //at a minimum we have the raw XML prop.
    $this->assertNotFalse($props);
  }
}
