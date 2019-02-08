<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsXmlParserTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * AssemblyProvider creates array of XML and keys values to test.
   */
  protected $assembly_xmls = NULL;


  /**
   * @throws \Exception
   */
  public function testBioProject_submission_key() {
    $parser = new \EUtilsBioProjectParser();

    $submission_test_string = '<Submission last_update="2018-11-21" submission_id="SUB4827559" submitted="2018-11-21">
        <Description>
            <!-- Submitter information has been removed -->
            <Organization role="owner" type="institute">
                <Name>Christian Medical College</Name>
                <!-- Contact information has been removed -->
            </Organization>
            <Access>public</Access>
        </Description>
        <Action action_id="SUB4827559-3"/>
        <Action action_id="SUB4827559-bp0"/>
        <Action action_id="SUB4827559-bs0"/>
    </Submission>';

    $xml = simplexml_load_string($submission_test_string);

    $submission_info = $parser->bioProjectSubmission($xml);

    $this->assertNotEmpty($submission_info);
    $this->assertArrayHasKey('organization', $submission_info);
    $this->assertEquals('Christian Medical College',
      $submission_info['organization']);
  }

  /**
   * @test
   * @group project
   *
   */
  public function testBioProjectGeneralParsing() {

    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    foreach (glob("$path/examples/bioprojects/*.xml") as $file) {
      $parser = new \EUtilsBioProjectParser();
      $project = $parser->parse(simplexml_load_file($file));

      $this->assertArrayHasKey('name', $project);
      $this->assertArrayHasKey('accessions', $project);
      $this->assertArrayHasKey('attributes', $project);
      $this->assertArrayHasKey('description', $project);
      $this->assertArrayHasKey('linked_records', $project);

      $this->assertTrue(is_array($project['accessions']));
      $this->assertTrue(is_array($project['attributes']));

      $accessions = $project['accessions'];
      $props = $project['tagProps'];


      $linked_records = $project['linked_records'];

      $this->assertArrayHasKey('organism', $linked_records);

      /**
       * These will be used to lookup biosamples and assemblies.
       */
      if (isset($linked_records['locus_tag_prefix'])) {
        $locus_tag = $linked_records['locus_tag_prefix'];

        $this->assertArrayHasKey('value', $locus_tag);
        $this->assertArrayHasKey('attributes', $locus_tag);

      }

    }
  }

  /**
   * @test
   * @group biosamples
   */
  public function testBioSampleParser() {
    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    foreach (glob("$path/examples/biosamples/*.xml") as $file) {
      $parser = new \EUtilsBioSampleParser();
      $biosample = $parser->parse(simplexml_load_file($file));

      $this->assertArrayHasKey('name', $biosample);
      $this->assertArrayHasKey('accessions', $biosample);
      $this->assertArrayHasKey('attributes', $biosample);
      $this->assertArrayHasKey('description', $biosample);
      $this->assertTrue(is_array($biosample['accessions']));
      $this->assertTrue(is_array($biosample['attributes']));

      if ($biosample['name'] == 'SAMN02953603'){
        // This sample has a linked project.  Was it parsed?
        $this->assertArrayHasKey('projects', $biosample);
        $this->assertNotEmpty($biosample['projects']);
      }
    }
  }

  /**
   * @group assembly
   * @dataProvider AssemblyProvider
   *
   * @param $path - the path to the xml file
   * @param $base_keys = key => value pairs.  used to check that each key
   *   contains what is expected.
   *
   * @todo add some key value pairs for base_keys
   */

  public function testAssemblyParser($path, $base_keys) {

    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();
    //We mock the FTP call to speed up the test.
    $ftp_response = ['# Assembly method:' => 'a method, v1.0'];
    $parser->expects($this->once())
      ->method('getFTPData')
      ->will($this->returnValue($ftp_response));

    $assembly = $parser->parse(simplexml_load_file($path));

    $this->assertArrayHasKey('name', $assembly);
    $this->assertArrayHasKey('accessions', $assembly);
    $this->assertArrayHasKey('attributes', $assembly);
    $this->assertArrayHasKey('description', $assembly);
    $this->assertTrue(is_array($assembly['attributes']));
    $this->assertArrayHasKey('ignored', $assembly);
    $this->assertArrayHasKey('category', $assembly);

    $attributes = $assembly['attributes'];

    $this->assertArrayHasKey('stats', $attributes);
    $this->assertArrayHasKey('files', $attributes);
    $this->assertArrayHasKey('ftp_attributes', $attributes);

    $this->assertArrayHasKey('# Assembly method:', $attributes['ftp_attributes']);
    $this->assertNotNull($attributes['ftp_attributes']['# Assembly method:']);

    $this->assertNotNull($assembly['name']);
    $this->assertNotNull($assembly['accessions']);
    $this->assertNotNull($assembly['attributes']);
    $this->assertNotNull($assembly['description']);


    $accessions = $assembly['accessions'];

    $acc_master = $base_keys['accessions'];

    if (!empty($acc_master)) {
      //we only bother specifying keys for some of hte files.
      $this->assertEquals($acc_master, $accessions);
    }

  }

  /**
   *
   */
  public function AssemblyProvider() {


    if ($this->assembly_xmls) {
      return $this->assembly_xmls;
    }


    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');

    $files = [
      [
        $path . "/examples/assembly/1949871_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/2004951_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/317138_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/524058_assembly.xml",
        [
          'name' => '',
          'sourcename' => 'SAMN00744358',
          'accessions' => [
            'assembly' => [
              'Refseq' => '817508',
              'Genbank' => '524058',
              'WGS' => 'AGSK01',
            ],
            'taxon_accession' => '72004',
            'bioprojects' => ['74739', '221623'],
            'biosamples' => ['744358'],
          ],
          'submission_date' => '2013/01/09 00:00',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/557018_assembly.xml",
        [],
      ],
      [
        $path . "/examples/assembly/559011_assembly.xml",
        [],
      ],
      [
        $path . "/examples/assembly/751381_assembly.xml",
        [

        ],
      ],
      [
        $path . "/examples/assembly/91111_assembly.xml",
        [

        ],
      ],
    ];

    $this->assembly_xmls = $files;
    return $files;
  }
}
