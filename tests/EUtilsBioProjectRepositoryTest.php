<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
class EUtilsBioProjectRepositoryTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * @group bioproject
   * @throws \Exception
   */
  public function testCreatingBioProject() {
    $repo = new \EUtilsBioProjectRepository();

    // Make sure creating a new one works.
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

    // Make sure creating a new one works.
    $name = 'Canis lupus familiaris: Reference genome sequence';
    $result = $repo->create($project);

    $this->assertObjectHasAttribute('project_id', $result);

    $props = db_select('chado.projectprop', 't')
      ->fields('t')
      ->condition('t.project_id', $result->project_id)
      ->execute()
      ->fetchAll();

    // At a minimum we have the raw XML prop.
    $this->assertNotEmpty($props);

    $xml_term = tripal_get_cvterm(['id' => 'local:full_ncbi_xml']);

    $props = db_select('chado.projectprop', 't')
      ->fields('t')
      ->condition('t.project_id', $result->project_id)
      ->condition('t.type_id', $xml_term->cvterm_id)
      ->execute()
      ->fetchObject();

    // At a minimum we have the raw XML prop.
    $this->assertNotFalse($props);
  }

  /**
   * @group pubs
   * @throws \Exception
   */
  public function testProjectAddsPubs() {


    $project = factory('chado.project')->create();

    $repo_master = new \EUtilsBioProjectRepository(FALSE);

    $repo = reflect($repo_master);
    $repo->base_record_id = $project->project_id;


    $pubs = ['9023104', '22751099'];
    $title= 'tRNAscan-SE: a program for improved detection of transfer RNA genes in genomic sequence.';

    $pubs = $repo->createPubs($pubs);

    $pub = db_select('chado.pub', 'p')->fields('p')->condition('p.title', $title)->execute()->fetchObject();

    $this->assertNotFalse($pub);

    $link = db_select('chado.project_pub', 'p')->fields('p')->condition('p.project_id', $project->project_id)->execute()->fetchObject();
    $this->assertNotFalse($link);
    $this->assertEquals($pub->pub_id, $link->pub_id);
  }
}
