<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsXmlParserTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testInit() {
    $connection = new \EUtils();

    //$connection->setDB('bioproject');
    //https://www.ncbi.nlm.nih.gov/bioproject/PRJNA506315
    $result = $connection->lookupAccessions('bioproject', ['506315']);

    $this->assertInstanceOf(\SimpleXMLElement::class, $result);
  }

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
   * @group wip
   */
  public function testBioProjectAttributesParser() {
    $parser = new \EUtilsXMLParser();

    $string = ' <Project>
        <ProjectID>
            <ArchiveID accession="PRJNA506315" archive="NCBI" id="506315"/>
            <LocalID>bp0</LocalID>
            <LocalID>bp0</LocalID>
        </ProjectID>
        <ProjectDescr>
            <Name>Bordetella pertussis strain:BPD2</Name>
            <Title>Bordetella pertussis strain:BPD2 Genome sequencing</Title>
            <Description>Complete genome sequence of Bordetella pertussis</Description>
            <LocusTagPrefix biosample_id="SAMN10457990">EHO96</LocusTagPrefix>
        </ProjectDescr>
        <ProjectType>
            <ProjectTypeSubmission>
                <Target capture="eWhole" material="eGenome" sample_scope="eMonoisolate">
                    <Organism species="520" taxID="520">
                        <OrganismName>Bordetella pertussis</OrganismName>
                        <Strain>BPD2</Strain>
                        <Supergroup>eBacteria</Supergroup>
                    </Organism>
                </Target>
                <Method method_type="eSequencing"/>
                <Objectives>
                    <Data data_type="eSequence"/>
                </Objectives>
                <IntendedDataTypeSet>
                    <DataType>genome sequencing</DataType>
                </IntendedDataTypeSet>
                <ProjectDataTypeSet>
                    <DataType>genome sequencing</DataType>
                </ProjectDataTypeSet>
            </ProjectTypeSubmission>
        </ProjectType>
    </Project>';

    $xml = simplexml_load_string($string);

    $submission_info = $parser->bioProject($xml);

    $this->assertNotEmpty($submission_info);

    $string = '<Project>
        <ProjectID>
        </ProjectID>
        <ProjectDescr>
        </ProjectDescr>
        <ProjectType>
        </ProjectType>
        <Waffle>I should cause an exception</Waffle>
    </Project>';

    $xml = simplexml_load_string($string);

    $this->expectException('Exception');
    $submission_info = $parser->bioProject($xml);
    $this->assertFalse($submission_info);
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

    }
  }

  /**
   * @group assembly
   * @dataProvider AssemblyProvider
   *
   * @param $path - the path to the xml file
   * @param $base_keys = key => value pairs.  used to check that each key contains what is expected.
   *
   * @todo add some key value pairs for base_keys
   */

  public function testAssemblyParser($path, $base_keys) {


    $parser = new \EUtilsAssemblyParser();
    $assembly = $parser->parse(simplexml_load_file($path));

    $this->assertArrayHasKey('name', $assembly);
    $this->assertArrayHasKey('accessions', $assembly);
    $this->assertArrayHasKey('attributes', $assembly);
    $this->assertArrayHasKey('description', $assembly);
    $this->assertTrue(is_array($assembly['attributes']));
    $this->assertArrayHasKey('ignored', $assembly);

    $attributes = $assembly['attributes'];

    $this->assertArrayHasKey('stats', $attributes);
    $this->assertArrayHasKey('files', $attributes);


    $this->assertNotNull($assembly['name']);
    $this->assertNotNull($assembly['accessions']);
    $this->assertNotNull($assembly['attributes']);
    $this->assertNotNull($assembly['description']);

  }

  /**
   *
   */
  public function AssemblyProvider() {

    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');

    $files = [
      [
        $path . "/examples/assembly/1949871_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/2004951_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/317138_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/524058_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/557018_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/559011_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/751381_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
      [
        $path . "/examples/assembly/91111_assembly.xml",
        [
          'name' => '',
          'accessions' => '',
          'attributes' => '',
          'description' => '',
        ],
      ],
    ];

    return $files;
  }
}
