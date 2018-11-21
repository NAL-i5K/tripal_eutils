<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;


class EutilsXmlParserTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testInit() {

    $parser = new \EutilsXMLParser('bioproject');
    $this->assertNotNull($parser);

    $connection = new \Euitils();

    $connection->set_db('bioproject');
    //https://www.ncbi.nlm.nih.gov/bioproject/PRJNA506315
    $result = $connection->lookup_accessions(['506315']);

    $parser->loadXML($result);
  }


  public function testParserBasicsBioProject() {

    $parser = new \EutilsXMLParser('bioproject');
    $xml = simplexml_load_file(__DIR__ . '/example_files/example_pertussis.xml');
    $parser->loadXML($xml);
  }


  /**
   * @group wip
   * @throws \Exception
   */
  public function testBioProject_submission_key(){
    $parser = new \EutilsXMLParser('bioproject');

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

    $parser_r = reflect($parser);
    $submission_info = $parser_r->bioproject_submission($xml);

    $this->assertNotEmpty($submission_info);
    $this->assertArrayHasKey('organization', $submission_info);
    $this->assertEquals('Christian Medical College',$submission_info['organization']);
  }


}
